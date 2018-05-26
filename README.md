# ESP8266 Load Cell
A MQTT client to read out the result of HX711 and send it to a MQTT Broker.

#### This is a (somewhat) fork of [newAM/LoadCellOccupany](https://github.com/newAM/LoadCellOccupany/). Be sure to check his repository out!

## Hardware

This is the exact Hardware I'm using. It cost around 5-10€ on AliExpress.
- ESP8266 - [WeMos D1 mini Pro](https://wiki.wemos.cc/products:d1:d1_mini_pro)
- ADC - HX711
- Load Cell - Search for "50kg load cell hx711" on AliExpress. It'll have a HX711 included.
- 3D printed case for the load cell.

## Software

You need some libarys to compile this Arduino Sketch.
- [esp8266/Arduino](https://github.com/esp8266/Arduino#installing-with-boards-manager) - Install via Arduino IDE
- [async-mqtt-client](https://github.com/marvinroger/async-mqtt-client) - [ZIP Download](https://codeload.github.com/marvinroger/async-mqtt-client/zip/master)
- [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) - [ZIP Download](https://codeload.github.com/me-no-dev/ESPAsyncTCP/zip/master)
- [HX711](https://github.com/bogde/HX711) - [ZIP Download](https://codeload.github.com/bogde/HX711/zip/master)

Please [open a Issue](https://github.com/Skaronator/ESP8266-Load-Cell/issues) if you have trouble to compile.
