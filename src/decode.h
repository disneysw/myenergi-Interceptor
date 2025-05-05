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
void decodeIncommingPacket(uint8_t *packetBuffer, int len, IPAddress ip, uint16_t port);


// Status Flags           Value 	   Zappi 	Eddi 	Description
#define STATUS_EDDI       0x0001  // No 	  Yes 	Data relates to eddi
#define STATUS_ZAPPI      0x0002  // Yes 	  No 	  Data relates to zappi
#define STATUS_HEATER_ON  0x0008  // Yes 	  Yes 	Heater is hot (eddi), cable unlocked? (zappi)
#define STATUS_BOOSTING   0x0010  // Yes 	  Yes 	Boosting
#define STATUS_NORMAL     0x0020  // Yes 	  Yes 	Always set during normal use - was clear during zappi firmware update
#define STATUS_UNKNOWN1   0x0040  // ? 	    Yes 	Possibly related to settings being changed
#define STATUS_UNKNOWN2   0x0080  // ? 	    Yes 	Possibly related to settings being changed
#define STATUS_HEATER1    0x0100  // Yes 	  Yes 	Heater 1 (eddi), unknown (zappi)
#define STATUS_HEATER2    0x0200  // Yes 	  Yes 	Heater 2 (eddi), unknown (zappi)


// Web API doc for ref (so we know what data is actually avail) https://github.com/twonk/MyEnergi-App-Api
// Eddi fitted H1d means heater 1 diverted energy and H1b means heater 1 boost



/*
Available data from Myenergi web API i.e. the hub sent most of it!
{
"eddi":[ 
  { 
     "dat":"09-09-2019",
     "tim":"16:55:50",
     "ectp1":1,
     "ectp2":1,
     "ectt1":"Grid",
     "ectt2":"Generation",
     "frq":50.15,
     "gen":304,
     "grd":4429,
     "hno":1,
     "pha":3,
     "sno":10088888,
     "sta":1,
     "vol":0.0,
     "ht1":"Tank 1",
     "ht2":"Tank 2",
     "tp1":-1,
     "tp2":-1,
     "pri":2,
     "cmt":254,
     "r1a":1,
     "r2a":1,
     "r2b":1,
     "che":1
  }
]
},
{
"libbi":[
  {
"deviceClass": "LIBBI",
"sno": 20005555,
"dat": "01-09-2024",
"tim": "15:26:00",
"ectp1": -729,
"ectp2": -68,
"ectp3": 0,
"ectt1": "Internal Load",
"ectt2": "Grid",
"ectt3": "None",
"ectp4": 0,
"ectp5": 148,
"ectt4": "None",
"ectt5": "DCPV",
"ectt6": "None",
"dst": 1,
"tz": 0,
"lmo": "BALANCE",
"sta": 6,
"frq": 49.87,
"pri": 1,
"soc": 86,
"isp": true,
"pha": 1,
"vol": 2414,
"mbc": 10200,
"mic": 5000,
"gen": 148,
"grd": -58,
"div": -729,
"ect1p": 1,
     "ect2p": 1,
"ect3p": 1,
"batteryDischargingBoost": false,
"pvDirectlyConnected": true,
"g100LockoutState": "NONE",
"countryCode": "GBR",
"isVHubEnabled": true,
"cmt": 254,
"fwv": "3702S5.433",
"newAppAvailable": false,
"newBootloaderAvailable": false,
"productCode": "3702"
}
]
},
{ 
"zappi":[ 
  { 
     "dat":"09-09-2019",
     "tim":"16:55:50",
     "div":3596,
     "ectp1":4369,
     "ectp2":295,
     "ectt1":"Grid",
     "ectt2":"Generation",
     "frq":50.01,
     "gen":304,
     "grd":4429,
     "pha":1,
     "sno":10077777,
     "sta":4,
     "vol":240.2,
     "pri":1,
     "cmt":254,
     "zmo":3,
     "tbh":9,
     "tbm":15,
     "tbk":2,
     "pst":"C2",
     "bsm":1,
     "mgl":100,
     "sbh":14,
     "sbk":5
  }
]
},
{ 
"harvi":[ 

]
*/

// Firstly, values 0x1 and 0x2 are not to do with whether this is a Zappi or Eddi,
// bits 0x1-0x4 represent the Device Priority. So those bits are not flags - they should be masked with either 0x3 or 0x7
// to give the priorty, depending upon whether it's in the first 2 or 3 bits.
// I would imagine 3 bits as I'm pretty sure the MyEnergi system supports 4 devices, and there is no priority 0.
// Secondly, bits 0x100 - 0x800 on the Zappi are again not flags but should be masked with 0x300 and shifted right 8 bits
// to give values 0-3 where on Zappi they represent the Charge Mode and as you discovered on Eddi represent the Heater:

// Zappi
// 1 = Fast Mode
// 2 = Eco Mode
// 3 = Eco+ Mode

// Eddi
// 1 = Heater 1
// 2 = Heater 2

// Finally for now, bits 0x1000 - 0x2000 represent a looping value (0-1-2-3-0-1-2-3-1...) which changes in response
// to a configuration change.  So I'm pretty sure they represent the last server config message that was applied by
// the device.

// convention:
// _lowercase - zero certainty what is that

// ---------------------------------------------------
struct MyenergiServerPkt
{
  uint32_t MAGIC;         // Packet identifer
  uint32_t Padding1;      // Always 0x00?
  uint32_t Timestamp;     // Unix timestamp
  uint16_t UDP_PacketSize; // Length of the payload
  uint16_t Sequence;      // Sequence number seems to allwasy be 0x0001?
  uint16_t _Unknown2;     // Seems to always be 0x399a?
  uint16_t _pktType;
  uint32_t _Unknown3;
  uint32_t _Unknown4;
  uint32_t ServerIP1;     // Server IP address
  uint32_t ServerIP2;     // Secondary server IP address
  uint64_t Padding2;
};

// ---------------------------------------------------
struct MyenergiHubPkt
{
  uint32_t MAGIC;           // Packet identifer
  uint32_t padding;
  uint32_t Serial;          // Serial number
  uint32_t SeqNumber;       // Increments by one for each packet
  uint32_t Timestamp;       // Unix timestamp
  uint32_t _wtf1;           // Seems to always be 0x0000000e
  uint16_t FirmwareVersion; // Currently 0x5458
  uint8_t _NetworkId;       // Always 83dec (0x53 | 's')
  uint8_t _wtf2;            // Always 0x00
  uint16_t UDP_PacketSize;  // Size of the packet
  uint16_t _wtf3;
};

// ---------------------------------------------------
struct PacketHeader
{
  uint8_t PayloadSize;    // Size of the payload
  uint8_t pkt_end;        // 's' (always 0x53 or 0x00 for last packet)
  uint16_t PacketType;    // Subpacket type
  uint8_t DevId;          //
  uint8_t _wtf1;          // 0xff => good known device, 0x1 => some shit, 0x4 => last record,
};

// ---------------------------------------------------
struct SubPacket0x3510
{
  // Len 46 Bytes
  PacketHeader Header;
  uint8_t EddiSerial[4];
  uint8_t _wtf1[8];
  uint16_t Status; // Bitfield
  uint16_t _wtf2;
  uint16_t _wtf3;

  uint8_t _wtf4; // Should be 0x08
  uint8_t d1_device; // always 0x11
  uint8_t _wtf5;
  uint16_t d1_watts;
  uint16_t d1_centiAmps;

  uint8_t _wtf6; // Should be 0x08
  uint8_t d2_device; // always 0x21
  uint8_t _wtf7;
  uint16_t d2_watts;
  uint16_t d2_centiAmps;

  uint8_t _wtf8; // Should be 0x08
  uint8_t d3_device; // always 0x31
  uint8_t _wtf9;
  uint16_t d3_watts;
  uint16_t d3_centiAmps;

};

// ---------------------------------------------------
struct SubPacket0x3601
{
  // 42 Bytes
  PacketHeader Header;
  uint8_t _wtf1[15];
  uint8_t EddiSerial1[4];
  uint8_t _wtf2[10]; // 0x00
  uint8_t _HubSerial[4];
  uint8_t _wtf3[5];
};

// ---------------------------------------------------
struct SubPacket0x3730
{
  // Len 40
  PacketHeader Header;
  uint16_t _wtf1[2];
  uint32_t harviSerial;

  uint8_t Clamp1_wtf;  // 0x08
  uint8_t Clamp1_wtf1; // ? 17-33-49
  uint8_t Clamp1_ClampType;
  uint8_t Clamp1_wtf2; // ? varies but always is either an increasing sequence or 0-0-0
  uint16_t Clamp1_Power_Watts;
  uint16_t Clamp1_Current_CentiAmps;

  uint8_t Clamp2_wtf;  // 0x08
  uint8_t Clamp2_wtf1; // ? 17-33-49
  uint8_t Clamp2_ClampType;
  uint8_t Clamp2_wtf2; // ? varies but always is either an increasing sequence or 0-0-0
  uint16_t Clamp2_Power_Watts;
  uint16_t Clamp2_Current_CentiAmps;

  uint8_t Clamp3_wtf;  // 0x08
  uint8_t Clamp3_wtf1; // ? 17-33-49
  uint8_t Clamp3_ClampType;
  uint8_t Clamp3_wtf2; // ? varies but always is either an increasing sequence or 0-0-0
  uint16_t Clamp3_Power_Watts;
  uint16_t Clamp3_Current_CentiAmps;
};

// ---------------------------------------------------
struct SubPacket0x5a5a
{
  // Len 54 Bytes
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

  // Zappi diverted? = 2351 ("h1d" / 60)
  // Grid imported? = 3097 ("imp" / 60)
  // Frequency = 4997 ("frq")
  // Voltage 1 = 2427 ("v1")
  // Voltage 2 = 2426
  // Voltage 3 = 2429
  // Temperature = 170

  // the day of week is a 3-bit value and the hour is a 5-bit value, they are packed into the same byte.
  // Something else is also packed into the "day of month" (which is also 5-bits) but I'm not sure what.

  PacketHeader Header;
  uint8_t _wtf1[44];
  uint8_t EddiSerial[4];
  uint16_t _wtf2;
};

// ---------------------------------------------------
struct SubPacket0x6b6b
{
  //  The 0x6b6b stuff the server sends looks like requests to read/write specific memory addresses.
  // It will read or write a fixed size chunk at a time, can't remember the specifics off the top of my head.
  // So like the timer schedule is divided across two or three address sets and the server asks what's there and
  // then modifies it and sends it back.
  // Similarly, triggering a manual boost seems to write something to a particular address.
  // Might be an IO port of some form. I plan to look at this as well but am mostly tidying up my existing code at the
  // moment so let me know what you find.
  //
  // Examining the memory used for eddi's config data, there seems to be 4 sets of timers (4 timers each).

  PacketHeader Header; // Six bytes
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
  PacketHeader Header;
  uint8_t _wtf1[45];
  uint8_t _wth2[8]; // Always 0x53 ('S')?
  uint16_t _wtf3; // Always 0x00 ?
};

// ---------------------------------------------------
struct SubPacket0xcc99
{
  // Len 58
  PacketHeader Header;
  uint8_t _wtf1;         // 0x00
};

// ---------------------------------------------------
struct SubPacketUnknown
{
  PacketHeader Header;
};


#endif // _DECODE_H