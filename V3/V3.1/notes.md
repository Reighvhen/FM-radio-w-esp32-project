<h1 align="center">ESP32 FM Radio Project — Version 3.1</h1>

<p>
  This is the third version of the project, part one. 
  Building on V2, the goals for this version were:
  <ul>
    <li>Add two volume buttons — one to increase volume, one to decrease</li>
    <li>Add two LEDs — red to indicate the radio is off, green to indicate it is on</li>
  </ul>
</p>

<p>
  <b>The Plan:</b><br>
  Volume up button on GPIO 4 increases volume by 1 up to a maximum of 15. 
  Volume down button on GPIO 5 decreases volume by 1. The volume range is 
  <b>[real silence, 0, 1, 2 ... 15]</b> — 17 different states in total. 
  Volume = 0 is NOT real silence. Real silence is only achieved by 
  <code>rx.setMute(true)</code> which cuts the audio signal at the chip level. 
  So pressing volume down when already at 0 triggers <code>rx.setMute(true)</code> 
  instead of going lower.
</p>

<h2>What I Learned:</h2>

<h3>Why isOn Instead of Just Using rx.getMute()</h3>
<p>
  A separate <code>bool isOn</code> variable was introduced to track whether 
  the radio is on or off. While <code>rx.getMute()</code> could technically 
  give us the same information, having our own variable is more predictable — 
  we know exactly what state it starts in (<code>false</code> = off) and we 
  control when and how it changes. It makes the code clearer and more reliable.
</p>

<h3>LED Control — digitalWrite() and ledState</h3>
<p>
  <code>ledState</code> is initialized globally as LOW, meaning LEDs start off. 
  When turning the radio on:
  <pre><code>digitalWrite(green, !ledState);  // !LOW = HIGH → green turns ON
digitalWrite(red, ledState);     // LOW → red turns OFF</code></pre>
  When turning the radio off:
  <pre><code>digitalWrite(red, !ledState);    // !LOW = HIGH → red turns ON
digitalWrite(green, ledState);   // LOW → green turns OFF</code></pre>
  One LED turns on while the other turns off at the same time.
</p>

<h3>Why lastUp and lastDown Must Be Updated Outside the isOn Block</h3>
<p>
  <code>lastUp</code> and <code>lastDown</code> are updated at the very bottom 
  of <code>loop()</code>, outside the <code>if (isOn == true)</code> block. 
  If they were inside that block, they would only update while the radio is on. 
  When the radio is off, they would freeze at their last value. The next time 
  the radio turns on, the very first volume button press would be ignored — 
  because <code>lastUp</code> is still stuck at LOW from before, so the condition 
  <code>lastUp == HIGH && nowUp == LOW</code> never triggers. After that first 
  missed press, everything works correctly. Moving the updates outside the block 
  ensures they always reflect the true current button state regardless of whether 
  the radio is on or off.
</p>

<h3>Button Detection — Where It Finally Clicked</h3>
<p>
  This version is where button detection logic truly made sense. 
  The key insight is that every loop cycle has four possible button state transitions:
  <ul>
    <li>HIGH → HIGH: no change, button not pressed</li>
    <li>LOW → LOW: no change, button held down</li>
    <li><b>HIGH → LOW: button being pressed ← this is what we want</b></li>
    <li>LOW → HIGH: button being released</li>
  </ul>
  Checking <code>lastState != nowState</code> alone catches both pressing AND 
  releasing. The additional condition <code>lastState == HIGH && nowState == LOW</code> 
  specifically isolates the pressing moment only — because people expect things 
  to happen when they push a button down, not when they let go.
  <br><br>
  The variable that stores the previous state (<code>lastUp</code>, 
  <code>lastDown</code>, <code>lastMuteState</code>) must be a 
  <b>global variable</b> — declared outside both <code>setup()</code> and 
  <code>loop()</code>. If declared inside <code>loop()</code>, it gets 
  recreated fresh every cycle and loses its previous value, making it 
  impossible to detect state changes.
</p>

<h3>Volume Logic in Plain English</h3>
<p>
<pre>
When the program starts:
  → Radio is off, muted, volume = 0, red LED on

Every loop cycle:

  Check power button:
    → Did it change from last time?
      → Was it pressed (HIGH to LOW)?
        → If radio is OFF: unmute, set volume 0, green LED on, red LED off
        → If radio is ON: mute, set volume 0, red LED on, green LED off
        → Wait 50ms for debounce
      → Update lastMuteState

  Read volume up and down button states

  If radio is ON:
    Check volume up button:
      → Did it change from last time?
        → Was it pressed?
          → If currently muted (from volume going to real silence): unmute, set volume 0
          → If volume is already 15: stay at 15
          → Otherwise: increase volume by 1
    Check volume down button:
      → Did it change from last time?
        → Was it pressed?
          → If volume is already 0: trigger real silence (setMute true)
          → Otherwise: decrease volume by 1

  Update lastUp and lastDown (always, even when radio is off)
</pre>
</p>
