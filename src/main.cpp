
#include "include.h"
#include "decode.h"
#include "file.h"
#include "display.h"
#include <stdio.h>
#include "ota.h"
#include "wifimanager.h"
#include "html.h"

// ---------------------------------------------------
void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.printf("Starting Myenergi Intercepter/n");

  initWifi();
  InitFilesystem();
  initOTA(); // Setup for Arduino OTA updates (See platformio.ini for upload_port)
  initDecode();
  //  initHTML();
}

// ---------------------------------------------------
void loop()
{
  int len = udp.parsePacket(); // Check for incoming UDP packets
  if (len)                     // Packet available
  {
#ifdef debugLoop
    Serial.print(" Received packet from : ");
    Serial.println(udp.remoteIP());
#endif

    uint8_t packetBuffer[UDP_PACKET_MAX_SIZE];
    memset(packetBuffer, 0, UDP_PACKET_MAX_SIZE);          // Clear the buffer
    len = udp.read(packetBuffer, UDP_PACKET_MAX_SIZE); // Read the packet into the buffer
    decodePacket(packetBuffer, len);
  }
  //  Serial.printf("process ota\n");
  ArduinoOTA.handle(); // Handle OTA updates
}
