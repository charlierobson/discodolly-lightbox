#include <FastLED.h>

#define NUM_LEDS 6
#define WS2812_PIN 5
#define WHITE_LED_PIN 5
#define BUTTON_PIN 0

CRGBArray<NUM_LEDS> leds;
byte huey[NUM_LEDS];

byte buttonState, mode, discoBrite;


void setup() {
  FastLED.addLeds<NEOPIXEL, WS2812_PIN>(leds, NUM_LEDS);
  pinMode(WHITE_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  mode = 0;
  buttonState = 0;

  changeLightColours(0);

  for (int i = 0; i < 256; ++i) {
    analogWrite(WHITE_LED_PIN, i);
    delay(4);
  }
}


void changeLightColours(byte sv) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    huey[i] = random();
    leds[i] = CHSV(huey[i], sv, sv);
  }
}


void goSleepNow() {
}


byte updateButton(bool isPressed) {
  buttonState <<= 1;
  if (isPressed) buttonState |= 1;
  return buttonState & 3;
}


void changeMode() {
  mode = 1 - mode;

  for (int i = 0; i < 256; ++i) {
    byte ii = 255 - i;
    if (mode == 0) {
      // no disco, go white
      analogWrite(WHITE_LED_PIN, i);
      for (int j = 0; j < NUM_LEDS; j++) {
        leds[j] = CHSV(huey[j], ii, ii);
      }
    }
    else {
      // no white, go disco
      analogWrite(WHITE_LED_PIN, ii);
      for (int j = 0; j < NUM_LEDS; j++) {
        leds[j] = CHSV(huey[j], i, i);
      }
    }
    delay(4);
  }
}


void loop() {
  static int dlnum = 0;
  static int cycles = 0;
  static int buttonTime = 0;

  byte buttonCode = updateButton(digitalRead(BUTTON_PIN) == LOW ? 1 : 0);
  if (buttonCode == 2) {
    // button was just pressed
    changeMode();
  } else if (buttonCode == 3) {
    // button held
    buttonTime += 10;
    if (buttonTime > 2000) {
      goSleepNow();
    }
  } else {
    buttonTime = 0;
  }

  if (++cycles == 16) {
    changeLightColours(mode ? 255 : 0);
    cycles = 0;
  }

  //FastLED.show();
  delay(32);
}
