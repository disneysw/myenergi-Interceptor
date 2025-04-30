#ifndef _WIFIMANAGER_H
#define _WIFIMANAGER_H


void initWifi();
void MQTT_callback(char* topic, byte* payload, unsigned int length);
void processMQTT();

#endif// _WIFIMANAGER_H