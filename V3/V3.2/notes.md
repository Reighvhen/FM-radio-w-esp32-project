<h1 align="center">ESP32 FM Radio Project — Version 3.2</h1>

<p>
  This version replaces the two volume buttons from V3.1 with a 
  <b>potentiometer for volume control</b>, making it feel more like a 
  real commercial radio with a physical knob rather than buttons.
  The LED setup remains the same as V3.1 — red for off, green for on — 
  but switched from <code>digitalWrite()</code> to <code>analogWrite()</code>.
</p>

<p>
  <b>The Plan:</b><br>
  Connect a 10K potentiometer to GPIO 32 — an analog capable pin on the ESP32. 
  One outer pin goes to 3.3V, the other outer pin goes to GND, and the middle 
  pin (wiper) connects directly to GPIO 32 with no resistor needed. 
  Use <code>analogRead(32)</code> to read the knob position and 
  <code>map()</code> to convert that reading to a volume level between 0 and 4. 
  The volume limit is set to 4 instead of 15 because 15 is too loud for me.
</p>

<h2>What I Learned:</h2>

<h3>Why a Potentiometer Instead of Buttons</h3>
<p>
  Two volume buttons work, but they don't feel like a real radio. 
  Commercial radios use a physical knob that smoothly controls volume — 
  that's the experience this version was trying to achieve. 
  A potentiometer is essentially a variable resistor with a knob, 
  giving a continuous range of values rather than discrete button presses.
</p>

<h3>How the Potentiometer is Wired</h3>
<p>
  The potentiometer has three pins:
  <ul>
    <li><b>One outer pin</b> → 3.3V (VCC)</li>
    <li><b>Other outer pin</b> → GND</li>
    <li><b>Middle pin (wiper)</b> → GPIO 32 directly, no resistor needed</li>
  </ul>
  This makes the potentiometer act as a <b>voltage divider</b> between 
  3.3V and GND. As the knob turns, the wiper outputs a variable voltage 
  between 0V and 3.3V. The ESP32 reads this voltage on GPIO 32 using 
  <code>analogRead()</code>. No resistor is needed here because the GPIO 
  pin is only reading voltage — it is not in the audio signal path at all. 
  The potentiometer is purely communicating with the ESP32, not touching 
  the audio circuit.
</p>

<h3>analogRead() and ADC Resolution</h3>
<p>
  <code>analogRead(32)</code> returns a number between 0 and 4095 based 
  on the knob position:
  <ul>
    <li>Knob fully down → 0V → returns <b>0</b></li>
    <li>Knob fully up → 3.3V → returns <b>4095</b></li>
    <li>Knob in the middle → ~1.65V → returns <b>~2047</b></li>
  </ul>
  By default, the ESP32 ADC operates at <b>12-bit resolution</b>, meaning 
  it produces values from 0 to 4095 for the input voltage range. 
  This comes from 2¹² = 4096 possible values (0 to 4095).
  <br><br>
  Note: only specific GPIO pins on the ESP32 support analog input — 
  GPIO 32, 33, 34, 35, 36, and 39. Regular digital pins cannot use 
  <code>analogRead()</code>.
</p>

<h3>map() Function</h3>
<p>
  <code>map()</code> converts a value from one range to another proportionally:
  <pre><code>int vol = map(analogRead(32), 0, 4095, 0, 4);</code></pre>
  This takes whatever <code>analogRead(32)</code> returns (0-4095) and 
  scales it proportionally to the volume range (0-4). Knob at 75% position 
  → volume at 75% of maximum. The math is:
  <pre><code>output = (input - fromLow) × (toHigh - toLow) / (fromHigh - fromLow) + toLow</code></pre>
  Volume is limited to 4 instead of 15 in this version because 15 is just too loud.
</p>

<h3>analogWrite() vs digitalWrite()</h3>
<p>
  This version switched from <code>digitalWrite()</code> to 
  <code>analogWrite()</code> for the LEDs. The difference:
  <ul>
    <li><code>digitalWrite()</code> → only HIGH or LOW, fully on or fully off</li>
    <li><code>analogWrite()</code> → values from 0 to 255 using PWM, 
    allowing brightness control</li>
  </ul>
  In practice, using <code>analogWrite(255)</code> and <code>analogWrite(0)</code> 
  produced no visible difference from <code>digitalWrite(HIGH)</code> and 
  <code>digitalWrite(LOW)</code> — the difference only matters when using 
  in-between values like 128 for half brightness.
</p>

<h3>Limitations of This Approach — Why V3.3 Was Needed</h3>
<p>
  This software volume control approach had two major problems:
  <ol>
    <li>
      <b>Volume jumps were noticeable and not smooth.</b> The RDA5807M only 
      has 16 discrete volume steps (0-15). Even limiting to 0-4, the 
      transition between each step was abrupt and clearly audible — 
      not the smooth experience of a real radio knob.
    </li>
    <li>
      <b>No true silence from the potentiometer.</b> <code>rx.setVolume(0)</code> 
      does not produce real silence — the chip still outputs a small signal 
      that the PAM8403 amplifies to an audible level. The only way to achieve 
      true silence is <code>rx.setMute(true)</code>, which cannot be mapped 
      to a potentiometer value. The only way to get true silence was pressing 
      the power button — not something that felt natural for volume control.
    </li>
  </ol>
  These limitations led directly to the hardware approach in V3.3.
</p>
