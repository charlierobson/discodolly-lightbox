#include <FastLED.h>
#include <avr/sleep.h>  

#define NUM_LEDS 6

// simply cos used in all demos
#define WS2812_PIN 5

// pin needs pwm
#define WHITE_LED_PIN 6

// (or 3, external int)
#define BUTTON_PIN 2

CRGBArray<NUM_LEDS> leds;
byte huey[NUM_LEDS];

byte buttonState, mode, discoBrite;


void setup() {
  FastLED.addLeds<NEOPIXEL, WS2812_PIN>(leds, NUM_LEDS);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(WHITE_LED_PIN, OUTPUT);

  onWake();
}


void changeLightColours(byte sv) {
  for (int i = 0; i < NUM_LEDS; ++i) {
    huey[i] = random();
    leds[i] = CHSV(huey[i], sv, sv);
  }
  // show()?
}


void onWake() {
  changeLightColours(0);

  for (int i = 0; i < 256; ++i) {
    analogWrite(WHITE_LED_PIN, i);
    delay(4);
  }

  buttonState = 0;
  mode = 100;
}


void goSleepNow() {
  for (int i = 255; i > -1; --i) {
    analogWrite(WHITE_LED_PIN, i);
    for (int j = 0; j < NUM_LEDS; j++) {
      leds[j] = CHSV(huey[j], i, i);
    }
    //show & ..
    delay(4);
  }

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(0, onWake, LOW);
  sleep_mode();

  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP  

  detachInterrupt(0); 
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
    // show & ..
    delay(4);
  }
}


void loop() {
  static int cycles, buttonTime;
  static unsigned long runningTime;

  if (mode == 100) {
    cycles = 0;
    buttonTime = 0;
    runningTime = 0;
    mode = 0;
  }

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
  runningTime += 32;
  if (runningTime > 1000*60*60) {
    goSleepNow();
  }
}
