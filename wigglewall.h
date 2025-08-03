/*
Wiggle Wall
by Clark Reinholtz,

This is the set of functions and objects specific to the Wiggle Wall, a 40 foot by 5 foot LED wall with ws2811's

This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

#pragma once

//******************************************************************************************************************
//define running modes for simple #if compile switches
#define SIMULATION true
#define NOT_SIMULATION (not SIMULATION)


//******************************************************************************************************************
#if NOT_SIMULATION
    #define MATRIX_WIDTH 64
    #define MATRIX_HEIGHT 25
#else
    #define MATRIX_WIDTH 32
    #define MATRIX_HEIGHT 32
#endif

#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define NUM_PINS 8
#define LEDS_PER_PIN (NUM_LEDS / NUM_PINS)

#include "manager.h"
#include "debug.h"


#if NOT_SIMULATION

    //******************************************************************************************************************
    //Octo stuff
    //******************************************************************************************************************
    #include <OctoWS2811.h>

    byte pinList[NUM_PINS] = {12,40,27,25,26,34,41,28};
    const int bytesPerLED = 3;  // change to 4 if using RGBW
    DMAMEM int displayMemory[NUM_LEDS * bytesPerLED / 4];
    int drawingMemory[NUM_LEDS * bytesPerLED / 4];
    const int config = WS2811_RGB | WS2811_800kHz;

    OctoWS2811 oleds(LEDS_PER_PIN, displayMemory, drawingMemory, config, NUM_PINS, pinList);



    //******************************************************************************************************************
    //WiggleWall Class
    //******************************************************************************************************************

    class WiggleWall {
    public:

        void setup(){
            oleds.begin();
        }

        void copyBuffer(){
            for (int thisPixel = 0 ; thisPixel < NUM_LEDS; thisPixel++){
                int r = leds[thisPixel].r;
                int g = leds[thisPixel].g;
                int b = leds[thisPixel].b;
                int color = (((int)(r))<<16) | ((int)(g)<<8) | ((int)(b));
                oleds.setPixel(thisPixel, color);
            }
        }// copyBuffer


    }; // class WiggleWall

//if HARDWARE ^^^
#endif
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

using namespace fl;


#define LED_PIN 12
#define BRIGHTNESS 96
#define COLOR_ORDER GRB

#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 25

#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

#define FIRST_ANIMATION POLAR_WAVES

// This is purely use for the web compiler to display the animartrix effects.
// This small led was chosen because otherwise the bloom effect is too strong.
#define LED_DIAMETER 0.15  // .15 cm or 1.5mm


CRGB leds[NUM_LEDS];
XYMap xyMap = XYMap::constructRectangularGrid(MATRIX_WIDTH, MATRIX_HEIGHT);


UITitle title("Animartrix");
UIDescription description("Demo of the Animatrix effects. @author of fx is StefanPetrick");

UISlider brightness("Brightness", 255, 0, 255, 1);
UINumberField fxIndex("Animartrix - index", 0, 0, NUM_ANIMATIONS - 1);
UINumberField colorOrder("Color Order", 0, 0, 5);
UISlider timeSpeed("Time Speed", 1, -10, 10, .1);

Animartrix animartrix(xyMap, FIRST_ANIMATION);
FxEngine fxEngine(NUM_LEDS);

/////////////////////////////////////////////////////////////////////////////
#include "manager.h"

Manager manager;

void setup() {
    auto screen_map = xyMap.toScreenMap();
    screen_map.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2811, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_map);
    FastLED.setBrightness(brightness);
    fxEngine.addFx(animartrix);

    colorOrder.onChanged([](int value) {
        switch(value) {
            case 0: value = RGB; break;
            case 1: value = RBG; break;
            case 2: value = GRB; break;
            case 3: value = GBR; break;
            case 4: value = BRG; break;
            case 5: value = BGR; break;
        }
        animartrix.setColorOrder(static_cast<EOrder>(value));
    });
    /////////////////////////////////////////////////////////////////////////////
    Serial.begin(115200);
}


void loop() {
    static unsigned long startMillis = millis();
    /////////////////////////////////////////////////////////////////////////////
    EVERY_N_MILLIS(100) manager.setDesiredBrightness(brightness);
    EVERY_N_MILLIS(100) manager.setSpeed(timeSpeed);
    static int lastFxIndex = -1;
    if (fxIndex.value() != lastFxIndex) {
        lastFxIndex = fxIndex;
        manager.setPattern(fxIndex);
    }
    fxEngine.draw(millis(), leds);
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
    EVERY_N_MILLIS(1000) Serial.print("FPS");
    EVERY_N_MILLIS(1000) Serial.print(1000.0/float(endMillis - startMillis));
    
}