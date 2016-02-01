# ha_logger - IOT
## home automation data logger

- [Nodemcu](http://nodemcu.com/index_en.html) with Arduino IDE
- based on [ESP8266](http://www.esp8266.com) with GPIOs
- DHT22 temperatur and humidity sensor
- ir serial com
- save data in REST API

![nodemcu](http://nodemcu.com/images/thumbnail/c1s.jpg_450x300.jpg)

### install

- install Arduino IDE > 1.6.2 https://www.arduino.cc/en/Main/Software
- start Arduino IDE, open Preferences window
- put in http://arduino.esp8266.com/stable/package_esp8266com_index.json at Additional Board Manager URLs field
- select Boards Manager from "Tools > Board"
- install esp8266 platform 
- select NodeMCU 1.0 under "Tools > Board"
- install [DHT Lib](https://github.com/adafruit/DHT-sensor-library) via Library Manager
- install [Time Lib](https://github.com/PaulStoffregen/Time) via Library Manager

## Dependencies
- [esp8266 Arduino](https://github.com/esp8266/Arduino)
- [DHT lib](https://github.com/adafruit/DHT-sensor-library)
- [Time Lib](https://github.com/PaulStoffregen/Time)

## Notes
- if the API is protected by Basic Auth, the base64(user:password) string must be provided in the http header
- no "http://" is needed in the host var for the client

## To Do
- ~~read DHT22 temperatur and humidity~~
- ~~save data via rest ha api~~
- ~~static update interval~~
- check watchdog, sometime it stops working
- add "headbeat"
- flexibleupdate interval
- ir serial com for eHz
- ir serial com for Sensus PolluCom E
