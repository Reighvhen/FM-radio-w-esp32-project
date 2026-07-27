# 📻FM Radio Project — ESP32 + RDA5807M + PAM8403

An ongoing FM radio project built with RDA5807M, ESP32, and PAM8403, programmed in Arduino IDE. Features free-frequency tuning, hardware volume control, and LED indicators. This repository serves as a progress log documenting different versions, problems encountered, and lessons learned.

The latest version features free-frequency tuning of FM radio stations controlled by potentiometers, a hardware volume control, and LED power indicators. Used an Analog Discovery 2 oscilloscope and FFT analysis to study audio waveforms and identify interference sources.

---

## About Me

Hi! I'm **Reigh Vhen Dimailig**, an Electrical Engineering student with interests in embedded systems, microcontrollers, and hands-on projects. Outside of engineering, I enjoy drawing and building fun things.

📫 Connect with me on [LinkedIn](https://www.linkedin.com/in/reigh-vhen)

---

## Components
- Freenove ESP32 WROOM
- RDA5807M FM Receiver Module
- PAM8403 Amplifier Module
- 10K Potentiometers (x2)
- RGB LED
- Push buttons (x3)
- Resistors (1kΩ, and 220Ω)
- FM Antenna
- Breadboard and jumper wires
- Analog Discovery 2 (for signal analysis)

---

## Version History

**V1** — Basic radio with hardcoded frequency, plays on power up

**V2** — Added on/off button to V1

**V3.1** — Added 2 volume buttons and red/green LEDs for power indication

**V3.2** — Simplified to 1 LED and potentiometer for volume control (software volume control via RDA5807M, max volume level 4)

**V3.3** — Improved volume control by controlling how much signal the PAM8403 amplifier receives, allowing true silence at minimum knob position

**V4** — Free frequency tuning across the FM band using a potentiometer mapped to 87.5–108 MHz

---

## Current Features
- Free frequency tuning across Edmonton FM band
- Hardware analog volume control with true silence at minimum
- Single RGB LED power indicator
- On/off button with debounce
- Signal analysis using an AD2 (Analog Discovery 2) oscilloscope and FFT

---

## Next Steps
- Further improve free tuning stability and efficiency
- Add LCD screen to display frequency, station name, song info (via RDS)
- Develop PCB version
- Learn 3D printing for custom radio enclosure

---

## Acknowledgements
- [PU2CLR RDA5807 Arduino Library](https://github.com/pu2clr/RDA5807) by Ricardo Lima Caratti — excellent documentation and detailed library

---

## References
- RDA5807M Datasheet
- Analog Discovery 2 / Waveforms software 
