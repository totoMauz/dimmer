#include <FastLED.h>

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <EEPROM.h>

/**
 * EEPROM
 */
const byte address_state = 0;
const byte address_hue = 1;

/**
 * LED
 */
#define LED_PIN     5
#define NUM_LEDS    25
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

byte v = 0;
#define SATURATION 255
volatile byte h = 60;

/**
 * Poti
 */
const byte pinPoti = 0;

/**
 * Rotary Encoder
 */
const byte pinCLK = 2;
const byte pinDT = 3;
const byte pinSW = 4;
volatile byte virtualPosition = 0;
boolean switchState = HIGH;
boolean lastPosition;

/**
 * states
 */
byte currentState = 1;
const byte maxState = 5;
#define STATE_COLOR 0
#define STATE_WHITE 1
#define STATE_RAINBOW 2
#define STATE_RAINBOW_2 3
#define STATE_CONFETTI 4

//Rainbow
byte hRb = 0;
byte hRbStart = 0;
#define DELTA_RAINBOW (256 / NUM_LEDS)

//Rainbow 2
byte hRb2 = 0;
byte hRbStart2 = 0;

/**
 * Debug
 */
const boolean debugPoti = false;
const boolean debugRotary = false;
const boolean debugState = false;

static unsigned long lastInterruptTime = 0;

void isr ()  {
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 5) {
    if (digitalRead(pinDT) == digitalRead(pinCLK)) {
      h += 2;
    }
    else {
      h -= 2;
    }
  }
  lastInterruptTime = interruptTime;
}

void setup() {
  delay( 3000 );
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  pinMode(pinPoti, INPUT);

  pinMode(pinCLK, INPUT);
  pinMode(pinDT, INPUT);
  pinMode(pinSW, INPUT);

  lastPosition = digitalRead(pinCLK);

  attachInterrupt(0, isr, CHANGE);

  //Read old values from EEPROM
  currentState = EEPROM.read(address_state);
  h = EEPROM.read(address_hue);

  if (debugPoti || debugRotary || debugState) {
    Serial.begin(9600);
    Serial.println("Debug mode");
    delay(100);
  }
}

void loop() {
  unsigned short valuePoti = analogRead(pinPoti);
  v = map(valuePoti, 0, 1023, 5, 255);

  if (debugPoti) {
    Serial.print(valuePoti);
    Serial.print(" :: ");
    Serial.println(v);
  }

  if (debugRotary) {
    Serial.print(virtualPosition);
    Serial.print(" :: ");
    Serial.println(h);
  }

  byte switchValue = digitalRead(pinSW);
  if (switchValue == HIGH) {
    if (switchState == LOW) {
      currentState = (currentState + 1) % maxState;
      EEPROM.update(address_state, currentState);
      if (debugRotary) {
        Serial.println("Switch released");
      }
    }/* else {
      //switch not pressed
    } */
  }
  /*
  else {
    //switch pressed
    if (switchState == HIGH) {
      if (debugRotary) {
        Serial.println("Switch pressed");
      }
    } else {
      //switch beeing pressed down
    }
  }
  */
  switchState = switchValue;

  if (debugState) {
    switch (currentState) {
      case STATE_COLOR:
        Serial.println("Singlecolor");
        break;
      case STATE_WHITE:
        Serial.println("White");
        break;
      case STATE_RAINBOW:
        Serial.println("Rainbow");
        break;
      case STATE_CONFETTI:
        Serial.println("Confetti");
        break;
      case STATE_RAINBOW_2:
      Serial.println("Rainbow 2");
        break;
    }
  }

  switch (currentState) {
    case STATE_COLOR:
      fadeLEDs();
      EEPROM.update(address_hue, h);
      break;
    case STATE_WHITE:
      fadeLEDsWhite();
      break;
    case STATE_RAINBOW:
      fadeLEDsRainbow();
      break;
    case STATE_CONFETTI:
      confetti();
      break;
    case STATE_RAINBOW_2:
      fadeLEDsRainbow2();
      break;
  }

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void fadeLEDsRainbow() {
  hRb = hRbStart;
  for ( byte i = 0; i < NUM_LEDS; i++) {
    hRb += DELTA_RAINBOW;
    leds[i] = CHSV( hRb, SATURATION, v);
  }
  ++hRbStart;
}

void fadeLEDsRainbow2() {
  hRb2 = hRbStart2;
  fill_solid( leds, NUM_LEDS, CHSV( hRb2, SATURATION, v));
  ++hRbStart2;
  //For confetti we only want ~25 updates per seconds instead of 100
  delay(30); 
}

void confetti()
{
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( h + random8(64), SATURATION, v);
  //For confetti we only want ~25 updates per seconds instead of 100
  delay(30); 
}


void fadeLEDsWhite() {
  fill_solid( leds, NUM_LEDS, CHSV( 60, 128, v));
}

void fadeLEDs() {
  for ( byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV( h, SATURATION, v);
  }
}
