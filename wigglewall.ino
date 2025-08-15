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
// 41  |  L  | Green
// 40  |  L  | Blue
// 25  |  L  | Brown
// 26  |  L  | Orange  <- this seems to be a problem pin for some reason, some flashing
// 12  |  R  | Orange
// 28  |  R  | Brown
// 34  |  R  | Blue
// 27  |  R  | Green  <- this seems to be a problem pin for some reason, lots of flashing
// ^^ This is a very specific order that creates a mirrored wall layout, where the "left" and "right" sides are interchangeable

// The dataflow direction of each wall pannel is flipped for easier cable runs (see graphic below with <- and -> denoting the dataflow direction)
// |<-Gr| |Bl->| |<-Br| |Or->| |<-Or| |Br->| |<-Bl| |Gr->|

// This is a very specific order that creates a mirrored wall layout, where the "left" and "right" sides are interchangeable
#define LED_PIN0 41
#define LED_PIN1 40
#define LED_PIN2 25
#define LED_PIN3 26
#define LED_PIN4 12
#define LED_PIN5 28
#define LED_PIN6 34
#define LED_PIN7 27




//NOTE REQUIRES REDEFINING THE radial_filter_radius:::       if (w > h) { this->radial_filter_radius = int(float(w)*23.0/32.0); } else { this->radial_filter_radius = int(float(h)*23.0/32.0); }
// AND requires fastled master branch for non square fixes.  This is the commit I am currently on during development: 2ff981b7d
#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 25
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

#define COLOR_ORDER RGB
#define BRIGHTNESS 200
#define FIRST_ANIMATION POLAR_WAVES
#define LED_DIAMETER 0.15  // .15 cm or 1.5mm // This is purely use for the web compiler to display the animartrix effects.

/////////////////////////////////////////////////////////////////////////////
//break up my 64x25 wall into 8 8x25 pannels
#define PANNEL_WIDTH 8
#define PANNEL_HEIGHT 25
#define NUM_LEDS_PER_PANNEL (PANNEL_WIDTH * PANNEL_HEIGHT)
#define NUM_PANNELS 8

CRGB ledsFull[NUM_LEDS]; //this is what animartrix renders into
CRGB leds_pin[NUM_PANNELS][NUM_LEDS_PER_PANNEL]; // and then I split the image into 8 parts for teensy output

XYMap xyMapFull = XYMap::constructRectangularGrid(MATRIX_WIDTH, MATRIX_HEIGHT);

//flip individual outputs because serpentine starts left to right then down, but my wall is up and down then left right
FASTLED_FORCE_INLINE uint16_t xy_serpentine_vertical(uint16_t x, uint16_t y,
                                            uint16_t width, uint16_t height) {
    (void)width;
    if (x & 1) // Even or odd row?
        // reverse every second line for a serpentine led layout
        return (x + 1) * height - 1 - y;
    else
        return x * height + y;
}

XYMap xyMap_pin = XYMap::constructWithUserFunction(PANNEL_WIDTH, PANNEL_HEIGHT, xy_serpentine_vertical);



/////////////////////////////////////////////////////////////////////////////
// Clark's stuff
/////////////////////////////////////////////////////////////////////////////   
//USE THIS TO ALLOW THE WEB COMPILER TO USE THE FULL 64x25 OUTPUT
#define SIMULATION

#define DEBUG_MILLIS 2000
#include "manager.h"
Manager manager;

#include "captureaudio.h"
CaptureAudio audio;

/////////////////////////////////////////////////////////////////////////////
Animartrix animartrix(xyMapFull, FIRST_ANIMATION);
FxEngine fxEngine(NUM_LEDS);

UISlider modulation("Modulation", 0, 0, sNone, 1);
UISlider fxIndex("Animation", 0, 0, NUM_ANIMATIONS - 1, 1);
UISlider brightness("Brightness", BRIGHTNESS, 0, 255, 1);
UISlider timeSpeed("Time Speed", 1, -10, 10, .1);

UIGroup waveSimControls("Simulation", brightness, timeSpeed, fxIndex, modulation);

void setup() {

    fxEngine.addFx(animartrix);
    animartrix.setColorOrder(static_cast<EOrder>(COLOR_ORDER));
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(brightness);
    Serial.begin(115200);

    /////////////////////////////////////////////////////////////////////////////
#ifndef SIMULATION
    FastLED.addLeds<WS2812, LED_PIN0, COLOR_ORDER>(leds_pin[0], NUM_LEDS_PER_PANNEL);
    FastLED.addLeds<WS2812, LED_PIN1, COLOR_ORDER>(leds_pin[1], NUM_LEDS_PER_PANNEL);
    FastLED.addLeds<WS2812, LED_PIN2, COLOR_ORDER>(leds_pin[2], NUM_LEDS_PER_PANNEL);
    FastLED.addLeds<WS2812, LED_PIN3, COLOR_ORDER>(leds_pin[3], NUM_LEDS_PER_PANNEL);
    FastLED.addLeds<WS2812, LED_PIN4, COLOR_ORDER>(leds_pin[4], NUM_LEDS_PER_PANNEL);
    FastLED.addLeds<WS2812, LED_PIN5, COLOR_ORDER>(leds_pin[5], NUM_LEDS_PER_PANNEL);
    FastLED.addLeds<WS2812, LED_PIN6, COLOR_ORDER>(leds_pin[6], NUM_LEDS_PER_PANNEL);
    FastLED.addLeds<WS2812, LED_PIN7, COLOR_ORDER>(leds_pin[7], NUM_LEDS_PER_PANNEL);
#endif
    /////////////////////////////////////////////////////////////////////////////
#ifdef SIMULATION
    auto screen_mapFull = xyMapFull.toScreenMap();
    screen_mapFull.setDiameter(LED_DIAMETER);
    FastLED.addLeds<WS2812, LED_PIN, COLOR_ORDER>(ledsFull, NUM_LEDS).setCorrection(TypicalLEDStrip).setScreenMap(screen_mapFull);
#endif
    audio.start();
}




void loop() {

    /////////////////////////////////////////////////////////////////////////////
    //update desired with sliders (simulation only)
    /////////////////////////////////////////////////////////////////////////////
    unsigned long startMillis = millis();
    EVERY_N_MILLIS(100) manager.setDesiredBrightness(brightness);
    EVERY_N_MILLIS(100) manager.setSpeed(timeSpeed);
    static int lastModulationIndex = -1;
    if (modulation.value() != lastModulationIndex) {
        lastModulationIndex = modulation;
        manager.setModulation(modulation);
    }
    static int lastFxIndex = -1;
    if (fxIndex.value() != lastFxIndex) {
        lastFxIndex = fxIndex;
        manager.setPattern(fxIndex);
    }

    /////////////////////////////////////////////////////////////////////////////
    //use audio / manager values to update brightness etc for next frame
    /////////////////////////////////////////////////////////////////////////////
    audio.update();
    manager.run(audio.sticky.signal);

    if (manager.clearAnimationChanged()){
        animartrix.fxSet(manager.currentAnimation);
    }
    FastLED.setBrightness(manager.currentBrightness);
    fxEngine.setSpeed(manager.timeSpeed);

    /////////////////////////////////////////////////////////////////////////////
    // Render the raw animation
    /////////////////////////////////////////////////////////////////////////////
    fxEngine.draw(millis(), ledsFull);
    static unsigned long drawMillis = millis();//////////////

    /////////////////////////////////////////////////////////////////////////////
    // apply hue shift
    /////////////////////////////////////////////////////////////////////////////
    CHSV hsv[NUM_LEDS];
    for (int y = 0; y < NUM_LEDS; y++){
        hsv[y] = rgb2hsv_approximate(ledsFull[y]);
        hsv[y].h = hsv[y].h + manager.hueOffset;
    }
    hsv2rgb_rainbow(hsv, ledsFull, NUM_LEDS);

    /////////////////////////////////////////////////////////////////////////////
    // Debug creating a solid gradient at power on to visually test mapping
    /////////////////////////////////////////////////////////////////////////////
    if (millis() < DEBUG_MILLIS){
        for (int y = 0; y < MATRIX_HEIGHT; y++){
            for (int x = 0; x < MATRIX_WIDTH; x++){
                int idx = xyMapFull(x,y);
                ledsFull[idx].r = x*255/MATRIX_WIDTH/2;
                ledsFull[idx].b = y*255/MATRIX_HEIGHT/2; //these just make a nice gradient to visually see mapping
                ledsFull[idx].g = 0;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Split the image into its output pins and associated leds variables
    /////////////////////////////////////////////////////////////////////////////
    for (int pin = 0; pin < NUM_PANNELS; pin++){
        for (int y = 0; y < PANNEL_HEIGHT; y++){
            for (int x = 0; x < PANNEL_WIDTH; x++){
                //get full wall  xx based on current pin*PANNEL_WIDTH and this pin's x
                int xx = pin * PANNEL_WIDTH + x;

                // reverse serpentine for even outputs (X gets inverted, y stays the same)
                int xxx;
                if (pin % 2 == 0) xxx = PANNEL_WIDTH-1-x;
                else xxx = x;

                leds_pin[pin][xyMap_pin(xxx,y)] = ledsFull[xyMapFull(xx,y)]; //todo make this a function, big map xy to little map pin & xy
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////
    // Debug by counting panels and lighting up first N leds in each panel
    // Show the results!!!
    /////////////////////////////////////////////////////////////////////////////
    if (millis() < DEBUG_MILLIS){
        for (int pin = 0; pin < NUM_PANNELS; pin++){
            for (int i = 0; i < pin; i++){
                leds_pin[pin][i].g = 200;
            }
            leds_pin[pin][pin].r = 200;
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    //show frame
    /////////////////////////////////////////////////////////////////////////////
    unsigned long copyMillis = millis();//////////////
    FastLED.show();
    unsigned long pushMillis = millis();

    /////////////////////////////////////////////////////////////////////////////
    //debug only
    /////////////////////////////////////////////////////////////////////////////
    EVERY_N_MILLIS(100) {
        #ifdef SIMULATION
            Serial.print("SIMULATION_MODE");
        #endif

        //Serial.print("drawTime:");
        //Serial.print(drawMillis - startMillis);
        //Serial.print(",");

        //Serial.print(", copyTime:");
        //Serial.print(copyMillis - drawMillis);
        //Serial.print(",");

        //Serial.print("pushTime:");
        //Serial.print(pushMillis - copyMillis);
        //Serial.print(",");

        //Serial.print("otherTime:");
        //Serial.print(millis() - pushMillis);
        //Serial.print(",");

        audio.debug();

        Serial.print("FPS:");
        Serial.print(1000.0/float(millis() - startMillis));
        Serial.print(",");

        Serial.println();
    }
    
}

