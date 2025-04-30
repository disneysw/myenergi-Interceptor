# Myenergi Intecepter

*** This is work in progress. Much of the code is not implemented yet !!!! ***


This project is designed for an ESP32 CYB (Cheap Yellow Board) development board which includes a OLED display.

The boards are available for about $16 from Amazon or AliExpress and use Wi-Fi to connect to your network.

The code is designed to act as a man-in-the-middle device between a Myenergi Hub and its server. Once installed it reads and
decodes packets and will forward relevant information to MQTT allowing better integration with Home Assistant.

![ESP32 CYD Image](https://github.com/disneysw/myenergi-Interceptor/blob/main/images/esp32-cyb.jpg?raw=true)



## Design Features

- Low cost Man-in-the-middle data capture for Myenergi devices allowing local integration with homeassistant
- Local Access Point for configuration of WiFi
- No hardware modification of Myenergi equipment required


## Setup Instructions

1. Clone the repository to your local machine.
2. Open the project in Visual Studio Code/PlatformIO
3. Connect the ESP32 CYB board to your computer via USB.
4. On first use you need to build a filesystem image (stuff in the data directory) and upload to the ESP32 (use the PlatformIO icon to see the menu options)
5. Then build and upload the code to the board using PlatformIO.
6. After it is installed you can switch to using the Arduino OTA upload process by uncommenting/changing the options in the platformio.ini file
7. The CYD boards can have slightly different displays which I think you can tell by a red or green tag on the screen cover. If the colors appear wrong on your display, try commenting out or deleting the "#define GREEN_TFT" in include.h
8. Check the defines in include.h - uncomment the decodeSubPacketxxxx as necessary

## Usage

- On first powerup the system will create a local Access Point called "Myenergi-Intecepter"
- Login using the password "passwordTest"
- Optionally set your myenergi hub and server IP addresses
- Select your WiFi and provide your password
- The board will re-connect to your local network and display its IP address 
- Change the "cloud server" device on you Myenergi Hub to point to the IP address shown on the screen
- Use the USB serial for debug info......
- Home assistant stuff still to be coded!!!!!


## Copyright
Copyright disneysw

## info 1

// Cloud data
// Looking at the data I've collected previously, it seems this particular byte is always
// in the range of 0x00 - 0x17 (so, 00 - 23 decimal). It doesn't look like a checksum to me
// - e.g. a 2 second increment of the timestamp didn't cause it to change, yet the next 2
// second increment did. No idea what it indicates though, sadly. It comes right after the
// seconds value so I figure it might be some kind of fractions of a second or something
// along those lines?

// With that in mind, I've been looking into this on and off for the past couple of days
// and have a couple of discoveries about your definition of the Status flags bitmap.
// Value 	Zappi 	Eddi 	Description
// 0x0001 	No 	Yes 	Data relates to eddi
// 0x0002 	Yes 	No 	Data relates to zappi
// 0x0008 	Yes 	Yes 	Heater is hot (eddi), cable unlocked? (zappi)
// 0x0010 	Yes 	Yes 	Boosting
// 0x0020 	Yes 	Yes 	Always set during normal use - was clear during zappi firmware update
// 0x0040 	? 	Yes 	Possibly related to settings being changed
// 0x0080 	? 	Yes 	Possibly related to settings being changed
// 0x0100 	Yes 	Yes 	Heater 1 (eddi), unknown (zappi)
// 0x0200 	Yes 	Yes 	Heater 2 (eddi), unknown (zappi)
// Firstly, values 0x1 and 0x2 are not to do with whether this is a Zappi or Eddi,
// bits 0x1-0x4 represent the Device Priority. So those bits are not flags - they
// should be masked with either 0x3 or 0x7 to give the priorty, depending upon whether
// it's in the first 2 or 3 bits.  I would imagine 3 bits as I'm pretty sure the
// MyEnergi system supports 4 devices, and there is no priority 0.

// Secondly, bits 0x100 - 0x800 on the Zappi are again not flags but should be masked
// with 0x300 and shifted right 8 bits to give values 0-3 where on Zappi they represent
// the Charge Mode and as you discovered on Eddi represent the Heater:

// Zappi
// 1 = Fast Mode
// 2 = Eco Mode
// 3 = Eco+ Mode
//
// Eddi
// 1 = Heater 1
// 2 = Heater 2

// Finally for now, bits 0x1000 - 0x2000 represent a looping value (0-1-2-3-0-1-2-3-1...)
// which changes in response to a configuration change.  So I'm pretty sure they represent
// the last server config message that was applied by the device.

// Today I spent a lot of time trying to work out the status byte at offset 12 in structure
// type 0x3510, using the various zappi boost modes and changing a few settings. It seems
// this byte is also used by the eddi when boosting (going to investigate timed boosts soon)
// and has bit 0x80 set for zappi but clear for eddi (which seems to always have bit 0x20 set).

// The bit after this seems to only be used on the zappi during some initialisation, whilst
// on the eddi it indicates the heater priority (0x20 is set if heater 2 has priority, clear
// if heater 1) and check interval (in 5 minute increments - 0x00 to 0x18)

// Also found that there are 2 bytes that indicate the firmware version if 0x8000 is clear,
// otherwise if it is set the rest of the bits contain boost time/amount information.

// I started forwarding the packets to the cloud server but drop the replies at the moment.
// Interestingly the myenergi app does let me see data graphs despite there being no comms
// with it for a couple of weeks. So clearly the hub is also sending historical data and I
// suspect this is what's in the mysterious structure type 0x5a5a. I'll poke around with
// that at some point but if you spot anything, that would be good to figure out (even
// though I do plan to do my own graphing from the live data.)

// As I thought, structure 0x5a5a contains historical energy data. I've inspected what
// eddi outputs so far and it's quite interesting as it seems there are readings per-heater
// for diversion and import, along with grid import/export readings, and general grid
// info (voltage/frequency.) There's some zero values for me which I reckon are probably
// generation data (as I don't have a CT clamp for this.)

// I can confirm that it looks like Solar Generation is at offset 24, and from the looks
// of it the value at offset 26 is the negative solar generation.  I.e. the negative power
// on the solar CT clamp, which for me reads around 8-11 when I’m not generating solar,
// but whenever I am this value sits at 0.

// generation info in the summary structure (0x3600)?

// I've made some further progress on this and have now found that the historic data also
// includes temperature data, and a number for the "day of week".

// Comparing this to the data available at the web APIs, it looks like the server is
// multiplying the values it receives from the hub by 60. For example, for 22:22 BST
// today (21:22 UTC) the web API reports:

// imp = 185820
// h1d = 141060
// v1 = 2427
// frq = 4997
// pect1 = 60
// nect1 = 1140
// nect2 = 1320

// ...whereas the data submitted by the hub reports these values:

// Zappi diverted? = 2351 ("h1d" / 60)
// Grid imported? = 3097 ("imp" / 60)
// Frequency = 4997 ("frq")
// Voltage 1 = 2427 ("v1")
// Voltage 2 = 2426
// Voltage 3 = 2429
// Temperature = 170

// I used to assume that the low byte of "status A" would have 0x80 set if it was a zappi,
// this turns out to be a heater type flag for eddi so there must be some other way to
// detect the device type.

// Examining the memory used for eddi's config data, there seems to be 4 sets of timers
// (4 timers each). I assume 2 of these are for the relay add-in board (which I don't have.)
// The annoying thing is that changing things in the app doesn't seem to have an immediate
// effect - it lags behind by a few minutes. Presumably this is to avoid wearing out
// whatever kind of memory is used for storing the settings. I'm continuing to examine this.
// Because of the way the protocol works, if you have the app open whilst adjusting settings
// on the devices, the app might overwrite the settings thus undoing them.

// It seems you can get at (almost?) all settings. I've found the "advanced settings PIN",
// display backlight timeout, contrast settings, CT configuration, generation icon, heater
// type icons, export threshold and margin etc. So there's a lot of flexibility as to what
// can be checked/changed with it, much more so than is offered in the app.

// Also I suspect that the cycling couple of high bits in "status B" that increment every
// time a setting changes is possibly to inform the server that it needs to refresh the
// config data so that e.g. timer info is pulled by the server.

// The server seems to include a lot of leftover data in the structure when sending commands
// and the layout seems a bit weird (boost time for eddi or charge amount for zappi is in
// the middle of the data area despite there being nothing else meaningful around it.)


## Info
import std.mem;
import std.sys;
import std.core;

using MyDevice = u32;

// convention:
// _lowercase - zero certainty what is that
// _CamelCase - have an idea what is that
// CamelCase - we are certain what is that
// lowercase - not a field, impl detail

struct HarviClamp {
    std::assert(std::mem::read_unsigned($, 1) == 8, "harvi record starts with 0x8");
    padding[1];

    u8 _wtf1; // ? 17-33-49
    std::assert(_wtf1 == 17 || _wtf1 == 33 || _wtf1 == 49, "wtf1");

    u8 ClampType;

    u8 _wtf2; // ? varies but always is either an increasing sequence or 0-0-0

    s16 Power_Watts;
    s16 Current_CentiAmps;

    std::print("    CT tpe={} pw={} amps={}", ClampType, Power_Watts, Current_CentiAmps / 100.0);
    std::print("       wtf1 = {}, wtf2 = {}", _wtf1, _wtf2);
};

struct HarviRecord<auto PayloadSize, auto PayloadEnd> {
    u32 HarviSerial [[color("00ff00")]];
    std::print("  HarviSerial={}", HarviSerial);

    HarviClamp HarviClamps[while($ < PayloadEnd-2)];

    // varies between packets, tend to be the same within one packet (but not always).
    // Frequently it's 0-0 for all entries in all harvis
    // Seconds since last reading?..
    u16 _end_bytes[1];
    std::print("harvi last bytes: {}", _end_bytes);
};

struct ZappiRecord<auto PayloadSize, auto PayloadEnd>  {

};

struct SharedRecord<auto PayloadSize, auto PayloadEnd> {
    u32 _stable_id;
    u32 _zero_or_wtf;
    u32 _four;
    u16 _device_type;
    u16 _record_type;

    std::print("    devtpe {}, rectpe {}",
          _device_type, _record_type);
    if (_record_type == 276) {
       std::assert(PayloadSize == 30, "");
        u16 _voltage;
            std::print("    voltage {}",
          _voltage / 10.0);
    } else if (_record_type == 8456) {
       std::assert(PayloadSize == 42 || PayloadSize == 50, "");
    } else if (_record_type == 108) {
       std::assert(PayloadSize == 38, "");
    } else if (_record_type == 206) {
       std::assert(PayloadSize == 56, "");
    } else if (_record_type == 4360) {
       std::assert(PayloadSize == 31, "");
    } else if (_record_type == 276) {
       std::assert(PayloadSize == 30, "");
    } else if (_record_type == 276) {
       std::assert(PayloadSize == 30, "");
    } else {
      std::print("  UKNOWN 13584 RECORD TYPE: {} of size {}", _record_type, PayloadSize);
      //std::assert(false, std::format("unknown rectype {} with sz {}", _record_type, PayloadSize));
    }
};

struct MyEnergiRecord
{
    u8 PayloadSize [[color("0000FF")]];

    u32 start = $;
    u32 record_end = start + PayloadSize;


    u8 NetId [[color("00ffff")]];
    u16 _PacketType;
    u8 DevId;

    bool debug = true || (_PacketType ==  14128);


    u8 _wtf1; // 0xff => good known device, 0x1 => some shit, 0x4 => last record,
    std::assert(_wtf1 == 0xFF || _wtf1 == 4 || _wtf1 == 1, "");
    if (debug) {
        std::print("tpe {}, NetId {}, DevId {},  sz {}",
          _PacketType, NetId , DevId,  PayloadSize);

  if (_PacketType ==  13584 || _PacketType ==  31097 || _PacketType == 14128) {
        MyDevice DevSerial [[color("880000")]];
        std::print("  DevSerial {}", DevSerial);
        if (_PacketType == 14128) {
            HarviRecord<PayloadSize, record_end> HarviRecord;
        } else  if(_PacketType == 13584) {
            SharedRecord<PayloadSize, record_end> SharedRecord;
        } else if (_PacketType == 31097) {
            ZappiRecord<PayloadSize, record_end> ZappiRecord;
        }
    } else {
        std::print("  UKNOWN PACKET TYPE: {}", _PacketType);
    }
}
     u8 _unknown_end[PayloadSize - ($ - addressof(NetId))] [[color("aaaaaa")]] ;

    if (debug) std::print("  {} {}", sizeof(_unknown_end), _unknown_end);

};

struct MyenergiPkt
{
   u32 MAGIC;
   std::assert(std::mem::read_unsigned($, 4) == 0x00000000, "myenergi message starts with 0x0 in second octet");
   padding [4];
   u32 Serial;
   u32 _wtf1;
   u32 Timestamp ;
   u32 _wtf2;
   u16 _FirmwareVersion;
   u8 _NetworkId;
   u8 _wtf3;
   u16 PacketSize [[color("0000FF")]];
   u16 _wtf4;
   MyEnergiRecord Records[while (!std::mem::eof())];
};

MyenergiPkt  pkt @ 0x0;





