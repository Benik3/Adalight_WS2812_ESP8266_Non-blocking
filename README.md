# Adalight WS2812 ESP8266 Non-blocking
Device for DIY Ambilight working as [Adalight](https://learn.adafruit.com/adalight-diy-ambient-tv-lighting/overview) but based on ESP8266 with non blocking library.

WS2812 and similar (WS2812b, WS2811, WS2813, SK6812) must be driven precisely because of lack of clock line. It use just one data line.
To achieve that timing Arduino AVR (UNO, mini, micro etc.) disable interrupts while rendering the LEDs. In this time, Arduino is not able to receive data on UART.

This project use [NeoPixelBus](https://github.com/Makuna/NeoPixelBus) library which use Serial1 Tx of ESP8266 to render WS2812 LEDs without disabling the interrupts.

## ESP8266 and 3,3V
ESP8266 use 3,3V as main power. You can buy e.g. Wemos D1 mini board which has LDO on board, so you can power it directly with 5V (e.g. from USB).
Anyway the output of the GPIO is still 3,3V.
WS2812 LEDs and similar has in datasheet, that logic High on data pin must be at least 0,7*Vcc. So if you power the LED with 5V, it's 3,5V. On some LED strip it leads to bad showing of colour (in most cases on the first LED).
The easiest way is to cut the first LED and lower it's voltage slightly. This can be done easily by one diode in series on power supply.

Here is nice diagram from [todbot.com](https://todbot.com/blog/2017/01/12/crashspace-bigbutton-w-esp8266/)
![alt text](https://todbot.com/blog/wp-content/uploads/2017/01/sacrificial-neopixel-fixed-thx-macegr.png)

You can use any diode with current at least 100mA and voltage drop 0,5V - 1V (even 0,3V should work but it's on the edge). E.g. 1N4148, 1N400x... 
The problem may not appear always. I tested here one WS2812 and one WS2813 strip, both works OK directly from ESP.