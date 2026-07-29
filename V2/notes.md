<h1 align="center">ESP32 FM Radio Project — Version 2</h1>

<p>
  This is the second version of the project. The goal was to 
  <b>add a single push button that acts as a power on/off toggle for the radio.</b>
  When pressed, the button mutes or unmutes the RDA5807M chip — 
  effectively turning the radio on and off.
</p>

<p>
  <b>The Plan:</b><br>
  Connect one button to GPIO 18. When pressed, toggle the mute state of the 
  RDA5807M using <code>rx.setMute()</code>. Simple in concept, but required 
  understanding several important hardware and software ideas first — 
  floating pins, pull-up resistors, button detection logic, and debouncing.
</p>

<p>
  <b>The Result:</b><br>
  The button successfully toggles the radio on and off. Moreover, using my  <b>Analog Discovery 2 oscilloscope</b> and <b>Waveforms software</b>, I was able to observe the actual audio signal coming out of the RDA5807M's LOUT pin for the first time.
</p>

<h2>What I Learned:</h2>

<h3>Volume 0 ≠ Real Silence</h3>
<p>
  From the previous version, I discovered that calling <code>rx.setVolume(0)</code> does <b>not</b> produce true silence. Volume 0 does not equal zero signal output. The fact that we could still hear something with volume set to 0 means the RDA5807M chip is still outputting a small signal — which the PAM8403 amplifier then amplifies to an audible level.
  <br><br>
<b>True silence can only be achieved by calling <code>rx.setMute(true)</code></b>, which sets the DMUTE bit to 0 in register 0x02, completely cutting the audio output at the chip level rather than just reducing its volume.
</p>

<h3>Floating Pins and Pull-up Resistors</h3>
<p>
  When a button is connected between a GPIO pin and GND, the pin has two possible states:
  <ul>
    <li><b>Button pressed</b> → pin connects to GND → reads LOW (0V)</li>
    <li><b>Button not pressed</b> → pin connects to <i>nothing</i> → <b>floating</b></li>
  </ul>
  A floating pin has no defined voltage — it randomly reads HIGH or LOW due to 
  electrical noise, causing the button to appear to trigger by itself randomly. 
  This is fixed by enabling the ESP32's internal <b>pull-up resistor</b>:
  <pre><code>pinMode(BUTTON_PIN, INPUT_PULLUP);</code></pre>
  This connects the pin to 3.3V through an internal resistor when nothing else 
  is driving it. Now the states are clearly defined:
  <ul>
    <li><b>Button not pressed</b> → pin pulled to 3.3V → reads HIGH</li>
    <li><b>Button pressed</b> → pin connects to GND → reads LOW</li>
  </ul>
</p>

<h3>Button Detection Logic</h3>
<p>
  We don't just check if the button is LOW — that would trigger thousands of times per second while the button is held down. Instead, we detect the exact moment the button transitions from not pressed to pressed by comparing the current state to the previous state.
  <br><br>
  Every loop cycle has four possible transitions:
  <ul>
    <li>HIGH → HIGH: no change, button not pressed</li>
    <li>LOW → LOW: no change, button held down</li>
    <li><b>HIGH → LOW: button being pressed ← this is what we want</b></li>
    <li>LOW → HIGH: button being released</li>
  </ul>
  The condition below specifically catches only the pressing moment:
  <pre><code>if (lastButtonState == HIGH && buttonState == LOW) {
    // button was just pressed
}</code></pre>
  <code>lastButtonState</code> remembers the previous state so we can detect 
  the change. It must be declared as a <b>global variable</b> so it persists 
  between loop cycles — if declared inside loop(), it would reset every cycle 
  and lose its previous value.
</p>

<h3>Debouncing</h3>
<p>
  Physical button contacts bounce — they make and break contact multiple times in milliseconds when pressed. This causes the toggle to fire multiple times per press. An odd number of bounces leaves the radio in the opposite state from intended — for example, pressing to mute with 3 bounces ends up unmuted instead.
  <br><br>
Fixed by adding <code>delay(50)</code> after detecting a press, giving the contacts time to settle before checking again:
  <pre><code>if (lastButtonState == HIGH && buttonState == LOW) {
    rx.setMute(!currentMuteState);
    delay(50);
}
lastButtonState = buttonState;</code></pre>
</p>

<h3>setMute() vs powerDown()</h3>
<p>
  <ul>
    <li>
      <code>rx.setMute(true)</code> — sets the DMUTE bit to 0, silencing the audio output while keeping the chip fully powered and initialized. The chip stays ready to play — it's just silent.
    </li>
    <li>
      <code>rx.powerDown()</code> — sets the ENABLE bit to 0, powering the chip down completely. Requires full reinitialization via <code>rx.setup()</code> to turn back on, which also causes a loud glitch sound and requires <code>rx.setFrequency()</code> to be called again.
    </li>
  </ul>
  For a simple on/off button, <code>rx.setMute()</code> is the better choice —simpler, quieter, and no reinitialization needed.
</p>

<h3>Analog Discovery 2 — First Real Measurements</h3>
<p>
  Connected the AD2 scope probe to the LOUT pin of the RDA5807M to observe the audio signal for the first time.
  <br><br>
  <b>What the waveform showed:</b>
  <ul>
    <li>
      <b>Unmuted:</b> A complex waveform representing the actual audio — voltage 
      swings up and down reflecting the loudness and shape of the sound at each 
      moment. Louder and busier audio produces larger voltage swings.
    </li>
    <li>
      <b>Muted:</b> The waveform is flattened to a straight line at approximately 900mV. This is the <b>DC offset</b> of the RDA5807M output — the chip outputs a constant DC voltage even when muted. This is normal behavior.
    </li>
  </ul>

  <b>Mistake I made and Grounding lesson I learned:</b><br>
  The ground clip of the AD2 must connect to the same GND as the circuit. Voltage is always measured relative to a reference point — if the reference is wrong, every measurement is wrong. Initially connecting the ground clip to the AD2's own ground instead of the circuit ground caused measurements to show no change when muting — a completely misleading result.
</p>
