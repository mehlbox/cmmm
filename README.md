# CMMM
ceiling mounted motorized microphone

This Arduino based project controls one set of microphone (in my case one ORTF set) to move it around just like a spider cam.

- using https://github.com/Jomelo/LCDMenuLib for display control
- using http://www.airspayce.com/mikem/arduino/AccelStepper/index.html for motor control

There are one master unit and two slave units where the master is equipped with a display and keys for input. Master and slave are connectet via i2c. Each slave unit controlls two stepper motor (4 stepper motor in total).
The coordinates are height and depth from stage edge. Input your desired coordinates on the master unit and it will calculate the length of the lines and send it to the slave units.

- Use 1k pullup resisrtor for i2c bus to achieve long wiring. I get about 50m bus lengh with 1k pull up on a simple telephon wire.

- Use master branch for a working version. It is usable exept some cosmetic bugs on control panel.

## Hardware
Master unit:
- Arduino ATmega328P or clone
- 20X4 LCD display module with HD44780 controller
- Buttons: left, right, up, down, esc, enter

Slave unit: (2x)
- Arduino ATmega328P or clone
- A4988 or compatible stepper motor driver (2x)
- Stepper motor with gear (2x)
