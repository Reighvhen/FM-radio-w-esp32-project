<h1 align = "center"> ESP32 FM Radio Project Version 1 </h1>
<p>
  This is the first version of the project. The goal was simple: <b> get the radio working. </b> The frequency is hardcoded in the code, meaning the radio automatically tunes to KISS 91.7 when powered on. <i>No buttons, no controls, just plug in and it plays.</i>
</p>
<h2> What I learned </h2>



<h3> Understanding How the ESP32 Configures the RDA5807M </h3> 
<p> 
  The RDA5807M is an independent hardware device with its own internal memory, called registers. When the ESP32 executes functions such as rx.setup(), rx.setFrequency(), and rx.setVolume(), it sends commands over the I²C communication bus to write values into these registers. <br>
  <br> Once these values are written, they remain stored in the RDA5807M as long as the chip continues to receive power (VCC). This is independent of what the ESP32 is doing afterward. The ESP32 can run different code, stop communicating, or even be disconnected (provided the RDA5807M has its own power supply), and the RDA5807M will continue using the last settings stored in its registers.<br>
  
  For example, if the ESP32 configures the RDA5807M to tune to 91.7 MHz, set the volume to 1, and unmute the audio, the radio will continue operating with those settings until: <br>
    <ul>
      <li>The ESP32 writes new values to the registers, or</li>
      <li>The RDA5807M loses power, causing its registers to reset to their default state.</li>
    </ul>
    
</p>

<h3> What rx.setup() actually does: </h3> 
<p> 
  According to the RDA5807M datasheet:
  <img src="img1.jpg">
  <i> 
    This image implies that the default state of the radio module, before using     it,in register 0x02, bit 14 = 0 -> DMUTE = 0, which means that the chip is       muted from the start
  </i>
  According to the source: PU2CLR RDA5807 Arduino Library

<pre>
  <code> 
  void RDA5807::setup(uint8_t clock_frequency, uint8_t oscillator_type, uint8_t rlck_no_calibrate)
    {
    this->oscillatorType = oscillator_type;
    this->clockFrequency = clock_frequency;
    this->rlckNoCalibrate = rlck_no_calibrate;
    
    Wire.begin();
    delay(10);
    powerUp();
    delay(this->maxDelayAftarCrystalOn);
  }
  </code>
</pre>
    
<p>
  When you call <code>rx.setup()</code>, it does three things in order: it calls:
  <ol>
    <li> <code>Wire.begin()</code>: initializes the I2C communication line between the ESP32 and the RDA5807M chip</li>
    <li> <code>delay(10)</code>: it waits 10 milliseconds for everything to stabilize</li>
    <li> <code>powerUp()</code>: which actually writes the initial configuration values into the chip's registers.</li><br>
   
          void RDA5807::powerUp()
          {
              reg02->raw = 0;
              reg02->refined.NEW_METHOD = 0;
              reg02->refined.RDS_EN = 0; // RDS disable
              reg02->refined.CLK_MODE = this->clockFrequency;
              reg02->refined.RCLK_DIRECT_IN = this->oscillatorType;
              reg02->refined.NON_CALIBRATE = this->rlckNoCalibrate;
              reg02->refined.MONO = 1;  // Force mono
              reg02->refined.DMUTE = 1; // Normal operation
              reg02->refined.DHIZ = 1;  // Normal operation
              reg02->refined.ENABLE = 1;
              reg02->refined.BASS = 1;
              reg02->refined.SEEK = 0;
              setRegister(REG02, reg02->raw);
              reg05->raw = 0x00;
              reg05->refined.INT_MODE = 0;
              reg05->refined.LNA_PORT_SEL = 2;
              reg05->refined.LNA_ICSEL_BIT = 0;
              reg05->refined.SEEKTH = 8; // 0b1000
              reg05->refined.VOLUME = 0;
              setRegister(REG05, reg05->raw);
          }
    


  <ul>
    <li>DMUTE = 1 -> unmutes the chip</li> 
    <li>DHIZ = 1 -> enables audio output</li>
    <li>VOLUME = 0 -> sets the volume to the minimum. <b> In this case, volume(0) does not equal real silence; by setting the volume to 0, we can still hear a little bit of sound.</b></li>
  </ul>
  </ol>

   <i> Overall, <code>rx.setup()</code>  initializes the I2C communication line between the ESP32 and the RDA5807M chip, unmutes the chip, and sets the volume to 0.</i>
</p>
