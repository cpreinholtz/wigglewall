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

using namespace fl;


#define LED_PIN 12
#define BRIGHTNESS 96
#define COLOR_ORDER GRB

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
CRGB leds0[200];
CRGB ledsFull[1600];
XYMap xyMap = XYMap::constructRectangularGrid(MATRIX_WIDTH, MATRIX_HEIGHT);
//XYMap xyMap = XYMap::constructSerpentine(MATRIX_WIDTH, MATRIX_HEIGHT);
XYMap xyMap0 = XYMap::constructSerpentine(8, 25);
XYMap xyMapFull = XYMap::constructSerpentine(64, 25);
//XYMap xyMap = XYMap::constructWithUserFunction(MATRIX_WIDTH, MATRIX_HEIGHT, XY);
//XYMap xyMap = XYMap::constructWithLookUpTable(MATRIX_WIDTH, MATRIX_HEIGHT, XYTable);
/////////////////////////////////////////////////////////////////////////////


UITitle title("Animartrix");
UIDescription description("Demo of the Animatrix effects. @author of fx is StefanPetrick");

UISlider brightness("Brightness", 255, 0, 255);
UINumberField fxIndex("Animartrix - index", 0, 0, NUM_ANIMATIONS - 1);
UINumberField colorOrder("Color Order", 0, 0, 5);
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
/*
    auto screen_map0 = xyMap0.toScreenMap();
    screen_map0.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2811, LED_PIN, COLOR_ORDER>(leds0, 25*8)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_map0);    */

    auto screen_mapFull = xyMapFull.toScreenMap();
    screen_mapFull.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2811, LED_PIN, COLOR_ORDER>(ledsFull, 1600)
        .setCorrection(TypicalLEDStrip)
        .setScreenMap(screen_mapFull);


    FastLED.setBrightness(brightness);
    Serial.begin(115200);
}

void loop() {
    /////////////////////////////////////////////////////////////////////////////
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

    static unsigned long drawMillis = millis();//////////////

    /*
    for (int y = 0; y < 25; y++){
        for (int x = 0; x < 8; x++){
            int xx = 32-4+x;
            int yy = 32-12+y;
            //copy just the middle of the 64x64 to leds0 as a test
            leds0[xyMap0(x,y)] = leds[xyMap(xx,yy)];
        }
    }*/

    upscale(leds, ledsFull, MATRIX_WIDTH, MATRIX_HEIGHT, xyMapFull);

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
    EVERY_N_MILLIS(1000) Serial.print("FPS: ");
    EVERY_N_MILLIS(1000) Serial.print(1000.0/float(endMillis - startMillis));

    EVERY_N_MILLIS(1000) Serial.print(", draw time: ");
    EVERY_N_MILLIS(1000) Serial.print(drawMillis - startMillis);    

    EVERY_N_MILLIS(1000) Serial.print(", draw time: ");
    EVERY_N_MILLIS(1000) Serial.print(copyMillis - drawMillis);


    EVERY_N_MILLIS(1000) Serial.print(", push time: ");
    EVERY_N_MILLIS(1000) Serial.println(endMillis - copyMillis);
    
}

