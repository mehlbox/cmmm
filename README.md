# CMMM
ceiling mounted moving microphone

This Arduino based project controls one set of microphone (in my case one ORTF set) to move it around just like a spider cam.

- using https://github.com/Jomelo/LCDMenuLib for display control
- using http://www.airspayce.com/mikem/arduino/AccelStepper/index.html for motor control

There are one master unit and two slave units where the master is equipped with a display and keys for input connectet via i2c to both slave units. Each slave unit controlls two stepper motor.
- Use 1k pullup resisrtor for i2c bus to achieve long wiring. I get about 50m bus lengh with 1k pull up on a simple telephon wire.

# Version 0.5
First released version. It is usable exept little bugs inside admin interface.
