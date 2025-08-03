# TOP PRIORITY TODO's
 - port example for OCTO output!!!!
    - try 25x64 wall dimensions
    - try serpentine xymap
    - why is instantiating octo class breaking the hardware output?
    - can I just use single or 8 fastled outputs???
    - need to figure out how to change radial filter radias???!!!!
    - port / fix for your weird mapping!!! (maybe just make serpentine for simplicity!!)
 - re-add hue shifting

 - beeping is due to XY map being non square!!!!
    trying my own map here https://forum.arduino.cc/t/irregular-shape-led-matrix/1172989/8  https://macetech.github.io/FastLED-XY-Map-Generator/

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
