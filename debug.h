/*
Wiggle Wall
by Clark Reinholtz,

This is the set of functions and objects specific to the Wiggle Wall, a 40 foot by 5 foot LED wall with ws2811's

This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

#pragma once





    void debug(){
#if DEBUG

    // create temporary test pattern for figuring out if the hardware is setup correctly
    // reds create a ramp increasing as index increases
    // blues create diagonal lines
    // greens are the beginning of each input strip (every 8th collumn)
    if (millis() < 2000){
        //figure out pixel order within the LEDs array
        //Fastest changing index seems to be width (THIS % WIDTH)
        for (int pin = 0 ; pin < NUM_PINS; pin++){
            for (int led = 0 ; led < LEDS_PER_PIN; led++){
                int thisPixel = pin*LEDS_PER_PIN+led;
                leds[thisPixel].r = thisPixel*255/NUM_LEDS;
                leds[thisPixel].b = 0;
                leds[thisPixel].g = 0;

                if(led == 0){
                    leds[thisPixel].g = 200;
                }
                if(led == 1){
                    leds[thisPixel].g = 100;
                    leds[thisPixel].b = 100;
                }
                if(led == MATRIX_HEIGHT){
                    leds[thisPixel].b = 200;
                }
                if(led == MATRIX_HEIGHT+1){
                    leds[thisPixel].g = 100;
                    leds[thisPixel].b = 100;
                }
            }
        }
    }


#endif

    }//debug