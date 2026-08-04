
//this has 2 buttons for volume control and 2 led for red and green
// The execution order is:
                          // Global variable declarations/initializations — once, at the very start
                          // setup() — once, right after
                          // loop() — runs, finishes, then immediately runs again, forever
  //You never go back to setup() unless the ESP32 resets or loses power. 

#include <RDA5807.h>
RDA5807 rx;
#define Bmute 18
#define Bup 4
#define Bdown 5
#define red 16
#define green 17

byte lastMuteState;
byte lastUp;
byte lastDown;

bool isOn = false; //
byte ledState = LOW; // initialize before anything that LED's are off

int maxVol = 15;

void setup()
{ //-----------------------------
  pinMode(Bmute, INPUT_PULLUP); //This tells the ESP32 "GPIO pin Bmute is an input, and enable the internal pull-up resistor."
  lastMuteState = digitalRead(Bmute);//This reads the initial state of the button the moment the program starts 
                                        //— before any button has been pressed. 
                                        // HIGH = not pressed, LOW = pressed cause its going to connect to GND thus giving us 0V

  pinMode(Bup, INPUT_PULLUP);
  lastUp = digitalRead(Bup);

  pinMode(Bdown, INPUT_PULLUP);
  lastDown = digitalRead(Bdown);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  
  //-----------------------------
  Serial.begin(115200);
  rx.setup(); //call rx.powerUp: initialize I2C communication & (DMUTE, DHIZ, ENABBLE)=1, & VOLUME=0
  rx.setFrequency(9170); //Freq of the radio we want to tune in
  rx.setVolume(0); // could be redundant cause the rx.setup already set vol=0
  rx.setMute(true); //true=muted, since the rx.setup unmutes it, we want to unmute it again so that it would be quiet when just plug in to power supply

}

void loop()
{

  
  byte nowMuteState = digitalRead(Bmute); // read the current state of the mute button

  Serial.print("lastMute: ");
  Serial.print(lastMuteState);
  Serial.print(" nowMute: ");
  Serial.println(nowMuteState);

  //Then we can compare the current state to the last one (from the previous execution of the loop function). 4 possibilities here:
      // 1. LOW -> LOW (last -> current).
      // 2. LOW -> HIGH.
      // 3. HIGH -> LOW.
      // 4. HIGH -> HIGH.
        // if last =! current state, it means that button has been pressed, 
        // if it was equal it means that button was not pressed cause the state stayed the same
  // we only enter the next block of code if the current and last state are different
  if (lastMuteState != nowMuteState) 
  { 
    
    //At this point we have only 2 possibilities left:
      // 1. Either the previous state was LOW and the current state is HIGH (pressed to not pressed). --> release
      // 2. Or the previous state was HIGH and the current state is LOW (not pressed to pressed). --> push
    // We interested in the moment the button goes down (pressed), not when it comes back up. 
      // Because ppl expect things to happen when button is pushed and not when they release.
    if (isOn == false && lastMuteState == HIGH && nowMuteState == LOW ) // if are off (currently) and press the button == turning ON
    {
      rx.setMute(false);
      //rx.setup();  // actually also unmutes the radio too
      //rx.setFrequency(9170); //Freq of the radio we want to tune in

      rx.setVolume(0); // set the volume to 0

      //turning on the green led and off the red
      digitalWrite(green, !ledState);
      digitalWrite(red, ledState); 
      isOn = true;
    }

    else if (isOn == true && lastMuteState == HIGH && nowMuteState == LOW ) // if are on and press the button == turning OFF
    {
      //rx.powerDown(); // Power the receiver off: doesnt touch the frequency, thus theres no need to initialize the freq again
      rx.setMute(true);
      rx.setVolume(0); //reset the volume back to 0
      
      //turning on the red led and off the green
      digitalWrite(red, !ledState); 
      digitalWrite(green, ledState);
      isOn = false;
    }

    delay(50);
    // now we move the current state to be the previous state so next time the loops run again it has the correct prev state
    lastMuteState = nowMuteState;
    
    //toggle the isOn state to get the current state of the isOn, prev we are using its prev state to check the next step
   // isOn = !isOn;
  }


  byte nowUp = digitalRead(Bup); // read the current state of the volume up button
  byte nowDown = digitalRead(Bdown); // read the current state of the volume down button
  
  if (isOn == true) // only if we are on can we use the volume buttons
  {
    //UP
    if (lastUp != nowUp) // pressed: if prev state =! now state
    {
      if (lastUp == HIGH && nowUp == LOW) // if pressed up
      {
        
        if (rx.getMute() == true)
        {
          rx.setMute(false);
          rx.setVolume(0);
        }
        else
        {
          int volume = rx.getVolume();

          if (volume == 15) // if we are currently in the max volume, stay at max volume
          {
            rx.setVolume(15); 
          }
          else
          {
            rx.setVolume(volume+1); //or else just up the volume
          }
        }
      }
    }



    //DOWN
    if (lastDown != nowDown) // pressed if prev state =! now state
    {
      if (lastDown == HIGH && nowDown == LOW) // if pressed down
      {
        int volume = rx.getVolume();

        if (volume == 0) // if we aee in volume=0 (not muted)
        {
          rx.setMute(true); // we want real silence so we set it to real mute
        }
        else
        {
          rx.setVolume(volume-1); // else just go down
        }
      }
    }  
  }
  lastUp = nowUp;
  lastDown = nowDown;
}
