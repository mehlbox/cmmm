// Slave

// segmentation of EEPROM bytes. One slot are 4 bytes together.:
// Slot 0:  bytes are called directly
//   Byte 0:  direction motor 1
//   Byte 1:  direction motor 2
//   Byte 2:  not in use
//   Byte 3:  gearRatio of motors
// Slot 1:  current position motor 1
// Slot 2:  current position motor 2
// Slot 3:  motor speed

#include <Wire.h>
#include <EEPROM.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

byte action;
byte calledmotor;
byte state;
byte pause;
byte MY_ADDRESS;
byte gearRatio;
byte motorStepping;
long positions[2]; // array for coordinates
long voltage;

#define Unit_EN 8 // one for all

#define i2c_jumper 11

#define voltage_devider_pin    A6
#define voltage_devider_ratio  27777.77778  //  1000000/36

// Unit1
#define Unit1_DIR  2
#define Unit1_STEP 5

// Unit2
#define Unit2_DIR  3
#define Unit2_STEP 6

/// Unit3
#define Unit3_DIR  4
#define Unit3_STEP 7

#define LED_Pin   13

#define DEBUG false

AccelStepper motor1(AccelStepper::DRIVER, Unit1_STEP, Unit1_DIR); // front
AccelStepper motor2(AccelStepper::DRIVER, Unit2_STEP, Unit2_DIR); // back
MultiStepper motorPair;

void setup() {
  pinMode(Unit_EN, OUTPUT);

  pinMode(LED_Pin, OUTPUT);
  pinMode(voltage_devider_pin, INPUT); // reading voltage
  pinMode(i2c_jumper, INPUT); digitalWrite(i2c_jumper,HIGH); // jumper to change i2c address if secondary device
  //if (!digitalRead(i2c_jumper)) MY_ADDRESS = 11; else MY_ADDRESS = 12; // set i2c address
  MY_ADDRESS = 11; // ### development
  Wire.begin(MY_ADDRESS);
  TWAR = (MY_ADDRESS << 1) | 1;  // enable RX broadcasts
  //_SFR_BYTE(PORTC) &= ~_BV(4); // deavtivate pullup  ### development
  //_SFR_BYTE(PORTC) &= ~_BV(5); // deavtivate pullup  ### development
  Wire.onReceive(receiveEvent); // set interrupt function
  Wire.onRequest(requestEvent); // set interrupt function
  if (EEPROM.read(0) == 255) newHardware();
  motor1.setPinsInverted(EEPROM.read(0));
  motor2.setPinsInverted(EEPROM.read(1));
  motor1.setCurrentPosition(load(1));
  motor2.setCurrentPosition(load(2));
  motor1.setMaxSpeed(load(3));
  motor2.setMaxSpeed(load(3));
  motorPair.addStepper(motor1);
  motorPair.addStepper(motor2);
  if (DEBUG) Serial.begin(9600);

state = 3;
// 0 = stopped
// 1 = turn on motors
// 2 = running motors
// 2+x = motors at destination
// 3 = turn off motors



  if (DEBUG) {
    Serial.print("MaxSpeed: ");Serial.println(load(3));
    Serial.print("motor stepping: ");Serial.println(EEPROM.read(2));
    Serial.print("gear ratio: ");Serial.println(EEPROM.read(3));
  }

}

void loop() {


  if (state == 1) { // turn on motors
    if (DEBUG) Serial.println("state 1");
    digitalWrite(LED_Pin, LOW);
    digitalWrite(Unit_EN, LOW);   //Motor EIN
    state = 2;
    if (DEBUG) Serial.println("state 2");
  }

  if ( state == 2 ) { // running motors
      motorPair.run();
  }

  if (state == 2 && motor1.distanceToGo() == 0 && motor2.distanceToGo() == 0) { // motors at destination
      state = 3;
      if (DEBUG) Serial.println("state 3");
  }

 //static unsigned long powerMillis;
  /*if (powerMillis < millis() && state == 2) { // disabled step count if voltage of motor dropped
    powerMillis = millis() + 100; // check 10 times per second
    voltage = analogRead(voltage_devider_pin) * voltage_devider_ratio;
    if (voltage < 8000000) { state = 3; if (DEBUG) Serial.println("power < 8V !!"); } //  about 8 volt
  }*/

  if (state == 3) { // turn off motors
      digitalWrite(LED_Pin, HIGH);
      digitalWrite(Unit_EN, HIGH);  // motors off
      if (motor1.currentPosition() != load(1)) save(motor1.currentPosition(), 1); // save current position in EEPROM
      if (motor2.currentPosition() != load(2)) save(motor2.currentPosition(), 2); // save current position in EEPROM
      state = 0;
      if (DEBUG) Serial.println("state 0");
  }
}

void receiveEvent(int howMany) { // receive data
  unsigned long data;
  action = Wire.read();                        //i2c 1 byte  //action --> case
  if (DEBUG && action != 14) { Serial.print("action: ");Serial.print(action); Serial.print("   state: ");Serial.println(state); }
  switch (action) {

    case 0: // stop motors
      state = 3;
      break;

    case 1: // move relativ #not tested
      calledmotor = Wire.read();
      data = recLong();                       //i2c 4 bytes
      if (calledmotor == 1) motor1.move(data);
      if (calledmotor == 2) motor2.move(data);
      if (state == 0) state = 1;
      break;

    case 2: // move absolute #not tested
      calledmotor = Wire.read();
      data = recLong();                         //i2c 4 bytes
      if (calledmotor == 1) motor1.moveTo(data);
      if (calledmotor == 2) motor2.moveTo(data);
      if (state == 0) state = 1;
      break;

    case 3: // destination coordinates ###
      positions[0] = recLong();
      positions[1] = recLong();
      if (state == 0) state = 1;
      motorPair.moveTo(positions);
      //if (DEBUG) { Serial.print("destination coordinates: ");Serial.print(positions[0]);Serial.print(" ");Serial.println(positions[1]); }
      break;

    case 5: // set direction ####
      calledmotor = Wire.read();
      if (calledmotor == 1) {
        EEPROM.write(0,!EEPROM.read(0));
        motor1.setPinsInverted(EEPROM.read(0));
      }
      if (calledmotor == 2) {
        EEPROM.write(1,!EEPROM.read(1));
        motor2.setPinsInverted(EEPROM.read(1));
      }
      break;

    case 6: // motorstatus ####
      calledmotor = Wire.read();
      break;

    case 7:  // correct error ####
      calledmotor = Wire.read();
      data = recLong();                       //i2c 4 bytes
      if (calledmotor == 1) motor1.setCurrentPosition(motor1.currentPosition() + data);
      if (calledmotor == 2) motor2.setCurrentPosition(motor2.currentPosition() + data);
      if (state == 0) state = 1;
      break;

    case 8: // set motor stepping
      motorStepping = Wire.read();
      if (motorStepping != EEPROM.read(2)){
        motor1.setCurrentPosition(motor1.currentPosition() * motorStepping / EEPROM.read(2));
        motor2.setCurrentPosition(motor2.currentPosition() * motorStepping / EEPROM.read(2));
        EEPROM.write(2,motorStepping);
      }
      break;

    case 11: // set gearRatio ####
      gearRatio = Wire.read();
      if (gearRatio != EEPROM.read(3)){
        motor1.setCurrentPosition(motor1.currentPosition() * gearRatio / EEPROM.read(3) );
        motor2.setCurrentPosition(motor2.currentPosition() * gearRatio / EEPROM.read(3) );
        EEPROM.write(3,gearRatio);
      }
      break;

    case 9: // set max speed ####
      long motorSpeed;
      Wire.read(); // dump one byte
      motorSpeed = recLong();
      if (DEBUG) Serial.println(motorSpeed);
      if (motorSpeed != load(3)){
        motor1.setMaxSpeed(motorSpeed);
        motor2.setMaxSpeed(motorSpeed);
        save(motorSpeed,3);
      }
      break;

  }
while(Wire.available()) Wire.read(); // clean up left over bytes from I2C buffer
}

void requestEvent() { // pickup data
    long data;
    byte mem[8];
    switch(action) {
    if (DEBUG) { Serial.print("requested: ");Serial.print(action); }
    case 0: // Stop  #not tested
      if (calledmotor == 1) { motor1.stop(); sendLong(motor1.distanceToGo());};
      if (calledmotor == 2) { motor2.stop(); sendLong(motor2.distanceToGo());};
      break;

    case 6: // status: rotating or not? ####
      if (calledmotor == 1) data = motor1.distanceToGo();
      if (calledmotor == 2) data = motor2.distanceToGo();
      if (data  > 0) Wire.write(1);
      if (data  < 0) Wire.write(2);
      if (data == 0) Wire.write(3);
     break;

    case 10: // polling for current position #not tested
      if (calledmotor == 1) data = motor1.currentPosition();
      if (calledmotor == 2) data = motor2.currentPosition();
      sendLong(data);
      break;

    case 12: // polling voltage ####
      voltage = analogRead(voltage_devider_pin) * voltage_devider_ratio;
      sendLong(voltage);
      break;

    case  4: // polling for current position ####
    case 14: // polling for current position ####
    //if (DEBUG) { Serial.print("polling current steps: ");Serial.print(motor1.currentPosition());Serial.print(" ");Serial.println(motor2.currentPosition()); }
      data = motor1.currentPosition();
      mem[0]=(data      ) & 0xFF;
      mem[1]=(data >>  8) & 0xFF;
      mem[2]=(data >> 16) & 0xFF;
      mem[3]=(data >> 24) & 0xFF;
      data = motor2.currentPosition();
      mem[4]=(data      ) & 0xFF;
      mem[5]=(data >>  8) & 0xFF;
      mem[6]=(data >> 16) & 0xFF;
      mem[7]=(data >> 24) & 0xFF;
      Wire.write(mem,8);
      break;

      case 16: // test
        Wire.write(99);
        break;

  }
}

int recInt() { // receive 16 bit variable via i2c
  int mem[2];
  mem[0] = Wire.read();
  mem[1] = Wire.read(); mem[1] =  mem[1] << 8;
  return (mem[0] |= mem[1]);
}

long recLong() { // receive 32 bit variable via i2c
  long mem[4];
  mem[0] = Wire.read();
  mem[1] = Wire.read(); mem[1] =  mem[1] << 8;
  mem[2] = Wire.read(); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
  mem[3] = Wire.read(); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
  return (mem[0] |= mem[1] |= mem[2] |= mem[3]);
}

void sendLong(long data) { // send 32 bit variable via i2c
    byte mem[4];
    mem[0]=(data      ) & 0xFF;
    mem[1]=(data >>  8) & 0xFF;
    mem[2]=(data >> 16) & 0xFF;
    mem[3]=(data >> 24) & 0xFF;
    Wire.write(mem,4);
}

void save(long data, int slot) { // save position in EEPROM
    byte mem[4];
    slot = (slot * 4); // translate slot in EEPROM address
    mem[0]=(data      ) & 0xFF;
    mem[1]=(data >>  8) & 0xFF;
    mem[2]=(data >> 16) & 0xFF;
    mem[3]=(data >> 24) & 0xFF;
    for (int n=0; n<=3;n++) EEPROM.write(slot+n, mem[n]);
}

long load(int slot) {  // load position from EEPROM
  slot = (slot * 4);   // translate slot in EEPROM address
  long mem[4];
  mem[0] = EEPROM.read(slot);
  mem[1] = EEPROM.read(slot + 1); mem[1] =  mem[1] << 8;
  mem[2] = EEPROM.read(slot + 2); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
  mem[3] = EEPROM.read(slot + 3); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
  return (mem[0] |= mem[1] |= mem[2] |= mem[3]);
}

/*void stepping(int stepping, int ms1, int ms2, int ms3) { // Gangschaltung   // deprecated, set by jumper
  switch(stepping) {

  case 1:
  digitalWrite(ms1, LOW);
  digitalWrite(ms2, LOW);
  digitalWrite(ms3, LOW);
  break;

  case 2:
  digitalWrite(ms1, HIGH);
  digitalWrite(ms2, LOW);
  digitalWrite(ms3, LOW);
  break;

  case 4:
  digitalWrite(ms1, LOW);
  digitalWrite(ms2, HIGH);
  digitalWrite(ms3, LOW);
  break;

  case 8:
  digitalWrite(ms1, HIGH);
  digitalWrite(ms2, HIGH);
  digitalWrite(ms3, LOW);
  break;

  case 16:
  digitalWrite(ms1, HIGH);
  digitalWrite(ms2, HIGH);
  digitalWrite(ms3, HIGH);
  break;

  default:
  digitalWrite(ms1, LOW);
  digitalWrite(ms2, LOW);
  digitalWrite(ms3, LOW);
  break;
  }
}*/

void newHardware() {
  EEPROM.write(0,0);
  EEPROM.write(1,0);
  EEPROM.write(3,27);
  save(352049,1);
  save(145667,2);
  save(1000,3);
}
