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
#define SIMULATION false
#define HARDWARE (not SIMULATION)

//used for extra switches like hardware mapping, etc
#define DEBUG true


//******************************************************************************************************************
#if HARDWARE
    #define MATRIX_WIDTH 64
    #define MATRIX_HEIGHT 25
#else
    #define MATRIX_WIDTH 32
    #define MATRIX_HEIGHT 32
#endif

#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define NUM_PINS 8
#define LEDS_PER_PIN (NUM_LEDS / NUM_PINS)

CRGB leds[NUM_LEDS];

#define COLOR_ORDER RGB



#include "manager.h"
#include "debug.h"


#if HARDWARE

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
