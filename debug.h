/*
Wiggle Wall
by Clark Reinholtz,

This is the set of functions and objects specific to the Wiggle Wall, a 40 foot by 5 foot LED wall with ws2811's

This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

#pragma once

#define DEBUG_MILLIS 10000



void debug(){
    // create temporary test pattern for figuring out if the hardware is setup correctly
    // reds create a ramp increasing as index increases
    // greens are the beginning of each input strip and indicate which pin we are on (1 to 8)
    // blues are on the second serpentine (this is wrong in the simulation because serpentine layout is messed up, should be top going down in hardware)
    if (millis() < DEBUG_MILLIS){
        //figure out pixel order within the LEDs array
        //Fastest changing index seems to be width (THIS % WIDTH)
        for (int pin = 0 ; pin < NUM_PINS; pin++){
            for (int led = 0 ; led < LEDS_PER_PIN; led++){
                int thisPixel = pin*LEDS_PER_PIN+led;
                leds[thisPixel].r = thisPixel*255/NUM_LEDS;
                leds[thisPixel].b = 0;
                leds[thisPixel].g = 0;

                if(led <= pin){
                    leds[thisPixel].g = 200;
                }

#if NOT_SIMULATION
                if(led == MATRIX_HEIGHT){ //serpentine
                    leds[thisPixel].b = 200;
                }
                if(led == MATRIX_HEIGHT+1){
                    leds[thisPixel].b = 100;
                }
#else
                if(led == MATRIX_HEIGHT*2-1){ //regular matrix
                    leds[thisPixel].b = 200;
                }
                if(led == MATRIX_HEIGHT*2-2){
                    leds[thisPixel].b = 100;
                }
#endif
            }
        }
    }
}//debug