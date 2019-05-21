#include <FastLED.h>

#define NUM_LEDS 6
#define WS2812_PIN 5
#define WHITE_LED_PIN 5
#define BUTTON_PIN 0

CRGBArray<NUM_LEDS> leds;

byte buttonState, mode, hue;

void setup() {
  FastLED.addLeds<NEOPIXEL, WS2812_PIN>(leds, NUM_LEDS);
  pinMode(WHITE_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  buttonState = 0;
  mode = 0;
}



byte updateButton(bool isPressed) {
  buttonState <<= 1;
  if (isPressed) buttonState |= 1;
  return buttonState & 3;
}

void changeMode() {
  mode = 1 - mode;

  if (mode == 0) {
    // no disco, go white
    for (int i = 0; i < 1024; i += 2) {
      leds.fadeToBlackBy(10);
      analogWrite(WHITE_LED_PIN, i);
      FastLED.delay(1);
    }
  }
  else {
    // no white, go disco
    for (int i = 0; i < 1024; i += 2) {
      analogWrite(WHITE_LED_PIN, 1022 - i);
      for (int j = 0; j < NUM_LEDS; j++) {
        leds[i] = CHSV(hue++, i / 4, i / 4);
      }
      FastLED.delay(1);
    }
  }
}


void loop() {
  if (updateButton(digitalRead(BUTTON_PIN) == LOW ? 1 : 0) == 2) {
    // button was just pressed
    changeMode();
  }

  if ((millis() & 511) == 511) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(hue++, 255, 255);
    }
  }

  FastLED.delay(33);
}

