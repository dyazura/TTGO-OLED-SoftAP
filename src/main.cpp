#include <Arduino.h>
#include <Wire.h> // Only needed for Arduino 1.6.5 and earlier

//Ported to ESP32
#ifdef ESP32
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiClient.h>
#define ESP_getChipId() ((uint32_t)ESP.getEfuseMac())
#define LED_ON HIGH
#define LED_OFF LOW
#else
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#define ESP_getChipId() (ESP.getChipId())
#define LED_ON LOW
#define LED_OFF HIGH
#endif

#include <AutoConnect.h>
#include <SSD1306Wire.h> // legacy: #include "SSD1306.h"
SSD1306Wire display(0x3c, 5, 4);

WebServer Server; // Replace with WebServer for ESP32
AutoConnect Portal(Server);

void rootPage()
{
  char content[] = "ESP32 AutoConnect";
  Server.send(200, "text/plain", content);
}

void setup()
{
  delay(1000);
  Serial.begin(115200); //Serial Init
  Serial.println();

  String chipID = String(ESP_getChipId(), HEX);
  chipID.toUpperCase();

  SSD1306Wire display(0x3c, 5, 4);
  display.init(); //SSD1306 Init
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "TTGO ESP32");
  display.drawString(0, 16, "EasyConnect");
  display.display();

  Server.on("/", rootPage);
  if (Portal.begin())
  {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());

    display.clear();
    display.drawString(0, 0, "WiFi connected");
    display.drawString(0, 16, WiFi.SSID());
    display.drawString(0, 32, WiFi.localIP().toString());
    display.drawString(0, 48, chipID);
    display.display();
  }
}

void heartBeatPrint(void)
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("+"); // + means connected to WiFi
    display.drawString(60, 120, "+");
    display.display();
  }
  else
  {
    Serial.print("-"); // - means not connected to WiFi
    display.drawString(60, 120, "-");
    display.display();
  }

  if (num == 80)
  {
    Serial.println();
    display.drawString(60, 120, " ");
    display.display();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(" ");
    display.drawString(60, 120, ".");
    display.display();
  }
}

//*******************************************
void check_status()
{
  static ulong checkstatus_timeout = 0;

#define HEARTBEAT_INTERVAL 10000L
  // Print hearbeat every HEARTBEAT_INTERVAL (10) seconds.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
  }
}

void loop()
{
  Portal.handleClient();
  //  check_status(); debug only
}