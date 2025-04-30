#ifndef _DECODE_H
#define _DECODE_H

#include <inttypes.h>
#include <stdio.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define BYTESWAP32(z) ((uint32_t)((z & 0xFF) << 24 | ((z >> 8) & 0xFF) << 16 | ((z >> 16) & 0xFF) << 8 | ((z >> 24) & 0xFF)))
#define UDP_PACKET_MAX_SIZE 512
#define UDP_PORT 87

void initDecode();
void decodePacket(WiFiUDP udp);
void decodePacket(uint8_t *packetBuffer, int len);

// convention:
// _lowercase - zero certainty what is that
// _CamelCase - have an idea what is that
// CamelCase - we are certain what is that
// lowercase - not a field, impl detail

// ---------------------------------------------------
struct MyenergiServerPkt
{
  uint32_t MAGIC;
  uint32_t Padding1;
  uint32_t Timestamp;
  uint16_t PayloadLength;
  uint16_t Sequence;
  uint32_t _Unknown2;
  uint32_t _Unknown3;
  uint32_t _Unknown4;
  uint32_t ServerIP1;
  uint32_t ServerIP2;
  uint64_t Padding2;
};

// ---------------------------------------------------
struct MyenergiHubPkt
{
  uint32_t MAGIC;
  uint32_t padding;
  uint32_t Serial;    // Serial number
  uint32_t SeqNumber; // Increments by one for each packet
  uint32_t Timestamp;
  uint32_t _wtf1;           // Seems to always be 0x0000000e
  uint16_t FirmwareVersion; // 5458
  uint8_t _NetworkId;       // Always 83dec (0x53 / 's')
  uint8_t _wtf2;            // Always 0x00
  uint16_t UDP_PacketSize;
  uint16_t _wtf3;
};

// ---------------------------------------------------
struct MyEnergiRecord
{
  uint8_t PayloadSize;
  uint8_t pkt_end; // 's' (always 0x53 or 0x00 for last packet)
  uint16_t PacketType;
  uint8_t DevId;
  uint8_t _wtf1; // 0xff => good known device, 0x1 => some shit, 0x4 => last record,
};

// ---------------------------------------------------
struct HarviClamp
{
  uint8_t padding[1]; // "harvi record starts with 0x8"
  uint8_t _wtf1;      // ? 17-33-49
  uint8_t ClampType;
  uint8_t _wtf2; // ? varies but always is either an increasing sequence or 0-0-0
  uint16_t Power_Watts;
  uint16_t Current_CentiAmps;
};

// ---------------------------------------------------
struct HarviRecord
{
  uint32_t HarviSerial;
  HarviClamp HarviClamps[10];
  // varies between packets, tend to be the same within one packet (but not always).
  // Frequently it's 0-0 for all entries in all harvis
  // Seconds since last reading?..
  uint16_t _end_bytes[1];
};

// ---------------------------------------------------
struct SharedRecord
{
  uint32_t _stable_id;
  uint32_t _zero_or_wtf;
  uint32_t _four;
  uint16_t _device_type;
  uint16_t _record_type;
};

// ---------------------------------------------------
struct SubPacket0x1035
{
  MyEnergiRecord Header;
};

// ---------------------------------------------------
struct SubPacket0x3601
{
  // 78
  MyEnergiRecord Header;
  uint8_t _wtf1[15];
  uint8_t HubSerial[4];
  uint8_t _wtf2[10]; // 0x00
  uint8_t EddiSerial[4];
  uint8_t _wtf3[9];
  uint8_t _wtf4[79]; 
};

// ---------------------------------------------------
struct SubPacket0x3510
{
    // Len 46 Bytes
  MyEnergiRecord Header;
//  uint32_t EddiSerial;
  uint8_t _wtf1[42];
};

// ---------------------------------------------------
struct SubPacket0x3730
{
  // Len 39
  MyEnergiRecord Header;
  uint16_t _wtf1[2];
  uint32_t  harviSerial;

  uint8_t  Clamp1_wtf; // 0x08
  uint8_t Clamp1_wtf1;      // ? 17-33-49
  uint8_t Clamp1_ClampType;
  uint8_t Clamp1_wtf2; // ? varies but always is either an increasing sequence or 0-0-0
  uint16_t Clamp1_Power_Watts;
  uint16_t Clamp1_Current_CentiAmps;

  uint8_t  Clamp2_wtf; // 0x08
  uint8_t Clamp2_wtf1;      // ? 17-33-49
  uint8_t Clamp2_ClampType;
  uint8_t Clamp2_wtf2; // ? varies but always is either an increasing sequence or 0-0-0
  uint16_t Clamp2_Power_Watts;
  uint16_t Clamp2_Current_CentiAmps;


  uint8_t  Clamp3_wtf; // 0x08
  uint8_t Clamp3_wtf1;      // ? 17-33-49
  uint8_t Clamp3_ClampType;
  uint8_t Clamp3_wtf2; // ? varies but always is either an increasing sequence or 0-0-0
  uint16_t Clamp3_Power_Watts;
  uint16_t Clamp3_Current_CentiAmps;
};

// ---------------------------------------------------
struct SubPacket0x6b6b
{
  MyEnergiRecord Header; // Six bytes
  uint8_t heaterID;
  uint8_t _wtf1;           // 0x00
  uint16_t _wtf2[17];      // 0x00
  uint16_t boostDuration1; // In seconds
  uint16_t _wtf3[3];       // 0x00
  uint16_t boostDuration2; // In seconds
  uint16_t _wtf4[3];       // 0x00
  uint16_t boostDuration3; // In seconds
  uint16_t _wtf5[3];       // 0x00
  uint16_t boostDuration4; // In seconds
  uint16_t _wtf6[3];       // 0x00
};


// ---------------------------------------------------
struct SubPacket0x7878
{
  // Len 45
  MyEnergiRecord Header;
  uint8_t _wtf1[45]; 
};

// ---------------------------------------------------
struct SubPacket0x7979
{
  MyEnergiRecord Header;
};

// ---------------------------------------------------
struct SubPacketUnknown
{
  MyEnergiRecord Header;
};

// ---------------------------------------------------
struct SubPacket0x5a5a
{
  // Len 35 Bytes
  //  As I thought, structure 0x5a5a contains historical energy data. I've inspected what
  //  eddi outputs so far and it's quite interesting as it seems there are readings per-heater
  //  for diversion and import, along with grid import/export readings, and general grid
  //  info (voltage/frequency.) There's some zero values for me which I reckon are probably
  //  generation data (as I don't have a CT clamp for this.)

  // I've made some further progress on this and have now found that the historic data also
  // includes temperature data, and a number for the "day of week".

  // I can confirm that it looks like Solar Generation is at offset 24, and from the looks
  // of it the value at offset 26 is the negative solar generation.  I.e. the negative power
  // on the solar CT clamp, which for me reads around 8-11 when I’m not generating solar,
  // but whenever I am this value sits at 0.

  MyEnergiRecord Header;
  uint8_t _wtf1[44];
  uint8_t EddiSerial[4]; 
  uint8_t _wtf2[6];
};

#endif // _DECODE_H