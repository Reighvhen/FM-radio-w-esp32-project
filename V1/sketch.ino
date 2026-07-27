//a regular radio just playing after plugging in
#include <RDA5807.h>
RDA5807 rx;

void setup(){
  rx.setup();  // Initialize the RDA5807 module
  rx.setFrequency(9170); // Tune to 91.7 MHz
  rx.setVolume(1); //  Set the volume level (0 to 15)
}

void loop(){
  // Nothing needs to happen repeatedly.
  // The radio continues playing the selected station.
}
