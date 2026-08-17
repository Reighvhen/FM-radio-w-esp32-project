<h1 align="center">ESP32 FM Radio Project — Version 3.3</h1>

<p>
  This version replaces the software volume control from V3.2 with a 
  <b>hardware analog volume control</b> — placing the potentiometer physically 
  in the audio signal path between the RDA5807M and the PAM8403 amplifier. 
  This solves both major problems from V3.2: the noticeable volume jumps 
  and the inability to achieve true silence from the potentiometer alone.
</p>

<p>
  <b>The Plan:</b><br>
  Instead of using the potentiometer to send a voltage to the ESP32 for 
  software volume control, place it directly in the audio signal path. 
  The potentiometer now controls how much of the audio signal from the 
  RDA5807M actually reaches the PAM8403 amplifier. Set 
  <code>rx.setVolume(4)</code> as a fixed maximum in code and let the 
  potentiometer handle all volume control from there.
</p>

<h2>What I Learned:</h2>

<h3>Hardware Volume Control — How It Works</h3>
<p>
  The complete audio signal path in this version:
  <pre>
RDA5807M LOUT → [1kΩ resistor] ↘
                                 → [potentiometer outer pin]
RDA5807M ROUT → [1kΩ resistor] ↗

[potentiometer other outer pin] → GND
[potentiometer middle pin/wiper] → PAM8403 L-IN and R-IN
  </pre>
  The potentiometer acts as a <b>voltage divider on the audio signal itself </b> — not on a power supply voltage like in V3.2. The audio signal enters one 
  outer pin, GND is on the other outer pin, and the wiper taps off a variable 
  amount of signal depending on knob position:
  <ul>
    <li><b>Knob at minimum</b> → wiper near GND → almost no signal reaches PAM8403 → <b>true silence</b></li>
    <li><b>Knob at maximum</b> → wiper at full signal → PAM8403 receives full signal → loudest</li>
    <li><b>Any position in between</b> → smooth continuous analog transition</li>
  </ul>
  This is fundamentally different from V3.2 — we are not telling the RDA5807M 
  chip to change its volume digitally. We are physically controlling how much 
  audio signal the amplifier receives as input. <i> (The audio signal is a voltage signal. When the RDA5807M outputs audio on LOUT and ROUT, it's outputting a varying voltage that represents the audio waveform — the voltage goes up and down at audio frequencies, and those variations are what carries the sound information.) </i>
</p>

<h3>Why rx.setVolume(4) is Hardcoded</h3>
<p>
  Since the potentiometer now handles all volume control in hardware, the 
  software volume just needs to be set to a fixed maximum and left alone. 
  <code>rx.setVolume(4)</code> sets the RDA5807M to output at a consistent 
  level, and the potentiometer controls how much of that output actually 
  reaches the amplifier. GPIO 32 is no longer needed — we are no longer 
  reading the potentiometer with <code>analogRead()</code>.
</p>

<h3>Why Two 1kΩ Resistors</h3>
<p>
  LOUT and ROUT are two separate signal sources from the RDA5807M. 
  Connecting them directly together without resistors would cause interference 
  — if one channel is at a slightly different voltage than the other at any 
  moment, current flows between them and distorts both signals. 
  One 1kΩ resistor on each channel isolates them before they merge at the 
  potentiometer input, preventing this interference. Two resistors are needed 
  — one per channel — not just one.
</p>

<h3>True Silence Achieved</h3>
<p>
  Unlike V3.2 where <code>rx.setVolume(0)</code> could not produce true silence, 
  this version achieves true silence purely from the potentiometer. When the 
  knob is turned all the way down, the wiper connects to GND — no audio signal 
  reaches the PAM8403 at all. There is nothing for the amplifier to amplify, 
  so the output is completely silent. This is the hardware equivalent of 
  <code>rx.setMute(true)</code> — but controlled physically by the knob 
  rather than by code.
</p>

<h3>Stereo vs Mono</h3>
<p>
  <b>Stereo</b> means two separate audio channels — left and right — with 
  different sounds creating a sense of width and positioning. Think of 
  headphones where the left ear hears slightly different sounds from the right, 
  making music feel wide and immersive.
  <br><br>
  <b>Mono</b> combines everything into one single channel. No left or right 
  positioning — just one sound playing through everything.
  <br><br>
  In this project, LOUT and ROUT from the RDA5807M are two separate stereo 
  channels. Merging them into one potentiometer converts the signal to mono. 
  However, since the project uses only one speaker, it was already effectively 
  mono regardless — one speaker can only produce one audio signal at a time. 
  So merging the channels made no practical difference to the listening experience.
</p>

<h3>Dual Gang vs Single Gang Potentiometer</h3>
<p>
  A <b>dual gang potentiometer</b> contains two separate potentiometers 
  mechanically linked to one knob — one for the left channel and one for 
  the right channel. This allows true stereo volume control where both 
  channels are adjusted simultaneously but kept separate all the way to 
  the speakers.
  <br><br>
  A <b>single gang potentiometer</b> (what we have) has only one control. 
  To use it for both channels, the left and right signals must be merged 
  before the potentiometer — which converts stereo to mono as explained above.
  <br><br>
  For a single speaker setup like this project, a single gang potentiometer 
  is perfectly fine. A dual gang would only matter if two separate speakers 
  were used for true stereo output.
</p>

<h3>Comparison: V3.2 vs V3.3 Volume Control</h3>
<p>
  <table border="1" cellpadding="8">
    <tr>
      <th></th>
      <th>V3.2 — Software Control</th>
      <th>V3.3 — Hardware Control</th>
    </tr>
    <tr>
      <td>What is controlled</td>
      <td>RDA5807M internal volume register</td>
      <td>Amount of signal reaching PAM8403</td>
    </tr>
    <tr>
      <td>Volume steps</td>
      <td>Discrete steps (0-4), noticeable jumps</td>
      <td>Continuous analog, perfectly smooth</td>
    </tr>
    <tr>
      <td>True silence</td>
      <td>Not possible from potentiometer alone</td>
      <td>Yes, knob fully down = no signal</td>
    </tr>
    <tr>
      <td>GPIO 32 used</td>
      <td>Yes, analogRead() to read knob</td>
      <td>No, potentiometer not connected to ESP32</td>
    </tr>
    <tr>
      <td>Resistors needed</td>
      <td>No</td>
      <td>Two 1kΩ resistors to isolate L and R channels</td>
    </tr>
  </table>
</p>
