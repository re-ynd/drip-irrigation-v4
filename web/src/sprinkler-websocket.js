export class SprinklerWebSocket {
  static _getHostName() {
    const wsHostname = window.location.hostname;
    if (wsHostname === 'localhost' || wsHostname === '127.0.0.1') {
      return 'sprinkler-right.local';
    }
    return wsHostname;
  }

  static _getWebSocketAddress() {
    return `ws://${SprinklerWebSocket._getHostName()}:8080`;
  }

  static getInstance() {
    return _sprinklerWebSocketInstance;
  }

  constructor() {
    this._statusListeners = new Set();
    this._messageListeners = new Set();
    this._reconnect = false;
    this._socket = null;
    this._pingTimer = null;
    this._status = 'Disconnected';
  }

  addStatusListener(callback) {
    this._statusListeners.add(callback);
  }

  removeStatusListener(callback) {
    this._statusListeners.delete(callback);
  }

  addMessageListener(callback) {
    this._messageListeners.add(callback);
  }

  removeMessageListener(callback) {
    this._messageListeners.delete(callback);
  }

  connect() {
    if (this._socket) {
      return;
    }
    this._setStatus('Connecting to');
    this._reconnect = true;
    this._socket = new WebSocket(SprinklerWebSocket._getWebSocketAddress());
    this._socket.onerror = this._onError.bind(this);
    this._socket.onopen = this._onOpen.bind(this);
    this._socket.onclose = this._onClose.bind(this);
    this._socket.onmessage = this._onMessage.bind(this);
  }

  _pingWebSocket() {
    this.send('ping');
    this._pingTimer = setTimeout(() => this._pingWebSocket(), 3000);
  }

  _onError() {
    this._setStatus('Failed to connect to');
    setTimeout(() => this.connect(), 3000);
    this._socket = null;
  }

  _onOpen() {
    this._pingWebSocket();
  }

  _onClose() {
    this._socket = null;
    this._setStatus('Disconnected from');
    if (!this._reconnect) {
      return;
    }

    if (this._pingTimer) {
      clearTimeout(this._pingTimer);
      this._pingTimer = null;
      setTimeout(() => this.connect(), 3000);
    }
  }

  _onMessage(event) {
    if (event.data !== 'pong') {
      this._setStatus('Received data from');
      this._messageListeners.forEach((callback) => callback(event.data));
    } else {
      this._setStatus('Connected to');
    }
  }

  send(message) {
    if (this._socket) {
      this._setStatus('Sent data to');
      this._socket.send(message);
    }
  }

  disconnect() {
    this._setStatus('Disconnecting from');
    this._reconnect = false;
    if (this._socket) {
      this._socket.close();
    }
  }

  _setStatus(status) {
    this._status = `${status} ${SprinklerWebSocket._getHostName()}`;
    this._statusListeners.forEach((callback) => callback(this._status));
  }
}

const _sprinklerWebSocketInstance = new SprinklerWebSocket();