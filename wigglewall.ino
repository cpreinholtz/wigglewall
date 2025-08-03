/// @file    Animartrix.ino
/// @brief   Demonstrates Stefan Petricks Animartrix effects.
/// @author  Stefan Petrick
/// @author  Zach Vorhies (FastLED adaptation)
///

/*
This demo is best viewed using the FastLED compiler.

Windows/MacOS binaries: https://github.com/FastLED/FastLED/releases

Python

Install: pip install fastled
Run: fastled <this sketch directory>
This will compile and preview the sketch in the browser, and enable
all the UI elements you see below.



OVERVIEW:
This is the famouse Animartrix demo by Stefan Petrick. The effects are generated
using polor polar coordinates. The effects are very complex and powerful.
*/


#include <stdio.h>
#include <string>

#include <FastLED.h>
#include "fl/json.h"
#include "fl/slice.h"
#include "fx/fx_engine.h"

#include "fx/2d/animartrix.hpp"
#include "fl/ui.h"
#include "fl/upscale.h"
#include "hsv2rgb.h"

using namespace fl;

#define LED_PIN 2
#define LED_PIN0 12
#define LED_PIN1 40
#define LED_PIN2 27
#define LED_PIN3 25
#define LED_PIN4 26
#define LED_PIN5 34
#define LED_PIN6 41
#define LED_PIN7 28
#define COLOR_ORDER GRB

#define BRIGHTNESS 200

//NOTE REQUIRES REDEFINING THE radial_filter_radius:::       if (w > h) { this->radial_filter_radius = int(float(w)*23.0/32.0); } else { this->radial_filter_radius = int(float(h)*23.0/32.0); }
#define MATRIX_WIDTH 25
#define MATRIX_HEIGHT 25

#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

#define FIRST_ANIMATION POLAR_WAVES

// This is purely use for the web compiler to display the animartrix effects.
// This small led was chosen because otherwise the bloom effect is too strong.
#define LED_DIAMETER 0.15  // .15 cm or 1.5mm

/////////////////////////////////////////////////////////////////////////////
#include "map.h"
CRGB leds[NUM_LEDS];
CRGB leds_pin[8][200];
CRGB ledsFull[1600];
XYMap xyMap = XYMap::constructRectangularGrid(MATRIX_WIDTH, MATRIX_HEIGHT);
XYMap xyMapFull = XYMap::constructRectangularGrid(64, 25);
XYMap xyMap_pin = XYMap::constructSerpentine(8, 25);
//XYMap xyMap = XYMap::constructSerpentine(MATRIX_WIDTH, MATRIX_HEIGHT);
//XYMap xyMap = XYMap::constructWithUserFunction(MATRIX_WIDTH, MATRIX_HEIGHT, XY);
//XYMap xyMap = XYMap::constructWithLookUpTable(MATRIX_WIDTH, MATRIX_HEIGHT, XYTable);


/////////////////////////////////////////////////////////////////////////////


UISlider brightness("Brightness", BRIGHTNESS, 0, 255, 1);
UISlider fxIndex("Animation", 0, 0, NUM_ANIMATIONS - 1, 1);
UISlider timeSpeed("Time Speed", 1, -10, 10, .1);

Animartrix animartrix(xyMap, FIRST_ANIMATION);
FxEngine fxEngine(NUM_LEDS);

/////////////////////////////////////////////////////////////////////////////
#include "manager.h"
Manager manager;

void setup() {
    //auto screen_map = xyMap.toScreenMap();
    //screen_map.setDiameter(LED_DIAMETER);
    //FastLED.addLeds<WS2811, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
        //.setCorrection(TypicalLEDStrip)
        //.setScreenMap(screen_map);
    //FastLED.setBrightness(brightness);
    fxEngine.addFx(animartrix);


    animartrix.setColorOrder(static_cast<EOrder>(COLOR_ORDER));

    /////////////////////////////////////////////////////////////////////////////

    auto screen_map_pin0 = xyMap_pin.toScreenMap();
    screen_map_pin0.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2812, LED_PIN0, COLOR_ORDER>(leds_pin[0], 25*8)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_map_pin0);    

    auto screen_map_pin1 = xyMap_pin.toScreenMap();
    screen_map_pin1.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2812, LED_PIN1, COLOR_ORDER>(leds_pin[1], 25*8)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_map_pin1);    

    auto screen_map_pin2 = xyMap_pin.toScreenMap();
    screen_map_pin2.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2812, LED_PIN2, COLOR_ORDER>(leds_pin[2], 25*8)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_map_pin2);

    auto screen_map_pin3 = xyMap_pin.toScreenMap();
    screen_map_pin3.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2812, LED_PIN3, COLOR_ORDER>(leds_pin[3], 25*8)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_map_pin3);

    auto screen_map_pin4 = xyMap_pin.toScreenMap();
    screen_map_pin4.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2812, LED_PIN4, COLOR_ORDER>(leds_pin[4], 25*8)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_map_pin4);

    auto screen_map_pin5 = xyMap_pin.toScreenMap();
    screen_map_pin5.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2812, LED_PIN5, COLOR_ORDER>(leds_pin[5], 25*8)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_map_pin5);

    auto screen_map_pin6 = xyMap_pin.toScreenMap();
    screen_map_pin6.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2812, LED_PIN6, COLOR_ORDER>(leds_pin[6], 25*8)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_map_pin6);

    auto screen_map_pin7 = xyMap_pin.toScreenMap();
    screen_map_pin7.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2812, LED_PIN7, COLOR_ORDER>(leds_pin[7], 25*8)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_map_pin7);

    /////////////////////////////////////////////////////////////////////////////
#if SIMULATION
    auto screen_mapFull = xyMapFull.toScreenMap();
    screen_mapFull.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2812, LED_PIN, COLOR_ORDER>(ledsFull, 1600)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_mapFull);
#endif


    FastLED.setBrightness(brightness);
    Serial.begin(115200);
}

void loop() {
    /////////////////////////////////////////////////////////////////////////////
    // Draw the animation in 25x25 space
    /////////////////////////////////////////////////////////////////////////////
    static unsigned long startMillis = millis();
    EVERY_N_MILLIS(100) manager.setDesiredBrightness(brightness);
    EVERY_N_MILLIS(100) manager.setSpeed(timeSpeed);
    static int lastFxIndex = -1;
    if (fxIndex.value() != lastFxIndex) {
        lastFxIndex = fxIndex;
        manager.setPattern(fxIndex);
    }
    fxEngine.draw(millis(), leds);
    static unsigned long drawMillis = millis();//////////////

    /////////////////////////////////////////////////////////////////////////////
    // apply hue shift on the smaller 25x25 image
    /////////////////////////////////////////////////////////////////////////////
    CHSV hsv[NUM_LEDS];
    static uint8_t hueOffset = 0;
    for (int y = 0; y < NUM_LEDS; y++){
        hsv[y] = rgb2hsv_approximate(leds[y]);
        hsv[y].h = hsv[y].h + hueOffset;
    }
    hsv2rgb_rainbow(hsv, leds, NUM_LEDS);
    EVERY_N_MILLIS(10) hueOffset++;

    /////////////////////////////////////////////////////////////////////////////
    // Upscale the animation to 64x25
    /////////////////////////////////////////////////////////////////////////////
    upscale(leds, ledsFull, MATRIX_WIDTH, MATRIX_HEIGHT, xyMapFull);

    /////////////////////////////////////////////////////////////////////////////
    // Split the image into its output pins and associated leds variables
    /////////////////////////////////////////////////////////////////////////////
    for (int pin = 0; pin <8; pin++){
        for (int y = 0; y < 25; y++){
            for (int x = 0; x < 8; x++){
                int xx = pin * 8 + x;
                leds_pin[pin][xyMap_pin(x,y)] = ledsFull[xyMapFull(xx,y)];
            }
        }
    }


    static unsigned long copyMillis = millis();//////////////
    FastLED.show();

    /////////////////////////////////////////////////////////////////////////////
    //use manager values to update brightness etc for next frame
    /////////////////////////////////////////////////////////////////////////////
    manager.run();

    if (manager.clearAnimationChanged()){
        animartrix.fxSet(manager.currentAnimation);
    }
    FastLED.setBrightness(manager.currentBrightness);
    fxEngine.setSpeed(manager.timeSpeed);

    static unsigned long endMillis = millis();
    EVERY_N_MILLIS(1000) {
        Serial.print("FPS: ");
        Serial.print(1000.0/float(endMillis - startMillis));

        Serial.print(", draw time: ");
        Serial.print(drawMillis - startMillis);

        Serial.print(", copy time: ");
        Serial.print(copyMillis - drawMillis);


        Serial.print(", push time: ");
        Serial.println(endMillis - copyMillis);
    }
    
}

