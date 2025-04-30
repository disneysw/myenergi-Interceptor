#include "include.h"
#include "decode.h"

#include <HexDump.h>

IPAddress myEnergiServer, myEnergiAltServer;

WiFiUDP udp;

uint8_t pkt_end = 0x53;

// ---------------------------------------------------
void initDecode()
{
    myEnergiServer.fromString(MYENERGI_SERVER);
    myEnergiAltServer.fromString(MYENERGI_SERVER);
    udp.begin(UDP_PORT);
}

// ---------------------------------------------------
void decodeServer(uint8_t *packetBuffer, int len)
{
#ifdef debugLoop
    Serial.printf("Received packet from Server\n");
#endif
    struct MyenergiServerPkt *server_packet = (struct MyenergiServerPkt *)packetBuffer;

    IPAddress myIP_Octects(WiFi.localIP()); // Get the local IP address as Octects
#ifdef debugDecode
    HexDump(Serial, packetBuffer, len);
#endif
    server_packet->ServerIP1 = BYTESWAP32(myIP_Octects); // Rewite  server IP address
    server_packet->ServerIP2 = BYTESWAP32(myIP_Octects); // Rewite  server IP address

#ifdef debugDecode
    HexDump(Serial, packetBuffer, len);
#endif
    // Send the updated packet to the myenergi hub
    udp.beginPacket(MYENERGI_HUB, UDP_PORT);
    udp.write(packetBuffer, len);
    udp.endPacket();
#ifdef debugDecode
    // 6b6b opcode – set operating parameters of the Eddi. 0a is the register to write, b0 04 is the boost duration in seconds (0x04b0 = 1200 seconds, 0 = off), and 01 means heater1, 02 for heater2. The sequence field increments for each command, the Eddi appears to ignore later conflicting commands that have the same seq number.
    // For setting the operating mode, the register is 0x14 and the following byte is 0 for stopped, 1 for normal.
    // Short c1b7 – unsure.
    // Long c1b7 – unsure.
    Serial.printf("Packet sequence: %x\n", server_packet->Sequence);
    Serial.printf("Packet size: %x\n", server_packet->PayloadLength);
    time_t rawtime = (time_t)server_packet->Timestamp; // Convert uint32_t timestamp to time string
    struct tm *timeinfo = localtime(&rawtime);
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);
    Serial.printf("Received Packet from Server - Timestamp: %s\n", timeString);
#endif
}

// ---------------------------------------------------
uint8_t decodeSubPacket0x3601(uint8_t *packetBuffer, int len)
{
    struct SubPacket0x3601 *packet0x3601 = (struct SubPacket0x3601 *)packetBuffer;
#ifdef decodeSubPacket3601
    Serial.printf("PacketType: 0x3601 ()\n");
    HexDump(Serial, packet0x3601->_wtf1, sizeof(packet0x3601->_wtf1));
    Serial.printf("Hub Serial: %04x\n", packet0x3601->HubSerial[0] | (packet0x3601->HubSerial[1] << 8) | (packet0x3601->HubSerial[2] << 16) | (packet0x3601->HubSerial[3] << 24));
    HexDump(Serial, packet0x3601->_wtf2, sizeof(packet0x3601->_wtf2));
    Serial.printf("Eddi Serial: %04x\n", packet0x3601->EddiSerial[0] | (packet0x3601->EddiSerial[1] << 8) | (packet0x3601->EddiSerial[2] << 16) | (packet0x3601->EddiSerial[3] << 24));
    HexDump(Serial, packet0x3601->_wtf3, sizeof(packet0x3601->_wtf3));
#endif
    pkt_end = packet0x3601->Header.pkt_end;
    return packet0x3601->Header.PayloadSize + 1;
};

// ---------------------------------------------------
uint8_t decodeSubPacket0x3510(uint8_t *packetBuffer, int len) // Harvi Record
{
    struct SubPacket0x3510 *packet0x3510 = (struct SubPacket0x3510 *)packetBuffer;
#ifdef decodeSubPacket3510
    Serial.printf("PacketType: 0x3510 (4149)   ");
    Serial.printf("PacketSize: %x\n", packet0x3510->Header.PayloadSize);
  //  Serial.printf("Eddi Serial: %04x\n", packet0x3510->EddiSerial);
    HexDump(Serial, packet0x3510->_wtf1, sizeof(packet0x3510->_wtf1));
#endif
    pkt_end = packet0x3510->Header.pkt_end;
    return packet0x3510->Header.PayloadSize + 1;
}

// ---------------------------------------------------
uint8_t decodeSubPacket0x1035(uint8_t *packetBuffer, int len)
{
    struct SubPacket0x1035 *packet0x1035 = (struct SubPacket0x1035 *)packetBuffer;
#ifdef debugDecodeSub
    Serial.printf("PacketType: 0x1035 (13584) - Shared Record\n");
#endif
    pkt_end = packet0x1035->Header.pkt_end;
    return packet0x1035->Header.PayloadSize + 1;
}

// ---------------------------------------------------
uint8_t decodeSubPacket0x3730(uint8_t *packetBuffer, int len) // Harvi Record
{
    struct SubPacket0x3730 *packet0x3730 = (struct SubPacket0x3730 *)packetBuffer;
#ifdef decodeSubPacket3730
    Serial.printf("PacketType: 0x3730 (14128) - HarviRecord\n");
    Serial.printf("wtf1: %04X\n", packet0x3730->_wtf1);
    Serial.printf("Harvi Serial: %i\n", packet0x3730->harviSerial);
    //[0] | (packet0x3730->harviSerial[1] << 8) | (packet0x3730->harviSerial[2] << 16));
    Serial.printf("Clamp1_Power_Watts: %i\n", packet0x3730->Clamp1_Power_Watts);
    Serial.printf("Clamp1_Current_CentiAmps: %i\n", packet0x3730->Clamp1_Current_CentiAmps);

    Serial.printf("Clamp2_Power_Watts: %i\n", packet0x3730->Clamp2_Power_Watts);
    Serial.printf("Clamp2_Current_CentiAmps: %i\n", packet0x3730->Clamp2_Current_CentiAmps);

    Serial.printf("Clamp3_Power_Watts: %i\n", packet0x3730->Clamp3_Power_Watts);
    Serial.printf("Clamp3_Current_CentiAmps: %i\n", packet0x3730->Clamp3_Current_CentiAmps);
    //    HexDump(Serial, packet0x3730->_wtf2, sizeof(packet0x3730->_wtf2));

#endif
    pkt_end = packet0x3730->Header.pkt_end;
    return packet0x3730->Header.PayloadSize + 1;
}

// ---------------------------------------------------
uint8_t decodeSubPacket0x5a5a(uint8_t *packetBuffer, int len)
{
    struct SubPacket0x5a5a *packet0x5a5a = (struct SubPacket0x5a5a *)packetBuffer;
#ifdef decodeSubPacket5a5a
    Serial.printf("PacketType: 0x5a5a (23130) - historical energy data   ");
    Serial.printf("PacketSize: %x\n", packet0x5a5a->Header.PayloadSize);
    HexDump(Serial, packet0x5a5a->_wtf1, sizeof(packet0x5a5a->_wtf1));
    Serial.printf("Eddi Serial: %04x\n", packet0x5a5a->EddiSerial[0] | (packet0x5a5a->EddiSerial[1] << 8) | (packet0x5a5a->EddiSerial[2] << 16) | (packet0x5a5a->EddiSerial[3] << 24));
    HexDump(Serial, packet0x5a5a->_wtf2, sizeof(packet0x5a5a->_wtf2));
#endif
    pkt_end = packet0x5a5a->Header.pkt_end;
    return packet0x5a5a->Header.PayloadSize + 1;
}

// ---------------------------------------------------
uint8_t decodeSubPacket0x6b6b(uint8_t *packetBuffer, int len)
{
    struct SubPacket0x6b6b *packet0x6b6b = (struct SubPacket0x6b6b *)packetBuffer;
#ifdef debugDecodeSub
    Serial.printf("PacketType: 0x6b6b (27499) - Server to Eddi Record\n");
#endif
    pkt_end = packet0x6b6b->Header.pkt_end;
    return packet0x6b6b->Header.PayloadSize + 1;
}

// ---------------------------------------------------
uint8_t decodeSubPacket0x7878(uint8_t *packetBuffer, int len)
{
    struct SubPacket0x7878 *packet0x7878 = (struct SubPacket0x7878 *)packetBuffer;
#ifdef decodeSubPacket7878
    Serial.printf("PacketType: 0x7878 (30840)\n");
    HexDump(Serial, packet0x7878->_wtf1, sizeof(packet0x7878->_wtf1));
#endif
    pkt_end = packet0x7878->Header.pkt_end;
    return packet0x7878->Header.PayloadSize + 1;
}

// ---------------------------------------------------
uint8_t decodeSubPacket0x7979(uint8_t *packetBuffer, int len) // Zappi Record
{
    struct SubPacket0x7979 *packet0x7979 = (struct SubPacket0x7979 *)packetBuffer;
#ifdef debugDecodeSub
    Serial.printf("PacketType: 0x7878 (31097) - Zappi record\n");
#endif
    pkt_end = packet0x7979->Header.pkt_end;
    return packet0x7979->Header.PayloadSize + 1;
}

// ---------------------------------------------------
uint8_t decodeSubPacketUnknown(uint8_t *packetBuffer, int len) // Unknown Record
{
    struct SubPacketUnknown *packetUnknown = (struct SubPacketUnknown *)packetBuffer;
#ifdef debugDecodeSub
    Serial.printf("Unknown PacketType: %#02X\n", packetUnknown->Header.PacketType);
#endif
    pkt_end = packetUnknown->Header.pkt_end;
    return packetUnknown->Header.PayloadSize + 1;
}

// ---------------------------------------------------
void decodeHub(uint8_t *packetBuffer, int len)
{
#ifdef debugLoop
    Serial.printf("Received packet from Hub\n");
#endif
    struct MyenergiHubPkt *hub_packet = (struct MyenergiHubPkt *)packetBuffer;

    udp.beginPacket(myEnergiServer, UDP_PORT);
    udp.write(packetBuffer, len);
    udp.endPacket();
    udp.clear();
#ifdef debugDecode
    time_t rawtime = (time_t)hub_packet->Timestamp; // Convert uint32_t timestamp to time string
    struct tm *timeinfo = localtime(&rawtime);
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);
    Serial.printf("Sent packet to Server Packet - Timestamp: %s\n", timeString);
    Serial.printf("Serial: %" PRIu32 "\n", hub_packet->Serial);
    Serial.printf("FirmwareVersion: %" PRIu16 "\n", hub_packet->FirmwareVersion);
    Serial.printf("NetworkId: %" PRIu8 "\n", hub_packet->_NetworkId);
    Serial.printf("UDP PacketSize: %#02X\n", hub_packet->UDP_PacketSize);
#endif
    packetBuffer += 0x20;
    do
    {
        uint16_t pkt_type;
        memcpy(&pkt_type, packetBuffer + 2, sizeof(pkt_type));
        uint8_t pkt_size;
        memcpy(&pkt_size, packetBuffer, sizeof(pkt_size));
#ifdef debugLoop
        memcpy(&pkt_end, packetBuffer + 1, sizeof(pkt_end));
        Serial.printf("Packet_size %" PRIu8 " - ", pkt_size);
        Serial.printf("Packet_end %" PRIu8 " - ", pkt_end);
        Serial.printf("Packet_type %" PRIu16 "\n", pkt_type);
#endif
        switch (pkt_type)
        {
        case 0x3601: // 13825
            packetBuffer += decodeSubPacket0x3601(packetBuffer, pkt_size);
            break;
        case 0x3510: // 4149
            packetBuffer += decodeSubPacket0x3510(packetBuffer, pkt_size);
            break;
        case 0x1035: // 13584 (SharedRecord)
            packetBuffer += decodeSubPacket0x1035(packetBuffer, pkt_size);
            break;
        case 0x3730: // 14128 (HarviRecord)
            packetBuffer += decodeSubPacket0x3730(packetBuffer, pkt_size);
            break;
        case 0x5a5a: // 23130
            packetBuffer += decodeSubPacket0x5a5a(packetBuffer, pkt_size);
            break;
        case 0x7878: // 30840
            packetBuffer += decodeSubPacket0x7878(packetBuffer, pkt_size);
            break;
        case 0x7979: // 31097 (ZappiRecord)
            packetBuffer += decodeSubPacket0x7979(packetBuffer, pkt_size);
            break;
        default:
            packetBuffer += decodeSubPacketUnknown(packetBuffer, pkt_size);
        }
    } while (pkt_end == 0x53); // 0x53 = 'S' (start of packet? || 0x00 = last packet)
#ifdef debugDecode
    Serial.printf("Processed packet -----------------------------\n");
#endif
}

// ---------------------------------------------------
void decodePacket(uint8_t *packetBuffer, int len)
{

    if (len)
    {
        uint_fast32_t magic = packetBuffer[0] | (packetBuffer[1] << 8) | (packetBuffer[2] << 16) | (packetBuffer[3] << 24);
#ifdef debugDecode
        Serial.printf("magic: %x\n", magic);
#endif
        if (magic == 0xab1234d1)
        {
            decodeServer(packetBuffer, len); // Decode, Rewite incomming server packet & Forward to myenergi hub
        }
        else if (magic == 0xfacecac0)
        {
            decodeHub(packetBuffer, len); //  Decode & Forward to myenergi server
        }
        else
        {
            Serial.printf("Unknown magic#: %x\n", magic);
        }
    };
}
