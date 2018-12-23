# Adalight WS2812 ESP8266 Non-blocking
Device for DIY Ambilight working as [Adalight](https://learn.adafruit.com/adalight-diy-ambient-tv-lighting/overview) but based on ESP8266 with non blocking library.

WS2812 and similar (WS2811,WS2813,SK6812) must be driven precisely because of lack of clock line. It use just one data line.
To achive that timing Arduino AVR (UNO, mini, micro etc.) disable interrupts while rendering the LEDs. In this time, Arduino is not able to receive data on UART.

This project use [NeoPixelBus](https://github.com/Makuna/NeoPixelBus) library which use Serial1 Tx of ESP8266 to render WS2812 LEDs without disabling the interrupts.
