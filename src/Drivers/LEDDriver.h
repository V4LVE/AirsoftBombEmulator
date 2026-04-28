#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <Arduino.h>
#include <Config.h>
#include <FastLED.h>



class LEDDriver {
    CRGB leds[NUM_LEDS];

    public:
        explicit LEDDriver() {
            FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
            FastLED.setBrightness(BRIGHTNESS);
        }
        void blinkRed() {
            for (int i = 0; i < NUM_LEDS; ++i) {
                leds[i] = CRGB::Red;
            }
            FastLED.show();

            delay(100);

            for (int i = 0; i < NUM_LEDS; ++i) {
                leds[i] = CRGB::Black;
            }
            FastLED.show();
        }
        void blinkWhite() {
            for (int i = 0; i < NUM_LEDS; ++i) {
                leds[i] = CRGB::White;
            }
            FastLED.show();

            delay(100);

            for (int i = 0; i < NUM_LEDS; ++i) {
                leds[i] = CRGB::Black;
            }
            FastLED.show();
        }
        void blinkBlue() {
            for (int i = 0; i < NUM_LEDS; ++i) {
                leds[i] = CRGB::Green;
            }
            FastLED.show();

            delay(100);

            for (int i = 0; i < NUM_LEDS; ++i) {
                leds[i] = CRGB::Black;
            }
            FastLED.show();
        }
    private:

    };

#endif