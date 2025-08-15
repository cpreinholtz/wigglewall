/*
CaptureAudio

by Clark Reinholtz, 
Audio input DSP

VO.42 beta version
 
This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/
#pragma once

#ifndef SIMULATION
    #include <Audio.h>
    //include this^&@
    AudioInputI2S2            audioSource;           //xy=698,360
    AudioAnalyzeFFT256       fft256_1;       //xy=1152,492
    AudioAmplifier           amp1;           //xy=470,93
    AudioConnection          patchCord0(audioSource, 0, amp1, 0);
    AudioConnection          patchCord2(amp1, 0, fft256_1, 0);

    #define USE_AUDIO
#endif



//if the input is greater than current filter output, filter is "pushed up" to input level,  otherwise it decays at a steady rate
//essentially a high pass (pass through?) for up swings and a low pass for downward motion
class PushUpDecay {

public:

    uint8_t signal=0; //lowpass iir value
    uint8_t latchedSignal=0; //lowpass iir value

    unsigned long startMillis = 0; //how long has it been since the last update
    unsigned long decayMillis = 500; //decay time total

    void update(uint8_t v){
        //pushup the latched signal if input is greater than output, capture this as the "start" of a new decay process
        if (v > signal) {
            latchedSignal = v;
            startMillis = millis();
        }

        //update current signal value
        unsigned long elapsedMillis = millis()-startMillis;
        if (elapsedMillis < decayMillis){
            //fade from latchedSignal to 0 based on elapsedMillis
            signal = lerp8by8(latchedSignal,0, 255*elapsedMillis/BRIGHTNESS_FADE_MILLIS);
        } else {
            signal = 0.0;
        }
    }
};

class IIR {

public:

    float signal=0; //lowpass iir value

    float iir_weight = .0005; // weight of the current samples per elapsed milli 
    //making weight as multiplier per millisecond makes this lowpass have the same time constant the same no matter what the update rate is
    // if iir_weight = .001 then thisSampleWeight is = .01 every 10 millis update = .1 / 100ms update = 1.0/1s update
    // if iir_weight = .0005 then thisSampleWeight is = .005 every 10 millis update... you get the idea

    unsigned long startMillis = 0; //how long has it been since the last update

    void update(float v){
        unsigned long elapsedMillis = millis()-startMillis;
        startMillis = millis();

        //thisSampleWeight based off elapsed time
        float thisSampleWeight = iir_weight * elapsedMillis;

        //cap time*weight to 0 to .5 for sanity,  wouldn't be much of a low pass otherwise
        if (thisSampleWeight > .5) thisSampleWeight = .5;
        else if (thisSampleWeight < 0) thisSampleWeight = 0;

        // output = weighted new sample + weighted feedback
        signal =  (v * thisSampleWeight) + (signal * (1.0 - thisSampleWeight));
    }
};



class CaptureAudio {

public:

    //filtered signals
    IIR iir_volume; //lowpass iir tuned to a VERY slow update time for measuring volume
    PushUpDecay sticky;//essentially a high pass (pass through?) for up swings and a low pass for downward motion, decay time set in constructor

    //intermediate signals
    float bin0; // current value in bin0 direct from fft
    uint8_t normalizedSignal; // bin0 after going through high pass and normalization from 0 to 255

    void start(){
        #ifdef USE_AUDIO
            fft256_1.averageTogether(3); //runs at 300Hz+, lets slow that down to ~ 100 hz
            AudioMemory(50);
            amp1.gain(20); // amplify sign to useful range
        #endif
    }


    void debug(){
        Serial.print("bin0:");
        Serial.print(bin0);
        Serial.print(",");

        Serial.print("normalizedSignal:");
        Serial.print(normalizedSignal);
        Serial.print(",");
        
        Serial.print("volume:");
        Serial.print(iir_volume.signal);
        Serial.print(",");
        
        Serial.print("sticky:");
        Serial.print(sticky.signal);
        Serial.print(",");
    }

    //normalize latest results from 0 to 255
    uint8_t highpassAndNormalize(float inputSignal, float lowpass){

        //simple IIR highpass by subtracting a low-pass filter's output from the original input signal
        float highpass = inputSignal - lowpass;

        //dont bother if highpass is negative
        if (highpass <= 0 ) return 0;

        //now normalize these results by multiplying by a scaler and dividing by lowpass outputs
        float equalizedScaler = 50.0;  // "average" normalized value
        //because of the highpass = inputSignal - lowpass above..
        // if this sample =    average volume, then normalizedFloat = 0
        // if this sample = 2X average volume, then normalizedFloat = equalizedScaler
        // if this sample = 3X average volume, then normalizedFloat = 2x equalizedScaler
        // if this sample = 4X average volume, then normalizedFloat = 3x equalizedScaler
        // so we are really only passing values ABOVE the lowpass value
        float normalizedFloat = equalizedScaler * highpass / lowpass;

        if (normalizedFloat > 255) return 255; //cap at max

        //return uint8_t
        return uint8_t(normalizedFloat);

    }

    void update(){
        #ifdef USE_AUDIO
            //check if fft results are new
            if (fft256_1.available()){

                //get latest reults from bin 0 (low frequency)
                bin0 = fft256_1.read(0);

                //sanitize clip below 0
                if (bin0 < 0) bin0 = 0;

                //normalize and convert to uint8_t
                normalizedSignal = this->highpassAndNormalize(bin0, iir_volume.signal);

                //send normalized signal through the PushUpDecay
                sticky.update(normalizedSignal);

                //update iir_volume for next time
                iir_volume.update(bin0);
            }
        #endif
    }
}; 
