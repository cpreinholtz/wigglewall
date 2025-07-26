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
#define HARDWARE (not SIMULATION)

//used for extra switches like hardware mapping, etc
#define DEBUG true


//******************************************************************************************************************
#if HARDWARE
    #define MATRIX_WIDTH 64
    #define MATRIX_HEIGHT 25
#else
    #define MATRIX_WIDTH 64
    #define MATRIX_HEIGHT 64
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

    // Any group of digital pins may be used
    const int numPins = 8;

    //top jack, bottom jack
    //orange, blue, green, brown, orange, blue, green, brown
    //defaults {2, 14, 7, 8, 6, 20, 21, 5};
    //I USED {2, 14, 7, 8, 6, 20, 21,***9***}; on the waggle because I needed pin 5 for I2S
    //byte pinList[numPins] = {2, 14, 7, 8, 6, 20, 21, 5};

    //mounting the octo shifter on the teensy 4.1 (dont connect teensy 3.3v to octo pin 15~!!!)
    //see blue notebook for explanation
    //results in T34 = O14, T33 = O13... 
    //orange, blue, green, brown, orange, blue, green, brown
    //byte pinList[numPins] = {12,34,27,28,  26,40,41,25};







    // wall top is  orange brown    blue green
    //pipe  top      1      8       2     7

    // wall bottom is green blue    brown orange
    //pipe  bot      3      6       4     5

    //so order is Lorange Rblue RGreen,Lbrown
    byte pinList[numPins] = {12,40,27,25,26,34,41,28};


    //const int nMaxPixels = nMaxPixels; // set this to your MAX leds per strip
    const int bytesPerLED = 3;  // change to 4 if using RGBW
    DMAMEM int displayMemory[NUM_LEDS * bytesPerLED / 4];
    int drawingMemory[NUM_LEDS * bytesPerLED / 4];
    const int config = WS2811_GRB | WS2811_800kHz;

    OctoWS2811 oleds(nMaxPixels, displayMemory, drawingMemory, config, numPins, pinList);



    //******************************************************************************************************************
    //WiggleWall Class
    //******************************************************************************************************************

    class WiggleWall {
    public:

        void setup(){
            oleds.begin();
        }





        void copyBuffer(){
            int thisPixel = 0;
            for (int ring = 0 ; ring < nRings; ring++){
                for (int pixel = 0; pixel < nPixelsPerRing[ring]; pixel++){
                    float r = leds[thisPixel].r;
                    float g = leds[thisPixel].g;
                    float b = leds[thisPixel].b;
                    int color = (((int)(r))<<16) | ((int)(g)<<8) | ((int)(b));
                    oleds.setPixel(pixel+ring*nMaxPixels, color);
              }
            }
        }// copyBuffer


    }; // class WiggleWall

//if HARDWARE ^^^
#endif
