#ifndef _WEB_SOCKET_H
#define _WEB_SOCKET_H

#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>

#include "linkedlist.h"

// Abstract class
class WebSocketListener {
  public:
   virtual void onClientConnected(uint32_t num) = 0;
   virtual void onMessageFromClient(String message) = 0;
};

class WebSocket {
  public:
    typedef void (*OnMessageFromClient)(String msg);

  private:
    int port;
    AsyncWebServer server;
    AsyncWebSocket webSocket;
    MyLinkedList<uint32_t> clients;
    WebSocketListener* webSocketListener;

    void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                 AwsEventType type, void* arg, uint8_t* data, size_t len) {
      if (type == WS_EVT_CONNECT) {
        IPAddress ip = client->remoteIP();
        Serial.printf("[WebSocket/%u] Connected from %d.%d.%d.%d\n",
            client->id(), ip[0], ip[1], ip[2], ip[3]);
        clients.add(client->id());
        webSocketListener->onClientConnected(client->id());
      } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("[WebSocket/%u] Disconnected!\n", client->id());
        for (int i = 0; i < clients.size(); i++) {
          if (clients.get(i) == client->id()) {
            clients.remove(i);
            break;
          }
        }
      } else if (type == WS_EVT_DATA) {
        // data packet
        AwsFrameInfo* info = (AwsFrameInfo*) arg;
        if (info->final && info->index == 0 && info->len == len) {
          if (info->opcode == WS_TEXT) {
            data[len] = 0;
            this->onMessage(client->id(), (char*) data);
          }
        }
      }
    }

    void onMessage(uint32_t id, String msg) {
      if (msg.equals("ping")) {
        webSocket.text(id, "pong");
      } else {
        Serial.printf("[WebSocket/%u] Message received: ", id);
        Serial.println(msg);
        webSocketListener->onMessageFromClient(msg);
      }
    }

  public:
    WebSocket(int port) : server(port), webSocket("/") {
      this->port = port;
    }

    void setup(WebSocketListener* webSocketListener) {
      this->webSocketListener = webSocketListener;
      webSocket.onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client,
          AwsEventType type, void* arg, uint8_t* data, size_t len) { 
        this->onWebSocketEvent(server, client, type, arg, data, len); 
      });
      server.addHandler(&webSocket);
      server.begin();
      Serial.println("[WebSocket] Web socket server started");

      // Add service to MDNS-SD
      MDNS.addService("sprinkler", "udp", port);
      Serial.println("[Websocket] Service added to mDNS");
    }

    void handle() {
    }

    void send(uint32_t id, String msg) {
      Serial.printf("[WebSocket/%u] Sending: ", id);
      Serial.println(msg);
      webSocket.text(id, msg);
    }

    void sendAll(String msg) {
      // Serial.println("[WebSocket] Sending message to all clients: " + msg);
      for (int i = 0; i < clients.size(); i++) {
        webSocket.text(clients.get(i), msg);
      }
    }

    int numClients() {
      return clients.size();
    }
};

#endif