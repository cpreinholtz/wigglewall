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

// unused, just defined in case I accidentally load the simulation code onto hardware (gets output on this pin)
#define LED_PIN 2

//I am going to label OCTO outputs LEFT and RIGHT based on looking from the top of the board
// _______
//|       |
//|       |
//| _   _ |
//||L| |R||
// --------
// The weird way I soldered my teensy makes for the following pinout
// pin | L/R | Eth Color
// 27  |  R  | Green
// 34  |  R  | Blue
// 28  |  R  | Brown
// 12  |  R  | Orange
// 26  |  L  | Orange
// 25  |  L  | Brown
// 40  |  L  | Blue
// 41  |  L  | Green
// ^^ This is a very specific order that creates a mirrored wall layout, where the "left" and "right" sides are interchangeable

// The dataflow direction of each wall pannel is flipped for easier cable runs (see graphic below with <- and -> denoting the dataflow direction)
// |<-Gr| |Bl->| |<-Br| |Or->| |<-Or| |Br->| |<-Bl| |Gr->|

// This is a very specific order that creates a mirrored wall layout, where the "left" and "right" sides are interchangeable
#define LED_PIN0 27
#define LED_PIN1 34
#define LED_PIN2 28
#define LED_PIN3 12
#define LED_PIN4 26
#define LED_PIN5 25
#define LED_PIN6 40
#define LED_PIN7 41


#define COLOR_ORDER RGB

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


//flip individual outputs because serperntine starts left to right then down, but my wall is up and down then left right
FASTLED_FORCE_INLINE uint16_t xy_serpentine_vertical(uint16_t x, uint16_t y,
                                            uint16_t width, uint16_t height) {
    (void)width;
    if (x & 1) // Even or odd row?
        // reverse every second line for a serpentine led layout
        return (x + 1) * height - 1 - y;
    else
        return x * height + y;
}
//XYMap xyMap_pin = XYMap::constructSerpentine(PIN_WIDTH, PIN_HEIGHT);
XYMap xyMap_pin = XYMap::constructWithUserFunction(8, 25,xy_serpentine_vertical);
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
// Clark's stuff
/////////////////////////////////////////////////////////////////////////////   
//#define SIMULATION
#define DEBUG_MILLIS 50000
#include "manager.h"
Manager manager;

void setup() {

    //not used anymore, the 25 by 25 is only used by the fx engine
    //auto screen_map = xyMap.toScreenMap();
    //screen_map.setDiameter(LED_DIAMETER);
    //FastLED.addLeds<WS2811, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
        //.setCorrection(TypicalLEDStrip)
        //.setScreenMap(screen_map);
    //FastLED.setBrightness(brightness);


    fxEngine.addFx(animartrix);
    animartrix.setColorOrder(static_cast<EOrder>(COLOR_ORDER));

    /////////////////////////////////////////////////////////////////////////////
#ifndef SIMULATION

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
#endif

    /////////////////////////////////////////////////////////////////////////////
#ifdef SIMULATION
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

    if (millis() < DEBUG_MILLIS){
        for (int y = 0; y < 25; y++){
            for (int x = 0; x < 64; x++){
                int idx = xyMapFull(x,y);
                ledsFull[idx].r = x*255/64;
                ledsFull[idx].b = y*255/25;
                ledsFull[idx].r = 0;
                ledsFull[idx].b = 0;
                ledsFull[idx].g = 0;
            }
        }
        //ledsFull[0].g = 100;
    }
    /////////////////////////////////////////////////////////////////////////////
    // Split the image into its output pins and associated leds variables
    /////////////////////////////////////////////////////////////////////////////
    for (int pin = 0; pin <8; pin++){
        for (int y = 0; y < 25; y++){
            for (int x = 0; x < 8; x++){
                //get fullwall  xx based on current pin and pin's x
                int xx = pin * 8 + x;

                // reverse serpentine for even outputs (X gets inverted, y stays the same)
                int xxx;
                if (pin % 2 == 0) xxx = 7-x;
                else xxx = x;

                leds_pin[pin][xyMap_pin(xxx,y)] = ledsFull[xyMapFull(xx,y)];
            }
        }
    }
    if (millis() < DEBUG_MILLIS){
        for (int pin = 0; pin <8; pin++){
            for (int i = 0; i < pin; i++){
                leds_pin[pin][i].g = 200;
            }
            leds_pin[pin][pin].r = 200;
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

