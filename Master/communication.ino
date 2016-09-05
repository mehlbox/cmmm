char motorstatus(int motor) { // Motor Statusabfrage
  char stat;
  int addr;
    addr = caladdr(motor);
    motor = calmotor(motor);
    busAction = 6;
    Wire.beginTransmission(addr);
    Wire.write(busAction);
    Wire.write(motor);
    Wire.endTransmission();
    Wire.requestFrom(addr, 1);
    switch (Wire.read()) {
      case -1:
        stat = '#'; // Keine Verbindung
        break;
      case 1:
        stat = '+'; // Motor dreht vorwärts
        break;
      case 2:
        stat = '-'; // Motor dreht rückwärts
        break;
      case 3:
        stat = 'O'; // Motor steht
        break;
      }
    return stat;
}

float spannung(int motor) { // Motor Statusabfrage
    char stat;
    int addr;
    long longVoltage;
    addr = caladdr(motor);
    busAction = 12;
    Wire.beginTransmission(addr);
    Wire.write(busAction);
    Wire.endTransmission();
    Wire.requestFrom(addr, 4);
    unsigned long mem[4];
    mem[0] = Wire.read();
    mem[1] = Wire.read(); mem[1] =  mem[1] << 8;
    mem[2] = Wire.read(); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
    mem[3] = Wire.read(); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
    longVoltage = mem[0] |= mem[1] |= mem[2] |= mem[3];
    longVoltage = longVoltage * 0.000001;
    if (longVoltage > 2) longVoltage = longVoltage + 0.45; // 
    return longVoltage;
}

void getCurrent(int addr) { // Aktuelle Werte holen via I2C
  if (busAction != 14 && busAction != 4) {
    busAction = 14;
    Wire.beginTransmission(0);
    Wire.write(busAction);
    Wire.endTransmission();
  }
  Wire.requestFrom(addr, 8);
  unsigned long mem[8];
  mem[0] = Wire.read();
  mem[1] = Wire.read(); mem[1] =  mem[1] << 8;
  mem[2] = Wire.read(); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
  mem[3] = Wire.read(); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
  currfrontline_steps = mem[0] |= mem[1] |= mem[2] |= mem[3];
  mem[4] = Wire.read();
  mem[5] = Wire.read(); mem[5] =  mem[5] << 8;
  mem[6] = Wire.read(); mem[6] =  mem[6] << 8; mem[6] =  mem[6] << 8;
  mem[7] = Wire.read(); mem[7] =  mem[7] << 8; mem[7] =  mem[7] << 8; mem[7] =  mem[7] << 8;
  currbackline_steps = mem[4] |= mem[5] |= mem[6] |= mem[7];
  currfrontline_mm = stepsTOmm(currfrontline_steps,1);
  currbackline_mm  = stepsTOmm(currbackline_steps,0);
}

unsigned long getlong(byte motor, byte action) { // Long Wert holen via I2C
  busAction = action;
  Wire.beginTransmission(caladdr(motor));
  Wire.write(action);
  Wire.write(calmotor(motor));
  Wire.endTransmission();
  Wire.requestFrom(caladdr(motor), 4);
  long mem[4];
  long data;
  mem[0] = Wire.read();
  mem[1] = Wire.read(); mem[1] =  mem[1] << 8;
  mem[2] = Wire.read(); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
  mem[3] = Wire.read(); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
  data = mem[0] |= mem[1] |= mem[2] |= mem[3];
  return data;
}

void sendTargetValue() { // Zielwerte senden
    busAction = 3;
    byte data[9]; 
    data[0]= busAction; // action
    data[1]=(targetfrontline_steps      ) & 0xFF; // vordere Schnur Zielwert
    data[2]=(targetfrontline_steps >>  8) & 0xFF;
    data[3]=(targetfrontline_steps >> 16) & 0xFF;
    data[4]=(targetfrontline_steps >> 24) & 0xFF;
    data[5]=(targetbackline_steps      ) & 0xFF; // hintere Schnur Zielwert
    data[6]=(targetbackline_steps >>  8) & 0xFF;
    data[7]=(targetbackline_steps >> 16) & 0xFF;
    data[8]=(targetbackline_steps >> 24) & 0xFF;
    Wire.beginTransmission(0); //broadcast
    Wire.write(data,9);
    Wire.endTransmission();
}

void sendLong(long senddata, byte motor, byte action) { // Long Wert senden
    busAction = action;
    byte data[6];
    data[0]=action;
    data[1]=calmotor(motor);
    data[2]=(senddata      ) & 0xFF;
    data[3]=(senddata >>  8) & 0xFF;
    data[4]=(senddata >> 16) & 0xFF;
    data[5]=(senddata >> 24) & 0xFF;
    Wire.beginTransmission(caladdr(motor));
    Wire.write(data,6);
    Wire.endTransmission();
}

void checkError() { //selbstdiagnose
  byte fehler;
  int stat1, stat3, span1, span3;
  static unsigned long errorMillis;
  if (demo == false) { //keine Fehler im Demo prüfen.
    stat1 = motorstatus(1);
    stat3 = motorstatus(3);
    span1 = spannung(1);
    span3 = spannung(3);
    if (stat1 == '#' || stat3 == '#' || span1 < 8 || span3 < 8) { // Angabe Spannung in Volt
      fehler = true;
      lcd.noCursor();
      lcd.clear();
      lcd.setCursor(0,0); lcd.print(F("Fehler!!!"));
      lcd.setCursor(8,1); lcd.print(F("Links Rechts"));
      digitalWrite(_lightPin, HIGH);
      while(fehler) {
        if (stat1 != '#' && stat3 != '#' && span1 >= 9 && span3 >= 9) { fehler = false; FUNC_back(); } // Fehler behoben
        if (millis() >= errorMillis && fehler == true) {
          Wire.beginTransmission(0); Wire.write(0); Wire.endTransmission(); // alle noch erreichbaren Motoren anhalten
          errorMillis = millis() + 100;
          stat1 = motorstatus(1);
          stat3 = motorstatus(3);
          span1 = spannung(1);
          span3 = spannung(3);
          lcd.setCursor(20,0); lcd.print(F("Verbi.: "));
            if (stat1 == '#') { lcd.print(F("Ni OK ")); } else {lcd.print(F(" OK   ")); }
            if (stat3 == '#') { lcd.print(F("Ni OK ")); } else {lcd.print(F(" OK   ")); }
          lcd.setCursor(20,1); lcd.print(F("Spann.: "));
            if (span1  <  9 ) { lcd.print(F("Ni OK " )); } else {lcd.print(F(" OK   " )); }
            if (span3  <  9 ) { lcd.print(F("Ni OK " )); } else {lcd.print(F(" OK   " )); }
        }
      }
    }
  } else {
    Wire.beginTransmission(0); Wire.write(0); Wire.endTransmission(); // Motoren im Demo Modus permanent Stop Signal schicken.
    if (millis() > 60000) demo = false; // Demo Modus nach einer Minuten verlassen.
  }
}
