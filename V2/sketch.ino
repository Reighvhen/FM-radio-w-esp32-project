//adding a on and off button
#include <RDA5807.h>
RDA5807 rx;
#define BUTTON_PIN 18

int lastButtonState;

void setup(){
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lastButtonState = digitalRead(BUTTON_PIN);
  
  rx.setup();
  rx.setFrequency(9170);
  rx.setVolume(1);
  rx.setMute(true);
  
}

void loop(){
  // read the value of the button
  int buttonState = digitalRead(BUTTON_PIN);
  
  bool currentState = rx.getMute();

  if (lastButtonState==HIGH && buttonState==LOW) {//state change

    rx.setMute(!currentState);

    delay(50)
    
  }
  lastButtonState = buttonState;
}
