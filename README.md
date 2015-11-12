# ha_logger - IOT
## home automation data logger

- [Nodemcu](http://nodemcu.com/index_en.html) with Arduino IDE
- based on [ESP8266](http://www.esp8266.com) with GPIOs
- DHT22 temperatur and humidity sensor
- save data in REST API

![nodemcu](http://nodemcu.com/images/thumbnail/c1s.jpg_450x300.jpg)

### install

- install Arduino IDE > 1.6.2 https://www.arduino.cc/en/Main/Software
- start Arduino IDE, open Preferences window
- put in http://arduino.esp8266.com/stable/package_esp8266com_index.json at Additional Board Manager URLs field
- select Boards Manager from "Tools > Board"
- install esp8266 platform 
- select NodeMCU 1.0 under "Tools > Board"
- install DHT lib from http://playground.arduino.cc/Main/DHTLib

## To Do
- read DHT22 temperatur and humidity
- ir serial com for eHz
- ir serial com for Sensus PolluCom E
- save data via rest ha api
