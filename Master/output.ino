// output content of EEPROM
// JSON Format
// {Slot:{x,y}}
void readMemory(Client *client) { //
  byte forSlot;
  client->print("{");
  for (forSlot = 0; forSlot <= 100; forSlot ++) {
    client->print("\"");client->print(forSlot);client->print("\":");
    client->print("{");client->print("\"x\":");client->print(load(forSlot+128));client->print(",\"y\":");client->print(load(forSlot));client->println("},");
  }
  client->print("\"slot\":");client->print(slot);client->print("}"); // add slot to 101
}

void i2cScan(Client *client) {
  byte error, address;
  int nDevices;
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
     if (error == 0) {
      client->print("I2C device found at address 0x");
      if (address<16) client->print("0");
      client->print(address,HEX);
      client->println("  !");
      nDevices++;
    } else if (error==4) {
      client->print("Unknown error at address 0x");
      if (address<16) client->print("0");
      client->println(address,HEX);
    }
  }
  if (nDevices == 0) client->println("No I2C devices found\n");
}
