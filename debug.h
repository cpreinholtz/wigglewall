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
    // reds create a ramp from 0 to MATRIX_WIDTH
    // blues create diagonal lines
    // greens are the beginning of each input strip (every 8th collumn)
    if (millis() < 5000){
        //figure out pixel order within the LEDs array
        //Fastest changing index seems to be width (THIS % WIDTH)
        for (int y = 0 ; y < MATRIX_HEIGHT; y++){
            for (int x = 0 ; x < MATRIX_WIDTH; x++){
                int thisPixel = y*MATRIX_WIDTH+x;
                leds[thisPixel].r = x;
                leds[thisPixel].b = 0;
                leds[thisPixel].g = 0;

                if( (x%MATRIX_HEIGHT)==y){
                    leds[thisPixel].b = 200;
                }

                if(y== 0 and (x%8) == 0){
                    leds[thisPixel].g = 200;
                }
            }
        }
    }


#endif

    }//debug