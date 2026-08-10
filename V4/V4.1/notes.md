<h1 align="center">ESP32 FM Radio Project — Version 4</h1>

<p>
  This version adds <b>free frequency tuning</b> across the FM band using 
  a second potentiometer connected to GPIO 32. Instead of being locked to 
  a single hardcoded station, the user can now turn a knob to tune to any 
  FM station available in the area — just like a classic analog radio dial.
</p>

<p>
  <b>The Plan:</b><br>
  Connect a second 10K potentiometer as a voltage divider — one outer pin 
  to 3.3V, the other to GND, and the middle pin (wiper) to GPIO 32. 
  Use <code>analogRead(32)</code> to read the knob position and 
  <code>map()</code> to convert it to a frequency between 8750 and 10800 
  (87.5 MHz to 108.0 MHz — the standard North American FM band). 
  Call <code>rx.setFrequency()</code> only when the knob has moved 
  significantly, to avoid constant retuning from electrical noise.
</p>

<h2>What I Learned:</h2>

<h3>FM Frequency Range</h3>
<p>
  The FM band in North America spans from <b>87.5 MHz to 108.0 MHz</b>. 
  In the library's integer format, frequencies are represented as tenths 
  of MHz — so 87.5 MHz = 8750 and 108.0 MHz = 10800. This is the range 
  we map the potentiometer to, covering all possible FM stations not just 
  in Edmonton but anywhere in North America.
</p>

<h3>Why rx.setFrequency() is Inside if (isOn == true)</h3>
<p>
  Frequency tuning only makes sense when the radio is on. Placing 
  <code>rx.setFrequency()</code> inside the <code>if (isOn == true)</code> 
  block ensures the chip is only tuned when actively playing — not while 
  the radio is muted or powered off.
</p>

<h3>The Potentiometer Noise Problem and the Threshold Fix</h3>
<p>
  <code>analogRead()</code> is never perfectly stable — even with the knob 
  completely still, electrical noise causes the reading to fluctuate by a 
  few counts every loop cycle. If <code>rx.setFrequency()</code> was called 
  every loop cycle, the chip would constantly be retuning and never have 
  time to lock onto a station and play audio. This is why initially, 
  not even static could be heard.
  <br><br>
  The fix is a <b>threshold condition</b>:
  <pre><code>int newFreq = map(analogRead(32), 0, 4095, 8750, 10800);
if (abs(newFreq - lastFreq) > 11) 
{
    rx.setFrequency(newFreq);
    delay(50);
    lastFreq = newFreq;
}</code></pre>
  <code>abs(newFreq - lastFreq) > 11</code> means: only retune if the new 
  frequency differs from the last set frequency by more than 11 units. 
  Anything less than 11 is considered electrical noise and gets ignored. 
  Only intentional knob movements produce a change larger than 11, 
  triggering an actual retune.
  <br><br>
  <code>lastFreq</code> remembers the last frequency we actually set, 
  giving us something to compare against each cycle to tell the difference 
  between real knob movement and noise fluctuation.
</p>

<h3>Why delay(50) After rx.setFrequency()</h3>
<p>
  After calling <code>rx.setFrequency()</code>, the RDA5807M chip needs 
  a moment to actually finish tuning before the next loop cycle runs. 
  Without this delay, the chip gets interrupted before it can lock onto 
  the station — resulting in silence or choppy audio even when the 
  frequency is correct. The <code>delay(50)</code> gives the chip 50 
  milliseconds to complete the tuning process.
</p>

<h3>The Experience of Free Tuning</h3>
<p>
  Turning the knob sweeps through the FM band — static between stations, 
  then audio when a station is found. This is the classic analog radio 
  dial experience. However, because the RDA5807M is a digital chip, 
  it briefly interrupts audio every time <code>rx.setFrequency()</code> 
  is called while retuning — unlike a true analog tuner which sweeps 
  continuously with no interruptions.
</p>

<h3>Limitations of This Approach</h3>
<p>
  <ul>
    <li>
      <b>Hardcoded frequency range:</b> The map() function is set to 
      8750-10800 (North American FM band). This would need to be changed 
      for use in other regions — for example, Japan uses 76-90 MHz and 
      Europe uses 87.5-108 MHz with different channel spacing.
    </li>
    <li>
      <b>Difficult to land exactly on a station:</b> The potentiometer 
      sweeps through 2050 frequency steps across its full rotation range. 
      Even small knob movements jump through many frequencies, making it 
      hard to land precisely on a specific station. A multi-turn 
      potentiometer or rotary encoder would give finer control.
    </li>
    <li>
      <b>Potentiometer instability:</b> Even with the threshold fix, 
      occasional large noise spikes can still trigger an unwanted retune, 
      causing a brief audio dropout. This is a hardware limitation of 
      using a potentiometer for frequency control.
    </li>
  </ul>
</p>

<h3>Future Improvements Being
