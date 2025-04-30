#include "include.h"
#include "wifimanager.h"
#include "decode.h"
#include "display.h"

WiFiManager wifiManager;

WiFiClient espClient;
// PubSubClient mqtt_client(MQTT_SERVER, MQTT_PORT, MQTT_callback, espClient);


bool shouldSaveConfig = false; // flag for saving data


void MQTT_callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

// ---------------------------------------------------
void saveConfigCallback()
{
  Serial.println("Should save config true");
  shouldSaveConfig = true;
}

// ---------------------------------------------------
void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

// ---------------------------------------------------
void initWifi()
{
  // wifiManager.resetSettings(); // wipe settings

  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setConfigPortalTimeout(180); // Config timeout

  char myenergi_server[41];
  sprintf(myenergi_server, MYENERGI_SERVER);
  WiFiManagerParameter param_myenergi_server("Server", "Myenergi Server", myenergi_server, 40);
  wifiManager.addParameter(&param_myenergi_server);

  char myenergi_hub[41];
  sprintf(myenergi_hub, MYENERGI_HUB);
  WiFiManagerParameter param_myenergi_hub("Hub", "Myenergi Hub", myenergi_hub, 40);
  wifiManager.addParameter(&param_myenergi_hub);

  tftSetup();
  tftClear();
  char msg[50];
  sprintf(msg, "Connect to Local AP : %s\n", CODENAME);
  tftLine1(msg);

  sprintf(msg, "Using password : %s\n", PASSWORD);
  tftLine2(msg);

  tftLine3("Then point web broswer at http://192.168.4.1");

#ifdef debugInit
  Serial.println("Starting WiFiManager in AP mode");
#endif

  // wm.autoConnect(); // auto generated AP name from chipid
  // wm.autoConnect("AutoConnectAP"); // anonymous ap

  if (!wifiManager.autoConnect(CODENAME, PASSWORD))
  {
    Serial.println("Failed to connect!");
    ESP.restart();
  }
  else
  {
    Serial.println("connected...)");
  }

  wifiManager.setSaveConfigCallback(saveConfigCallback);

  tftSetup();
  tftClear();

  sprintf(msg, "Configure Myenergi hub for: %s\n", WiFi.localIP().toString().c_str());
  tftLine1(msg);
  sprintf(msg, "Myenergi server is: %s\n", MYENERGI_SERVER);
  tftLine2(msg);

//  if (mqtt_client.connect(CODENAME, MQTT_USERNAME, MQTT_PASSWORD)) {
//    mqtt_client.publish("outTopic","hello world");
//    mqtt_client.subscribe("inTopic");
//  }
//  else {
//    Serial.println("Failed to connect to MQTT broker");
//  }

}



//void processMQTT() {
//  mqtt_client.loop();
//}