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

## Info

- https://github.com/PeterTM/mqtt-energi?tab=readme-ov-file
- https://phab.mallen.id.au/diffusion/51/browse/default/src/
- Login to local ap and run tcpdump -i br0 host 192.168.1.133 -U -w tcpdump.pcap
- https://myenergi.info/udp-protocol-details-for-local-data-collection-and-t1201-s70.html
- https://greiginsydney.com/packet-sniffing-your-unifi-access-points/
- https://web.imhex.werwolv.net/



