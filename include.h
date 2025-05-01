#ifndef _INCLUDE_H
#define _INCLUDE_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <HexDump.h>
#include "wifimanager.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include <PubSubClient.h>

#define debugInit
#define debugLoop
#define debugDecode
#define debugDecodeSub
#define decodeSubPacket3510
#define decodeSubPacket3601
#define decodeSubPacket3730
#define decodeSubPacket5a5a
#define decodeSubPacket7878

#define GREEN_TFT // Try uncommenting this line if you have issues with the TFT colors

#define CODENAME "Myenergi-Intecepter" // Update "upload_port = " in platformio.ini if you change this for OTA to work
#define PASSWORD "passwordTest"

#define MYENERGI_SERVER "15.197.208.241"
#define MYENERGI_HUB "192.168.1.133"

#define MQTT_SERVER "192.168.1.10"
#define MQTT_PORT 1883
#define MQTT_USERNAME "mqtt"
#define MQTT_PASSWORD "mqtt7"

extern WiFiUDP udp;

#endif //_INCLUDE_H