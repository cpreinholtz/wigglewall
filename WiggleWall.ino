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

//******************************************************************************************************************
#include "wigglewall.h" 
//******************************************************************************************************************

#define LED_PIN 3

// This is purely use for the web compiler to display the animartrix effects.
// This small led was chosen because otherwise the bloom effect is too strong.
#define LED_DIAMETER 0.15  // .15 cm or 1.5mm





#if HARDWARE
WiggleWall wiggleWall();
#endif

Manager manager;


void setup() {
    ////////////////////////////////////////////////////////////////////////////
    //
    auto screen_map = xyMap.toScreenMap();
    screen_map.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2811, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_map);

    manager.start();
#if HARDWARE
    wiggleWall.setup();
#endif

}



int lastSlider = -1;

void loop() {
    manager.setDesiredBrightness(brightness);

    fxEngine.setSpeed(timeSpeed);
    if (int(fxIndex) != lastSlider){
        lastSlider = int(fxIndex);
        manager.setPattern(lastSlider);
        //animartrix.fxSet(lastSlider);
    }

    manager.run();

    fxEngine.draw(millis(), leds);

#if SIMULATION
    debug();
    FastLED.show();
#else
    wiggleWall.copyBuffer();
#endif

}


