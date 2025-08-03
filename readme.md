# TOP PRIORITY TODO's
 - port example for OCTO output!!!!
    - try paralelle teensy outputs https://github.com/FastLED/FastLED/blob/master/examples/TeensyMassiveParallel/TeensyMassiveParallel.ino
    - port / fix for your weird mapping!!! (maybe just make serpentine for simplicity!!)

 - beeping is due to XY map being non square!!!!
    - XXX trying my own map using these https://forum.arduino.cc/t/irregular-shape-led-matrix/1172989/8  https://macetech.github.io/FastLED-XY-Map-Generator/
    - try 25x25 with upscale = XXms draw 6ms push
    - try 64x64 and use middle or downscale? = 16ms draw 6ms push
        - push time = 32 leds/ms = for 200 leds(1600/8) should be about 6.4ms
        - draw time ~ 16 ms 
        - nearly 100 fps still using octo
    - try issueing a complaint / problem report to allow non square XYmaps


 - Teensy 4.1: 50 parallel pins
    https://github.com/FastLED/FastLED/blob/master/examples/TeensyMassiveParallel/TeensyMassiveParallel.ino


 - re-add hue shifting
 - fix / redo the led mounting!!!!
 - make human knob interface for brightness / time
 - re-add music reactivity

# If you have time:
 - redo curtain with 1/2 the horizontal spacing (16 strips per pole?)  this would essentially make the curtain 20x6 instead of 40x5 (this limits the power / data length needed and makes conversion from simulation much simpler)
 - redo curtain for true serpentine layout (doubles the power runs which may actually help with dropout issues)

# Other nice to haves
 - re-add moddables
 - re-add dynamics like center shifting?
 - add knobs to play with!!! maybe just make separate module
