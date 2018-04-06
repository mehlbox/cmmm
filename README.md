# CMMM
ceiling mounted motorized microphone

This Software controls a set of flying microphone (in my case one ORTF set) to move it around in certain directions.

There are one master unit and two slave units. Master and slave are connectet via I2C. Each slave unit controlls two stepper motor (4 stepper motor in total).
The coordinates you can chose are height and depth from stage edge. Input your desired coordinates on the web interface and the software will calculate the length of the lines and send it to the slave units.
- connect devices via I2C
- use the P82B715 I2C Bus Extender to achieve long wiring
- control settings via the web interface on your smartphone

## Hardware

Master unit:
- RaspberryPi

Slave unit: (2x)
- Arduino pro mini (ATmega328P)
- CNC Shield V4
- A4988 or compatible stepper motor driver (2x each, total 4x)
- Stepper motor with gear (2x each, total 4x)

AccelStepper library for Arduino is being used for motor control
http://www.airspayce.com/mikem/arduino/AccelStepper/