/*
Wiggle Wall
by Clark Reinholtz,

This is the set of functions and objects specific to the Wiggle Wall, a 40 foot by 5 foot LED wall with ws2811's

This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

#pragma once



//******************************************************************************************************************
//Animartrix and effects engine
#if HARDWARE
    XYMap xyMap = XYMap::constructSerpentine(MATRIX_WIDTH, MATRIX_HEIGHT);
#else
    XYMap xyMap = XYMap::constructRectangularGrid(MATRIX_WIDTH, MATRIX_HEIGHT);
#endif

Animartrix animartrix(xyMap, POLAR_WAVES);
FxEngine fxEngine(NUM_LEDS);

//******************************************************************************************************************
//User Controls

UISlider brightness("Brightness", 20, 0, 255);
UISlider fxIndex("ANIM Index", 0, 0, NUM_ANIMATIONS - 1,1);
UISlider timeSpeed("Time Speed", 1, -10, 10, .1);


//******************************************************************************************************************
//


// how many seconds between swapping animations
#define ANIMATION_UPDATE_PERIOD 10


class Manager {
public:


    bool doAnimationUpdates = true;
    int currentAnimation = 0;

    void run(){
        if (doAnimationUpdates) {
            EVERY_N_SECONDS(ANIMATION_UPDATE_PERIOD) randomPattern();
        }
    }

    // randomly update animation
    void randomPattern(){
        setPattern(random(NUM_ANIMATIONS));
    }
    //set animation to a certain index
    void setPattern(int i){
        currentAnimation = constrain(i,0,NUM_ANIMATIONS-1);
        animartrix.fxSet(currentAnimation);
    }

};

