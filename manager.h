/*
Wiggle Wall
by Clark Reinholtz,

This is the set of functions and objects specific to the Wiggle Wall, a 40 foot by 5 foot LED wall with ws2811's

This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

#pragma once

/////////////////////////////////////////////////////////////////////////////

// how many seconds between swapping animations
#define ANIMATION_UPDATE_PERIOD 10

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
    bool doBrightnessTransitions = true;
    int desiredBrightness = 100;
    int currentBrightness = 100;

    //speed
    float timeSpeed = 1.0;

    // state machine
    unsigned long startMillis = 0;
    int state=sIdle;

    void start(){
        this->setBrightness(desiredBrightness);
        this->setPatternNow(currentAnimation);
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
            if (doBrightnessTransitions){
                startMillis = millis();
                state = sFadeOut;
                Serial.println("trigger");
            } else {
                randomPattern();
            }
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
            Serial.println("done1");
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
            Serial.println("done2");

        }
    }



    ////////////////////////////////////////////////////////////////////////////
    // brightness updates
    ////////////////////////////////////////////////////////////////////////////
    void setBrightness(int b){
        b = constrain(b,0,255);
        currentBrightness = b;
    }

    // allow user to set brightness but wont disrupt state machine if we are currently dimming
    void setDesiredBrightness(int b){
        desiredBrightness = constrain(b,0,255);
        if (state == sIdle) {
            setBrightness(desiredBrightness);
        }
        Serial.print("Setting desired brightness to: ");
        Serial.println(b);
    }


    ////////////////////////////////////////////////////////////////////////////
    // Animation updates
    ////////////////////////////////////////////////////////////////////////////
    // randomly update animation
    void randomPattern(){
        setPatternNow(random(NUM_ANIMATIONS));
    }

    //set animation to a certain index
    void setPatternNow(int i){
        currentAnimation = constrain(i,0,NUM_ANIMATIONS-1);
        Serial.print("Setting current pattern to: ");
        Serial.println(currentAnimation);
    }    

    //set animation to a certain index
    void fxSet(int i){
        setPatternNow(i);
    }


    /////////////////////////////////////////////////////////////////////////////
    // speed Updates
    /////////////////////////////////////////////////////////////////////////////
    void setSpeed(float t){
        timeSpeed = t;
        Serial.print("Setting timeSpeed to: ");
        Serial.println(timeSpeed);
    }

};

