#ifndef HOSTNAME
#define HOSTNAME "home-right"
#endif

#define WIFI_SSID "<your ssid>>"
#define WIFI_PASSWORD "<your ssid password>"

#define OTA_HOSTNAME HOSTNAME
#define OTA_PASSWORD "<ota password>"

#define WEB_SOCKET_PORT 8080
#define WEB_SERVER_PORT 80

// Pin mapping. See
// https://tttapa.github.io/ESP8266/Chap04%20-%20Microcontroller.html
// https://www.electronicwings.com/nodemcu/nodemcu-gpio-with-arduino-ide
// https://i1.wp.com/www.teachmemicro.com/wp-content/uploads/2018/04/NodeMCUv3.0-pinout.jpg?ssl=1
// https://iotbytes.wordpress.com/nodemcu-pinout/

// LCD connections
// https://diyi0t.com/lcd-display-tutorial-for-arduino-and-esp8266/
// Wire    LCD  ESP8266
//---------------------
// blue    RS   D0
// red     EN   D2
// brown   D4   D3
// orange  D5   D5
// grey    D6   D6
// purple  D7   D7
//
// red     Vcc  5V
// white   GND  GND
#define LCD_RS        D0
#define SPRINKLER_PIN D1
#define LCD_EN        D2
#define LCD_D4        D3
#define LED_PIN       D4
#define LCD_D5        D5
#define LCD_D6        D6
#define LCD_D7        D7
#define UNUSED_1      D8
