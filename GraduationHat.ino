#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#define PIN 6

#define LEFT 2
#define RIGHT 3

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(12, 11, PIN,
                                               NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                                               NEO_GRB + NEO_KHZ800);

uint16_t rgb888to565(uint32_t color) {
  // ChatGPT wrote this function
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  uint16_t r5 = (r >> 3) & 0x1F;
  uint16_t g6 = (g >> 2) & 0x3F;
  uint16_t b5 = (b >> 3) & 0x1F;

  return (r5 << 11) | (g6 << 5) | b5;
}

int8_t mode = 0;

long lastUpdate = 0;

const uint8_t waitVal = 200;

void LMode() {
  if (millis() - lastUpdate < waitVal) return;
  mode++;
  if (mode > 7) mode = 0;
  lastUpdate = millis();
}

void RMode() {
  if (millis() - lastUpdate < waitVal) return;
  mode--;
  if (mode < 0) mode = 7;
  lastUpdate = millis();
}

void setup() {
  matrix.begin();
  matrix.clear();
  matrix.show();
  matrix.setTextWrap(false);
  matrix.setFont(&FreeMonoBold9pt7b);
  matrix.setTextColor(matrix.Color(255, 100, 0));
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LEFT), LMode, FALLING);
  attachInterrupt(digitalPinToInterrupt(RIGHT), RMode, FALLING);
  Serial.begin(9600);
}

void loop() {
  matrix.setBrightness(255);
  Serial.println(mode);

  switch (mode) {
    case (0):
      // Diagonal rainbow
      for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
        if (mode != 0) break;
        Serial.println(mode);
        int pixelHue = firstPixelHue + (65536L / matrix.numPixels());
        for (int i = 0; i < matrix.width() * 2; i++) {
          for (int j = 0; j < matrix.width(); j++) {
            uint8_t x = matrix.width() - j;
            uint8_t y = matrix.height() - (i - j);
            uint8_t index;
            if (y % 2 == 0)
              index = x + y * matrix.width() - 1;
            else
              index = matrix.width() - x + y * matrix.width();
            matrix.setPixelColor(index, matrix.gamma32(matrix.ColorHSV(pixelHue + i * 4096)));
          }
        }
        matrix.show();
        delay(20);
      }
      break;

    case (1):
      // Circle rainbow
      matrix.fillScreen(0);
      for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
        if (mode != 1) break;
        Serial.println(mode);
        uint32_t pixelHue = firstPixelHue + (65536L / matrix.numPixels());
        for (uint8_t i = 0; i < 11; i++) {
          matrix.drawCircle(5, 5, i, rgb888to565(matrix.gamma32(matrix.ColorHSV(pixelHue - i * 4096))));
        }
        matrix.show();
        delay(50);
      }
      matrix.show();
      break;


    case (2):
      // Square rainbow
      matrix.fillScreen(0);
      for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
        if (mode != 2) break;
        Serial.println(mode);
        uint32_t pixelHue = firstPixelHue + (65536L / matrix.numPixels());
        for (uint8_t i = 0; i < matrix.width(); i++) {
          matrix.drawRect(5 - i / 2, 5 - i / 2, i, i, rgb888to565(matrix.gamma32(matrix.ColorHSV(pixelHue - i * 4096))));
        }
        matrix.show();
        delay(50);
      }
      matrix.show();
      break;

    case (3):
      // Stars
      {  // This extra layer of brackets is required... for some reason
        const uint16_t duration = 5000;
        const unsigned long startTime = millis();
        const uint8_t numStars = 5;
        const uint8_t minDist = 4;

        matrix.setBrightness(32);
        matrix.fillScreen(0);
        matrix.show();

        struct xypair {
          uint8_t x;
          uint8_t y;
        };

        xypair stars[numStars];

        for (uint8_t i = 0; i < numStars; i++) {
          stars[i].x = random(1, matrix.width() - 2);
          stars[i].y = random(1, matrix.height() - 2);
          for (uint8_t j = 0; j < numStars; j++) {
            if (j == i) continue;
            if ((abs(stars[i].x - stars[j].x) < minDist && abs(stars[i].y - stars[j].y) < minDist) && millis() - startTime < duration / 32) {
              i--;
              break;
            }
          }
        }

        while (millis() - startTime < duration) {
          if (mode != 3) break;
          long amount = (duration / 2) - (millis() - startTime);
          int16_t intensity = map(abs(amount), duration / 2, 0, 0, 700);
          if (intensity > 255) intensity = 255;
          uint16_t color = matrix.Color(intensity, intensity * 0.9, 0);
          for (uint8_t i = 0; i < numStars; i++) {
            matrix.drawPixel(stars[i].x, stars[i].y, color);
            matrix.drawCircle(stars[i].x, stars[i].y, 1, color);
          }
          matrix.show();
        }
      }
      break;

    case (4):
      // Spinning Bar
      for (float i = 0; i < PI * 2; i += PI / 6) {
        if (mode != 4) break;
        uint8_t x1 = 5 + 5 * sin(i);
        uint8_t y1 = 5 + 5 * cos(i);
        uint8_t x2 = 5 + 5 * sin(i + PI);
        uint8_t y2 = 5 + 5 * cos(i + PI);
        matrix.clear();
        matrix.drawLine(x1, y1, x2, y2, matrix.Color(255, 255, 255));
        matrix.show();
        delay(300);
      }
      break;

    case (5):
      // Spinning Triangle
      for (float i = 0; i < PI * 2; i += PI / 6) {
        if (mode != 5) break;
        uint8_t x1 = 5 + 5 * sin(i);
        uint8_t y1 = 5 + 5 * cos(i);
        uint8_t x2 = 5 + 5 * sin(i + PI * 2 / 3);
        uint8_t y2 = 5 + 5 * cos(i + PI * 2 / 3);
        uint8_t x3 = 5 + 5 * sin(i + PI * 4 / 3);
        uint8_t y3 = 5 + 5 * cos(i + PI * 4 / 3);
        matrix.clear();
        matrix.drawTriangle(x1, y1, x2, y2, x3, y3, matrix.Color(255, 255, 255));
        matrix.fillTriangle(x1, y1, x2, y2, x3, y3, matrix.Color(255, 255, 255));
        matrix.show();
        delay(300);
      }
      break;

    case (6):
      matrix.setBrightness(16);
      matrix.fillScreen(matrix.Color(255, 255, 255));
      matrix.show();
      break;

    case (7):
      static int x = matrix.width();
      matrix.clear();
      matrix.setCursor(x, matrix.height() - 1);
      matrix.print("MHS Class of 2025");
      matrix.show();
      x--;
      if (x < -200) {
        x = matrix.width();
      }
      delay(100);

      break;

    default:
      matrix.clear();
      matrix.show();
      break;
  }
}
