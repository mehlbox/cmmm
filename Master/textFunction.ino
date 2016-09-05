void animation() { // Animation (beim senden)
  for (int n=0; n<20; n++) {
    lcd.setCursor(n,0);
    lcd.print(F("#"));
    lcd.setCursor(n,1);
    lcd.print(F("#"));
    lcd.setCursor(n+20,0);
    lcd.print(F("#"));
    lcd.setCursor(n+20,1);
    lcd.print(F("#"));
    delay(20);
  }
}

void dispmotor(byte motor, byte extent) { // Text zusammenstellen für Displayausgabe
  if (motor == 1 || motor == 3) lcd.print(F(" vorne "));
  if (motor == 2 || motor == 4) lcd.print(F("hinten "));
  
  if (extent == 1) {
    if (motor == 1 || motor == 2) lcd.print(F("L"));
    if (motor == 3 || motor == 4) lcd.print(F("R"));
  }
  
  if (extent == 2) {
    if (motor == 1 || motor == 2) lcd.print(F("links "));
    if (motor == 3 || motor == 4) lcd.print(F("rechts"));
  }
}

void dispaction(byte stat) { // Text zusammenstellen für Displayausgabe
  if (stat == 2 || stat == 4) lcd.print(F("Stop  "));
  if (stat == 1) lcd.print(F("hoch  "));
  if (stat == 3) lcd.print(F("runter"));
}

int lcdPrintNR(long x) { // Anzahl Leerzeichen vor Zahlen
  int space = 0;
    if( x >= 0) space++;
    if( abs(x) < 10) space++;
    if( abs(x) < 100) space++;
    if( abs(x) < 1000) space++;
    if( abs(x) < 10000) space++;
    if( abs(x) < 100000) space++;
    for (int n = 1; n <= space; n++) lcd.print(F(" "));
    lcd.print(x);
}

int lcdmPrintNR(long x) { // Anzahl Leerzeichen vor kleinen Zahlen
  int space = 0;
    if( abs(x) < 10) space++;
    if( abs(x) < 100) space++;
    for (int n = 1; n <= space; n++) lcd.print(F(" "));
    lcd.print(x);
}
