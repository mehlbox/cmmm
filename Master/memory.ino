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
  save(0,   2200); // Aktuelle Position
  save(128, 5000); // Aktuelle Position
  save(101,    0); // gesamtbreite
  save(102, 8000); // gesamttiefe
  save(103, 5800); // gesamthoehe
  save(104, 3500); // maxhoehe
  save(105, 1000); // minhoehe
  save(106,29500); // Spulendurchmesser-alt / trotzdem noch newHardware Erkennung
  save(107,   1);  // Aktuelle gewählter Slot
  save(108, 800);  // Geschwindigkeit
  save(109, 29500);// Spulendurchmesser vorne
  save(110,150000);// Spulendurchmesser hinten
  EEPROM.write(1022,27); // Ratio. Teil von Slot 255  Planetgetriebe auf Motor montiert...
  EEPROM.write(1023, 1); // Gang.  Teil von Slot 255  Gang 1,2,4 volle, halbe, viertel Schritte. Leiser aber auch schwächer...
}

void save(int slot, long data) { // Position in EEPROM Speichern
    byte mem[4];
    slot = (slot * 4); // Adresse verschieben
    mem[0]=(data      ) & 0xFF;
    mem[1]=(data >>  8) & 0xFF;
    mem[2]=(data >> 16) & 0xFF;
    mem[3]=(data >> 24) & 0xFF;
    for (int n=0; n<=3;n++) EEPROM.write(slot+n, mem[n]);
}

long load(int slot) {    //Position aus EEPROM laden
  slot = (slot * 4); // Adresse verschieben
  long mem[4];
  mem[0] = EEPROM.read(slot);
  mem[1] = EEPROM.read(slot + 1); mem[1] =  mem[1] << 8;
  mem[2] = EEPROM.read(slot + 2); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
  mem[3] = EEPROM.read(slot + 3); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
  return (mem[0] |= mem[1] |= mem[2] |= mem[3]);
}


