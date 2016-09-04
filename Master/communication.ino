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
