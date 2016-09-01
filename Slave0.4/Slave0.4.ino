// Slave

// Aufeilung im EEPROM. Ein Slot fasst 4 bytes zusammen.:
// Slot 0:  Direkt angesprochen
//   Byte 0:  Drehrichtung Motor 1
//   Byte 1:  Drehrichtung Motor 2
//   Byte 2:  Gang Motoren
//   Byte 3:  Ratio Motoren
// Slot 1:  Aktuelle Position Motor 1
// Slot 2:  Aktuelle Position Motor 2
// Slot 3:  Geschwindigkeit der Motoren

#include <Wire.h>
#include <EEPROM.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

byte action;
byte calledmotor;
byte state;
byte pause;
byte ratio;
byte MY_ADDRESS;
long positions[2]; // Array für die Koordinaten
long voltage;

#define Unit1_EN  10
#define Unit1_MS1 11
#define Unit1_MS2 12
#define Unit1_MS3 14
#define Unit1_STEP 4
#define Unit1_DIR  5

#define Unit2_EN   9
#define Unit2_MS1  8
#define Unit2_MS2  7
#define Unit2_MS3  6
#define Unit2_STEP 3
#define Unit2_DIR  2

#define LED_Pin   13

AccelStepper motor1(AccelStepper::DRIVER, Unit1_STEP, Unit1_DIR);
AccelStepper motor2(AccelStepper::DRIVER, Unit2_STEP, Unit2_DIR);
MultiStepper motoren;

void setup()
{
  pinMode(Unit1_EN, OUTPUT);
  pinMode(Unit1_MS1, OUTPUT);
  pinMode(Unit1_MS2, OUTPUT);
  pinMode(Unit1_MS3, OUTPUT);

  pinMode(Unit2_EN, OUTPUT);
  pinMode(Unit2_MS1, OUTPUT);
  pinMode(Unit2_MS2, OUTPUT);
  pinMode(Unit2_MS3, OUTPUT);

  pinMode(LED_Pin, OUTPUT);
  pinMode(A3, INPUT); // Spannung auslesen
  pinMode(16, INPUT); digitalWrite(16,HIGH); // Jumper für i2c Adresse
  if (!digitalRead(16)) MY_ADDRESS = 11; else MY_ADDRESS = 12; // i2c Adresse festlegen
  Wire.begin(MY_ADDRESS);
  TWAR = (MY_ADDRESS << 1) | 1;  // broadcasts Empfang aktivieren
  _SFR_BYTE(PORTC) &= ~_BV(4); // deaktiviere Pullup
  _SFR_BYTE(PORTC) &= ~_BV(5); // deaktiviere Pullup
  Wire.onReceive(receiveEvent); // interrupt Funktion zuweisen
  Wire.onRequest(requestEvent); // interrupt Funktion zuweisen
  if (EEPROM.read(0) == 255) newHardware();
  motor1.setPinsInverted(EEPROM.read(0));
  motor2.setPinsInverted(EEPROM.read(1));
  gear(EEPROM.read(2),Unit1_MS1,Unit1_MS2,Unit1_MS3);
  gear(EEPROM.read(2),Unit2_MS1,Unit2_MS2,Unit2_MS3);
  ratio = EEPROM.read(3);
  motor1.setCurrentPosition(load(1));
  motor2.setCurrentPosition(load(2));
  motor1.setMaxSpeed(load(3));
  motor2.setMaxSpeed(load(3));
  motoren.addStepper(motor1);
  motoren.addStepper(motor2);
  
state = 3;
// 0 = angehalten
// 1 = Motoren einschalten
// 2 = Motoren laufen
// 2+x = Motoren haben Nullpunkt erreicht
// 3 = Motoren aus
}

void loop() {
  static unsigned long powerMillis;
  
  if (state == 1) { // Motor EIN
    digitalWrite(LED_Pin, LOW);
    digitalWrite(Unit1_EN, LOW);   //Motor EIN
    digitalWrite(Unit2_EN, LOW);   //Motor EIN
    state = 2;
  } 
  
  if ( state == 2 ) { // Motoren laufen
      motoren.run();
  }
  
  if (state == 2 && motor1.distanceToGo() == 0 && motor2.distanceToGo() == 0) { // Nullpunkt erreicht
      state = 3;
  }

  if (powerMillis < millis() && state == 2) { // Verhindert dass bei Spannungsabfall die Schritte weiter gezählt werden. 
    powerMillis = millis() + 100; // Prüfung 10 in der Sekunden.
    voltage = analogRead(A3) * 27777.77778; //  1000000/36
    if (voltage < 8000000) state = 3; //  ca. 8 Volt
  }

  if (state == 3) { // Motoren aus
      digitalWrite(LED_Pin, HIGH);
      digitalWrite(Unit1_EN, HIGH);  //Motor AUS
      digitalWrite(Unit2_EN, HIGH);  //Motor AUS
      if (motor1.currentPosition() != load(1)) save(motor1.currentPosition(), 1); // Aktuelle Position in EEPROM speichern
      if (motor2.currentPosition() != load(2)) save(motor2.currentPosition(), 2); // Aktuelle Position in EEPROM speichern  
      state = 0;
  }
}

void receiveEvent(int howMany) { // Daten erhalten
  unsigned long data;
  action = Wire.read();                        //i2c 1 byte  //case
  switch (action) {
      
    case 0: // Motor Stop
      state = 3;
      break;   

    case 1: // Move relativ #not tested
      calledmotor = Wire.read();
      data = recLong();                       //i2c 4 bytes
      if (calledmotor == 1) motor1.move(data);
      if (calledmotor == 2) motor2.move(data);
      if (state == 0) state = 1;
      break;   

    case 2: // Move absolute #not tested
      calledmotor = Wire.read();
      data = recLong();                         //i2c 4 bytes
      if (calledmotor == 1) motor1.moveTo(data);
      if (calledmotor == 2) motor2.moveTo(data);
      if (state == 0) state = 1;
      break;
      
    case 3: // Zielwerte koordinaten ####
      positions[0] = recLong();
      positions[1] = recLong();
      if (state == 0) state = 1;  
      motoren.moveTo(positions);
      break;
            
    case 5: // Drehrichtung ändern ####
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

    case 6: // Motorstatus ####
      calledmotor = Wire.read();
      break;
      
    case 7:  // Abweichng korrigieren ####
      calledmotor = Wire.read();
      data = recLong();                       //i2c 4 bytes
      if (calledmotor == 1) motor1.setCurrentPosition(motor1.currentPosition() + data);
      if (calledmotor == 2) motor2.setCurrentPosition(motor2.currentPosition() + data);
      if (state == 0) state = 1;
      break;
      
    case 8: // Gang einstellen ####
      byte data_gear;
      data_gear = Wire.read();
      if (data_gear != EEPROM.read(2)){
        motor1.setCurrentPosition(motor1.currentPosition() * data_gear / EEPROM.read(2));
        motor2.setCurrentPosition(motor2.currentPosition() * data_gear / EEPROM.read(2));
        EEPROM.write(2,data_gear);
        gear(EEPROM.read(2),Unit1_MS1,Unit1_MS2,Unit1_MS3);
        gear(EEPROM.read(2),Unit2_MS1,Unit2_MS2,Unit2_MS3);
      }      
      break;
      
    case 11: // Ratio einstellen ####
      ratio = Wire.read();
      if (ratio != EEPROM.read(3)){
        motor1.setCurrentPosition(motor1.currentPosition() * ratio / EEPROM.read(3) );
        motor2.setCurrentPosition(motor2.currentPosition() * ratio / EEPROM.read(3) );
        EEPROM.write(3,ratio);
      }      
      break;
      
    case 9: // Geschwindigkeit einstellen ####
      unsigned long motorSpeed;
      Wire.read(); // Byte verwerfen
      motorSpeed = recLong();
      if (motorSpeed != load(3)){
        motor1.setMaxSpeed(motorSpeed);
        motor2.setMaxSpeed(motorSpeed);
        save(motorSpeed,3);
      }      
      break;
      
  }
while(Wire.available()) Wire.read(); // Übrig gebliebene Bytes aus I2C Puffer entfernen. Sollte bei fehlerfreien Programmierung nicht auftreten.
}

void requestEvent() { // Daten wurden angefordert

    long data;
    byte mem[8];
    switch(action) {
    
    case 0: // Stop  #not tested
      if (calledmotor == 1) { motor1.stop(); sendLong(motor1.distanceToGo());};
      if (calledmotor == 2) { motor2.stop(); sendLong(motor2.distanceToGo());};
      break;

    case 6: // Status dreht noch oder nicht? ####
      if (calledmotor == 1) data = motor1.distanceToGo();
      if (calledmotor == 2) data = motor2.distanceToGo();
      if (data  > 0) Wire.write(1);
      if (data  < 0) Wire.write(2);
      if (data == 0) Wire.write(3);
     break;

    case 10: // Abfrage aktuelle Position #not tested
      if (calledmotor == 1) data = motor1.currentPosition();
      if (calledmotor == 2) data = motor2.currentPosition();
      sendLong(data);
      break;
      
    case 12: // Spannung abfragen ####
      voltage = analogRead(A3) * 27777.7777777777777;
      sendLong(voltage);
      break;
   
    case  4: // Abfrage aktuelle Position vorne und hinten ####
    case 14: // Abfrage aktuelle Position vorne und hinten ####
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
      
  }
}

int recInt() { // Erhalte 16 Bit Variable via i2c
  int mem[2];
  mem[0] = Wire.read();
  mem[1] = Wire.read(); mem[1] =  mem[1] << 8;
  return (mem[0] |= mem[1]);
}

long recLong() { // Erhalte 32 Bit Variable via i2c
  long mem[4];
  mem[0] = Wire.read();
  mem[1] = Wire.read(); mem[1] =  mem[1] << 8;
  mem[2] = Wire.read(); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
  mem[3] = Wire.read(); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
  return (mem[0] |= mem[1] |= mem[2] |= mem[3]);
}

void sendLong(long data) { // Sende 32 Bit Variable via i2c
    byte mem[4];
    mem[0]=(data      ) & 0xFF;
    mem[1]=(data >>  8) & 0xFF;
    mem[2]=(data >> 16) & 0xFF;
    mem[3]=(data >> 24) & 0xFF;
    Wire.write(mem,4);
}

void save(long data, int slot) { // Position in EEPROM Speichern
    byte mem[4];
    slot = (slot * 4); // Adresse für EEPROM verschieben
    mem[0]=(data      ) & 0xFF;
    mem[1]=(data >>  8) & 0xFF;
    mem[2]=(data >> 16) & 0xFF;
    mem[3]=(data >> 24) & 0xFF;
    for (int n=0; n<=3;n++) EEPROM.write(slot+n, mem[n]);
}

long load(int slot) {  // Position aus EEPROM laden
  slot = (slot * 4);   // Adresse für EEPROM verschieben
  long mem[4];
  mem[0] = EEPROM.read(slot);
  mem[1] = EEPROM.read(slot + 1); mem[1] =  mem[1] << 8;
  mem[2] = EEPROM.read(slot + 2); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
  mem[3] = EEPROM.read(slot + 3); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
  return (mem[0] |= mem[1] |= mem[2] |= mem[3]);
}

void gear(int gear, int ms1, int ms2, int ms3) { // Gangschaltung
  switch(gear) {
  
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
}

void newHardware() {
  EEPROM.write(0,0);
  EEPROM.write(1,0);
  EEPROM.write(2,1);
  EEPROM.write(3,27);
  save(352049,1);
  save(145667,2);
  save(1000,3);
}

