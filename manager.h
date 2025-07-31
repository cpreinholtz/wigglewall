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
#if SIMULATION
UISlider brightness("Brightness", 20, 0, 255);
UISlider fxIndex("ANIM Index", 0, 0, NUM_ANIMATIONS - 1,1);
UISlider timeSpeed("Time Speed", 1, -10, 10, .1);
#endif

//******************************************************************************************************************
//


// how many seconds between swapping animations
#define ANIMATION_UPDATE_PERIOD 100

// how many seconds should the brightness fade for
#define BRIGHTNESS_FADE_MILLIS 1000

typedef enum {
  sIdle,
  sFadeIn,
  sFadeOut
} envelopState;

class Manager {
public:

    //animation
    bool doAnimationUpdates = true;
    int currentAnimation = 0;

    //brightness
    int desiredBrightness = 100;

    // state machine
    unsigned long startMillis = 0;
    int state=sIdle;

    void start(){
        this->setBrightness(desiredBrightness);
        this->setPattern(currentAnimation);

    }

    void run(){
        if (doAnimationUpdates) {
            EVERY_N_SECONDS(ANIMATION_UPDATE_PERIOD) trigger();
        }

        if (state == sFadeOut){
            fadeOut();
        } else if (state == sFadeIn){
            fadeIn();
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    // State machine
    ////////////////////////////////////////////////////////////////////////////
    void trigger(){
        if (state == sIdle){
            startMillis = millis();
            state = sFadeOut;
        }
    }


    // fade from desiredBrightness to 0, then change animations and transition to sFadeIn
    void fadeOut(){
        unsigned long elapsedMillis = millis()-startMillis;
        if (elapsedMillis < BRIGHTNESS_FADE_MILLIS){
            setBrightness(lerp8by8(desiredBrightness,0, 255*elapsedMillis/BRIGHTNESS_FADE_MILLIS));
        } else {
            setBrightness(0);
            randomPattern();
            state = sFadeIn;
            startMillis = millis();
        }
    }

    // fade from 0 to desiredBrightness, then transition to sIdle
    void fadeIn(){
        unsigned long elapsedMillis = millis()-startMillis;
        if (elapsedMillis < BRIGHTNESS_FADE_MILLIS){
            setBrightness(lerp8by8(0,desiredBrightness, 255*elapsedMillis/BRIGHTNESS_FADE_MILLIS));
        } else {
            setBrightness(desiredBrightness);
            state = sIdle;
        }
    }



    ////////////////////////////////////////////////////////////////////////////
    // Brightness updates
    ////////////////////////////////////////////////////////////////////////////
    void setBrightness(int brightness){
        brightness = constrain(brightness,0,255);
        FastLED.setBrightness(brightness);
    }


    // allow user to set brightness but wont disrupt state machine if we are currently dimming
    void setDesiredBrightness(int brightness){
        desiredBrightness = constrain(brightness,0,255);
        if (state == sIdle) {
            setBrightness(desiredBrightness);
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    // Animation updates
    ////////////////////////////////////////////////////////////////////////////
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

