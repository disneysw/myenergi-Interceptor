#include "include.h"
#include "html.h"

AsyncWebServer server(80);

// ---------------------------------------------------
// Replaces placeholder with LED state value
String processor(const String &var)
{
    Serial.println(var);
    if (var == "GPIO_STATE")
    {
        return "ok";
    }
    return String();
}

void initHTML()
{
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/index.html", String(), false, processor); });

    // Route to load style.css file
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/style.css", "text/css"); });

    server.begin();
}