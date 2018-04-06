#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include <ArduinoJson.h>

#define _lockPin  A2
#define DEBUG false

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 10, 178);
EthernetServer server(80);

float spool_front, spool_back;   // durchmesser Spule in µm
unsigned long dimension_z, dimension_x, dimension_y, range_y_max, range_y_min, range_x_max, range_x_min; // Geometrie Grenzen in mm
unsigned long motorSpeedLimit;
unsigned long currfrontline_steps, currfrontline_mm, currbackline_steps, currbackline_mm; // Schritte
unsigned long targetfrontline_steps, targetfrontline_mm, targetbackline_steps, targetbackline_mm; // Schritte
long speedfrontline_steps, speedbackline_steps;
unsigned long set_x_mm, set_y_mm, save_x_mm, save_y_mm; // coordinates in millimeter
byte gearRatio, stepping;
byte busAction, slot, lastLockState;

byte state = 0;
// Mögliche Werte für State
// 0 = Initialisierung
// 1 = Neue Werte erhalten
// 2 = Motoren laufen
// 3 = Zielwert erreicht, nachschwingen abwarten, speichern
// 4 = Display nach 5 Sekundenaussschalten

void setup()
{
  pinMode(_lockPin, INPUT); digitalWrite(_lockPin, HIGH); // für die Abfrage ob Mischpult aus
  while(!digitalRead(_lockPin)); // nicht aufwachen wenn Mischpult aus
  wdt_enable(WDTO_2S);
  Wire.begin(); // start up i2c bus
  TWBR = 158; TWSR |= bit(TWPS0); // slow down bus clock
  if (load(120) == -1 ) newHardware(); //EEPROM is empty ... load default
  lastLockState = digitalRead(_lockPin); // prevent loading slot 100 on boot up
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.begin(9600);
}

void loop() {
  static unsigned long runMillis, sleepMillis, i2cMillis, looptime; // Timer Variablen
  looptime = millis();
  wdt_reset();


  if (state == 0) { // initialisierung - letzte Position laden
    set_y_mm    = load(0);
    set_x_mm    = load(0+128);
    dimension_x = load(101);
    dimension_y = load(102);
    dimension_z = load(103);
    range_x_min = load(104);
    range_x_max = load(104+128);
    range_y_min = load(105);
    range_y_max = load(105+128);
    motorSpeedLimit  = load(108);
    spool_front = load(109);
    spool_back  = load(110);
    slot = load(120);
    sendLong(motorSpeedLimit, 0, 9);

    gearRatio = EEPROM.read(1022);
      Wire.beginTransmission(0); //sende gearRatio
      Wire.write(11);
      Wire.write(gearRatio);
      Wire.endTransmission();
    stepping = EEPROM.read(1023);
      Wire.beginTransmission(0); //sende stepping
      Wire.write(8);
      Wire.write(stepping);
      Wire.endTransmission();
    state = 1;

  }

  if (state == 1) { // Neue Werte erhalten
      if (DEBUG) Serial.println("state 1 - new value");
      //checkError();
      checkLimits(&set_y_mm, &set_x_mm);
      save(0,    set_y_mm); // Neue Position in EEPROM speichern
      save(0+128, set_x_mm); // Neue Position in EEPROM speichern
      CoordinateTOline(set_y_mm, set_x_mm); //Zielwerte berechnen
      sendTargetValue(); // Einstellungen via broadcasting versenden
      state = 2;
      if (DEBUG) Serial.println("state 2 - motors are running");
  }

  if(state == 2) sleepMillis = millis(); // motoren laufen

  if(millis() >= runMillis && state == 2 ) {
    //if (DEBUG) Serial.print(currfrontline_steps); if (DEBUG) Serial.print(" "); if (DEBUG) Serial.println(currbackline_steps);
    runMillis = millis() + 1000; // 1 mal pro Sekunde
    unsigned long previousfrontline_steps = currfrontline_steps;
    unsigned long previousbackline_steps = currbackline_steps;
    getCurrent(11); // Hole aktuelle Schnurlänge vorne + hinten
    speedfrontline_steps = currfrontline_steps - previousfrontline_steps;
    speedbackline_steps  = currbackline_steps  - previousbackline_steps;
    if (currfrontline_steps == targetfrontline_steps) {
      //getCurrent(12);
      //if (currfrontline_steps == targetfrontline_steps) {
        state = 3;
        if (DEBUG) Serial.println("state 3 - arrived");
      //}
    }
  }

  if(millis() >= i2cMillis) { // && state != 2
    i2cMillis = millis() + 1000; // 1 mal pro Sekunde
    //checkError();
   }

  if(state == 3 && millis() - sleepMillis >= 1000) { // abwarten nach Position erreicht - fertig
    state = 4;
    if (DEBUG) Serial.println("state 4 - stable");
  }


  if(!digitalRead(_lockPin) && lastLockState == 1) { // Mischpult ausgeschaltet. Mikrofone anheben bzw. Position 100 laden.
    lastLockState = 0;
    set_y_mm = load(100);   // Slot 100 als Standart laden
    set_x_mm = load(1+128); // Slot 1 als Standart laden
    checkLimits(&set_y_mm, &set_x_mm);
    state = 1;
    if (DEBUG) Serial.println("state 1 - mixer has been turned off");
  }

  /*if(state == 4) { // nicht wieder aufwachen wenn Mischpult aus
    wdt_disable();
    while(!digitalRead(_lockPin)); // sleep
    wdt_enable(WDTO_2S);
  }*/

  if(digitalRead(_lockPin) && lastLockState == 0) { // Mischpult eingeschaltet.. Slot 1 laden
    lastLockState = 1;
    set_y_mm = load(1);     // Slot 1 als Standart laden
    set_x_mm = load(1+128); // Slot 1 als Standart laden
    checkLimits(&set_y_mm, &set_x_mm);
    state = 1;
    if (DEBUG) Serial.println("state 1 - mixer has been turned on");
  }
network();
looptime = millis() - looptime;
if ( looptime >  50 ) { Serial.print("loopduration: "); Serial.println(looptime); }
if ( looptime > 100 ) { delay(10); Serial.println("desync");} // desyncronize with client
if ( looptime > 500 ) { // restart master device
  Serial.println("restart"); 
  wdt_enable(WDTO_15MS);
  while(true); }
}
