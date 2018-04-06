/*
Erklärung für die Speicherbelegung:
Ein ATMEGA 328p hat 1024 Bytes EEPROM. Da hier große Zahlen gespeichert werden, werden jeweils 4 Bytes für eine Variable benötigt.
1024 / 4 = 256
Eine Long Variable kann mit der Funktion save(slot,data) gespeichert werden.
Dabei entspricht ein Slot vier aufeinander folgenden Bytes im Speicher.
Somit ergibt es eine maximale Anzahl von 256 Slots. Also Slot 0 bis 255.
Mit der Funktion load() wird der Inhalt des Speichers wieder geladen.
Da für eine Koordinate zwei Variablen abgelegt werden, sind die Slots 1 bis 100 und 129 bis 228 als Speicherplätze reserviert.
Für die restlichen Speicherplätze siehe funktion newHardware()
*/

void newHardware() {
  if (DEBUG) Serial.println("newHardware has been called !!!");
  save(0,   2200);    // Aktuelle Position
  save(0+128, 5000);  // Aktuelle Position
  save(101, 7300);    // dimension_x
  save(102, 5800);    // dimension_y
  save(103, 4600);    // dimension_z
  save(104, 2000);    // range_x_min
  save(104+128, 7300);// range_x_max
  save(105, 2000);    // range_y_min
  save(105+128, 3500);// range_y_max
  save(108,  800);    // maximale Geschwindigkeit in Schritte pro ???
  save(109, 30000);   // Spulendurchmesser vorne in µm
  save(110,150000);   // Spulendurchmesser hinten in µm
  save(120,     1);   // Aktuelle gewählter Slot / newHardware() wird ausgeführt wenn leer
  EEPROM.write(1022,27); // gearRatio. Teil von Slot 255  Planetgetriebe auf Motor montiert...
  EEPROM.write(1023, 1); // stepping.  Teil von Slot 255.  stepping 1,2,4 volle, halbe, viertel Schritte. Leiser aber auch schwächer...
}

void save(int slot, unsigned long data) { // Position in EEPROM Speichern
  if (data != load(slot)) {
    byte mem[4];
    slot = (slot * 4); // Adresse verschieben
    mem[0]=(data      ) & 0xFF;
    mem[1]=(data >>  8) & 0xFF;
    mem[2]=(data >> 16) & 0xFF;
    mem[3]=(data >> 24) & 0xFF;
    for (int n=0; n<=3;n++) EEPROM.write(slot+n, mem[n]);
  }
}

unsigned long load(int slot) {    //Position aus EEPROM laden
  slot = (slot * 4); // Adresse verschieben
  long mem[4];
  mem[0] = EEPROM.read(slot);
  mem[1] = EEPROM.read(slot + 1); mem[1] =  mem[1] << 8;
  mem[2] = EEPROM.read(slot + 2); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
  mem[3] = EEPROM.read(slot + 3); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
  return (mem[0] |= mem[1] |= mem[2] |= mem[3]);
}

void clearEEPROM() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    if (EEPROM.read(i) != 255) EEPROM.write(i, 255);
  }
}
