#include "include.h"
#include "ota.h"

extern WiFiManager wifiManager;

// ---------------------------------------------------
void handlePreOtaUpdateCallback()
{
    Update.onProgress([](unsigned int progress, unsigned int total)
                      { Serial.printf("CUSTOM Progress: %u%%\r", (progress / (total / 100))); });
}

// ---------------------------------------------------
void initOTA()
{
    ArduinoOTA.begin();
    wifiManager.setPreOtaUpdateCallback(handlePreOtaUpdateCallback);
}