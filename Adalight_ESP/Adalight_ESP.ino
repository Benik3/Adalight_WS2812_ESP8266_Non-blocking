//Adalight on ESP8266 with NeoPixelBus library and Uart method for non blocking render of WS2812 and similar.
//You can define smoothing of the frames with variable SMOOTH

#include <NeoPixelBus.h>

#define PixelCount 110      //number of leds
#define PixelPin 4          //must be D4 (GPIO2) for ESP8266 Uart Method
#define SERIAL_RX_BUFF ((PixelCount * 3) + 6) * 3 //Set higher UART buffer to prevent overflow, this will set buffer to 3x size of the frame
#define SMOOTH 0        //number of interpolations, one interpolation takes about 4ms, 0 = no smoothing

#if SMOOTH
RgbColor startBuff[PixelCount], currentBuff[PixelCount], endBuff[PixelCount];
float progress;
#endif

//gamma 2.2 calibration curve
const uint8_t g22[] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 43, 44, 45, 45, 46, 47, 48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 67, 68, 69, 70, 71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 87, 89, 90, 91, 92, 93, 95, 96, 97, 98, 99, 101, 102, 103, 105, 106, 107, 109, 110, 111, 113, 114, 116, 117, 119, 120, 122, 123, 125, 126, 128, 129, 131, 132, 134, 135, 137, 139, 140, 142, 143, 145, 147, 148, 150, 152, 153, 155, 157, 159, 160, 162, 164, 165, 167, 169, 170, 172, 174, 176, 177, 179, 181, 183, 185, 187, 189, 191, 193, 195, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 249, 251, 253, 255};

// Adalight sends a "Magic Word" (defined in /etc/boblight.conf) before sending the pixel data
const uint8_t prefix[] = {'A', 'd', 'a'};
uint8_t hi, lo, chk;

NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> strip(PixelCount, PixelPin);
//NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2813Method> strip(PixelCount, PixelPin);

void setup()
{
  //tested up to 2 000 000Bd with the preset size of Serial buffer, but I recommend to use 500000-921600Bd
  Serial.begin(500000);
  Serial.setRxBufferSize(SERIAL_RX_BUFF);

  strip.Begin();
  strip.Show();

  //Initial testing flash
  strip.ClearTo(RgbColor(255, 0, 0));
  strip.Show();
  delay(500);
  strip.ClearTo(RgbColor(0, 255, 0));
  strip.Show();
  delay(500);
  strip.ClearTo(RgbColor(0, 0, 255));
  strip.Show();
  delay(500);
  strip.ClearTo(RgbColor(0, 0, 0));
  strip.Show();

  Serial.print("Ada\n"); // Send "Magic Word" string to host
}

void loop()
{
  // wait for the first byte of Magic Word
  for (int i = 0; i < sizeof prefix; ++i) {
    while (!Serial.available()) ;;
    // Check next byte in Magic Word
    if (prefix[i] != Serial.read()){
      i = 0;
    }
  }
  
  // Hi, Lo, Checksum

  while (!Serial.available()) ;;
  hi = Serial.read();
  while (!Serial.available()) ;;
  lo = Serial.read();
  while (!Serial.available()) ;;
  chk = Serial.read();

#if SMOOTH
  if (chk == (hi ^ lo ^ 0x55))
  {
    memcpy(startBuff, currentBuff, sizeof(currentBuff));  //copy actual frame as starting point of smoothing
    progress = 1.0 / SMOOTH;
    for (uint8_t i = 0; i < PixelCount; i++) {
      while (!Serial.available());
      endBuff[i].R = Serial.read();
      while (!Serial.available());
      endBuff[i].G = Serial.read();
      while (!Serial.available());
      endBuff[i].B = Serial.read();
    }
  }

  do //do smoothing until new frame come or smoothing is finished
  {
    for (uint8_t i = 0; i < PixelCount; i++)
    {
      RgbColor color = RgbColor::LinearBlend(startBuff[i], endBuff[i], progress);
      currentBuff[i] = color;

      color.R = g22[color.R];
      color.G = g22[color.G];
      color.B = g22[color.B];
      strip.SetPixelColor(i, color);
    }
    strip.Show();

    if (progress >= 1.0) //smoothing complete, we can stop now
    {
      progress = 1.0 / SMOOTH;
      break;
    }
    else
    {
      progress = progress + (1.0 / SMOOTH);
      if (progress > 1.0) progress = 1.0;
    }
  } while (Serial.available() < PixelCount);

#else
  if (chk == (hi ^ lo ^ 0x55))
  {
    for (uint8_t i = 0; i < PixelCount; i++) {
      RgbColor color;
      while (!Serial.available());
      color.R = g22[Serial.read()];
      while (!Serial.available());
      color.G = g22[Serial.read()];
      while (!Serial.available());
      color.B = g22[Serial.read()];
      strip.SetPixelColor(i, color);
    }
    strip.Show();
  }
#endif

}
