# dimmer
NeoPixel Light with different modes

# Installation

Use Arduino IDE to load it onto your board and have the correct curcuit in place. (consists of a 25 neopixels (WS2812B), a potentiometer and a rotary-control)

# Supported modes

Currently 5 modes are supported. By pushing the rotary the state is changed. The last state is persisted to the EEPROM and restore on the next boot. In addition in the solid color mode the color is persisted.

## solid color

The potentiometer changes the brightness and the rotary the color to display

## WHITE

The potentiometer changes the brightness, the rotary has no value

## RAINBOW 1

All LEDs show a different color and slowly circle through all available colors, making the impression of a wandering rainbow. The potentiometer changes the brightness, the rotary has no value

## RAINBOW 2

All LED display the same color and slowly circle through all available colors. The potentiometer changes the brightness, the rotary has no value

## Confetti

Randomly light up LEDs and let them fade to black. The potentiometer changes the brightness and the rotary the color to display 