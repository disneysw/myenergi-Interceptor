#include "include.h"
#include "mqtt.h"

PubSubClient mqttClient;

char config_json[] = "";
/*
{"dev" :
     {"ids" : ["HUB"],
      "name" : CODENAME,
      "sw" : "1.1.1",
      "mf" : "Myenergi",
      "mdl" : "xxxxxx",
      "cu" : "http://192.168.1.48/index"},
 "name" : "voltage",
 "~" : "HARVI",
 "avty_t" : "~/connected",
 "uniq_id" : HARVI_SERIAL,
 "qos" : 1,
 "dev_cla" : "voltage",
 "unit_of_meas" : "V",
 "stat_t" : "~/voltage/get",
 "stat_cla" : "measurement"}
 */

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
#ifdef debugLoop
    Serial.println("MQTT callback called");
#endif
    // handle message arrived
}

void initMQTT()
{
#ifdef debugInit
    Serial.println("Starting WiFiManager in AP mode");
#endif
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setBufferSize(512);
    mqttClient.setKeepAlive(30);

#ifdef debugInit
    Serial.println("Sending homeassistant MQTT discovery message");
#endif

    // mqttClient.subscribe("homeassistant/#");

    // Send homeassistant discovery message (use with caution)
    // homeassistant/device/hub_serial/config
}

void mqttLoop()
{
    if (!mqttClient.connected()) // Check if we need to reconnect
    {
#ifdef debugLoop
        Serial.println("MQTT not connected");
#endif
        if (mqttClient.connect(CODENAME, MQTT_USERNAME, MQTT_PASSWORD))
        {
#ifdef debugLoop
            Serial.println("MQTT connected");
#endif
            Serial.println("MQTT connected");
            mqttClient.subscribe("homeassistant/#");
        }
    }
    else
    {
        mqttClient.loop();
    }
}