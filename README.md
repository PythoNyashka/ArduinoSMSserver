# ArduinoSMSserver

It's simple - send GET request to server `{server_ip}/sendsms?tel={tel_number}&msg={your_msg}`

- ArduinoGPRS - code for arduino with GPRS shield
- ArduinoServer - code for arduino with Ethernet shield

Requirements:
1. `SPI.h`
2. `Ethernet.h` (https://github.com/arduino-libraries/Ethernet)
3. `SoftwareSerial.h` 
4. `GPRS_Shield_Arduino.h` (https://github.com/amperka/gprs-shield)

![server_img](/gitimg/server_img.jpg)
