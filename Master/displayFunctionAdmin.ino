void FUNC_Admin(void){  // Zugangssperre wenn Admin nicht freigeschalten   
  if (millis() - adminTimer > 300000) LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
}


void FUNC_abweichnung_abmess(void) { 
  static unsigned long tempHeight, tempDepth;
  if(!LCDML.FuncInit()) {
    menu = 0;
    cursorOn = true;
    tempHeight = hoehe_mm;
    tempDepth  = tiefe_mm;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176einstellen in mm:"));
    lcd.setCursor(0,1); lcd.print(F("H\357he:"));
    lcd.setCursor(20,0);lcd.print(F("Raumtiefe:"));
    lcd.setCursor(10,1); lcdPrintNR(tempHeight);
    lcd.setCursor(30,0); lcdPrintNR(tempDepth);
    menu = 2;
  }

  if (menu == 1) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    tempHeight ++; menu = 2; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  tempHeight --; menu = 2; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); tempHeight = tempHeight / 10; menu = 2; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  tempHeight = tempHeight * 10; menu = 2; }
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 3; }
  }

  if (menu == 2) {
      if (tempHeight < 0) tempHeight = 0;
      if (tempHeight > gesamthoehe) tempHeight = gesamthoehe;
      lcd.setCursor(10,1); lcdPrintNR(tempHeight); lcd.print((char)0x7F);
      lcd.setCursor(16,1);
      menu = 1;
  }

  if (menu == 3) {
    lcd.setCursor(10,1); lcdPrintNR(tempHeight); lcd.print(F(" "));
    menu = 5;
  }

  if (menu == 4) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    tempDepth ++; menu = 5; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  tempDepth --; menu = 5; } 
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); tempDepth = tempDepth/10; menu = 5; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  tempDepth = tempDepth*10; menu = 5; }
    if (LCDMenuLib_checkButtonEnter()) { menu = 7; }
  }

  if (menu == 5) {
    if (tempDepth < 0) tempDepth = 0;
    if (tempDepth > gesamttiefe) tempDepth = gesamttiefe;
    lcd.setCursor(30,0); lcdPrintNR(tempDepth); lcd.print((char)0x7F);
    lcd.setCursor(36,0);
    menu = 4;
  }

  if (menu == 7) {
  //Motor Stop
  Wire.beginTransmission(0);
  Wire.write(0);
  Wire.endTransmission();
  getCurrent(11);
  CoordinateTOline(tempHeight, tempDepth);
  sendLong(mmTOsteps(targetfrontline_steps - currfrontline_steps,1),1, 7); //vorne  L
  sendLong(mmTOsteps(targetbackline_steps  - currbackline_steps ,0),2, 7); //hinten L
  sendLong(mmTOsteps(targetfrontline_steps - currfrontline_steps,1),3, 7); //vorne  R
  sendLong(mmTOsteps(targetbackline_steps  - currbackline_steps ,0),4, 7); //hinten R
    hoehe_mm = tempHeight;
    tiefe_mm = tempDepth;
    state = 0;
    menu = 99;
    animation();
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}


void FUNC_abweichnung_schnur(void) { 
  static byte selected_motor;
  static long selected_steps;
  if(!LCDML.FuncInit()) {
    cursorOn = true;
    menu = 0;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Abweichung Schnur:"));
    menu = 2;
  }

  if (menu == 1) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    selected_motor++; menu = 2; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  selected_motor--; menu = 2; }
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 3; }
  }

  if (menu == 2) {
      if (selected_motor < 1) selected_motor= 4;
      if (selected_motor > 4) selected_motor= 1;
      lcd.setCursor(0,1); dispmotor(selected_motor,2); lcd.setCursor(15,1); lcd.print((char)0x7F);
      lcd.setCursor(15,1);
      menu = 1;
  }

  if (menu == 3) {
    lcd.setCursor(0,0);
    lcd.print(F("Abweichung in mm:"));
    lcd.setCursor(15,1); lcd.print(F(" "));
    menu = 5;
  }

  if (menu == 4) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    selected_steps++; menu = 5; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  selected_steps--; menu = 5; } 
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); selected_steps=selected_steps/10; menu = 5; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  selected_steps=selected_steps*10; menu = 5; }
    if (LCDMenuLib_checkButtonEnter()) { menu = 7; }
  }

  if (menu == 5) {
    if (selected_steps < -10000) selected_steps=-10000;
    if (selected_steps >  10000) selected_steps= 10000;
    lcd.setCursor(28,0); lcdPrintNR(selected_steps); lcd.print((char)0x7F);
    lcd.setCursor(34,0);
    menu = 4;
  }

  if (menu == 7) {
    if (selected_motor == 1 || selected_motor == 3) sendLong(mmTOsteps(selected_steps,1),selected_motor, 7); //vorne
    if (selected_motor == 2 || selected_motor == 4) sendLong(mmTOsteps(selected_steps,0),selected_motor, 7); //hinten
    state = 1;
    menu = 99;
    animation();
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}

void FUNC_drehrichtung(void) { 
  static byte selected_motor;
  if(!LCDML.FuncInit()) {
    cursorOn = true;
    menu = 0;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Drehrichtung \341ndern:"));
    menu = 2;
  }

  if (menu == 1) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    selected_motor++; menu = 2; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  selected_motor--; menu = 2; }
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 7; }
  }

  if (menu == 2) {
      if (selected_motor < 1) selected_motor= 4;
      if (selected_motor > 4) selected_motor= 1;
      lcd.setCursor(0,1); dispmotor(selected_motor,2); lcd.setCursor(15,1); lcd.print((char)0x7F);
      lcd.setCursor(15,1);
      menu = 1;
  }
  
  if (menu == 7) {
    Wire.beginTransmission(caladdr(selected_motor));
    Wire.write(5);
    Wire.write(calmotor(selected_motor));
    Wire.endTransmission();
    menu = 99;
    animation();
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}

void FUNC_bereich(void)
{ 
  static long gesamtbreite_neu, gesamttiefe_neu, gesamthoehe_neu;
  if(!LCDML.FuncInit()) {
    cursorOn = true;
    menu = 0;
    gesamthoehe_neu = gesamthoehe;
    gesamttiefe_neu = gesamttiefe;
    gesamtbreite_neu = gesamtbreite;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176Aktionsbereich:"));
    lcd.setCursor(0,1); lcd.print(F("H\357he:"));
    lcd.setCursor(20,0);lcd.print(F("Tiefe:"));
    lcd.setCursor(20,1);lcd.print(F("Breite:"));
    lcd.setCursor(10,1); lcdPrintNR(gesamthoehe_neu);
    lcd.setCursor(30,0); lcdPrintNR(gesamttiefe_neu);
    lcd.setCursor(30,1); lcdPrintNR(gesamtbreite_neu);
    menu = 2;
  }

  if (menu == 1) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    gesamthoehe_neu ++; menu = 2; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  gesamthoehe_neu --; menu = 2; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); gesamthoehe_neu = gesamthoehe_neu / 10; menu = 2; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  gesamthoehe_neu = gesamthoehe_neu * 10; menu = 2; }
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 3; }
  }

  if (menu == 2) {
      if (gesamthoehe_neu < 0) gesamthoehe_neu = 0;
      lcd.setCursor(10,1); lcdPrintNR(gesamthoehe_neu); lcd.print((char)0x7F);
      lcd.setCursor(16,1);
      menu = 1;
  }

  if (menu == 3) {
    lcd.setCursor(10,1); lcdPrintNR(gesamthoehe_neu); lcd.print(F(" "));
    menu = 5;
  }

  if (menu == 4) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    gesamttiefe_neu ++; menu = 5; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  gesamttiefe_neu --; menu = 5; } 
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); gesamttiefe_neu = gesamttiefe_neu/10; menu = 5; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  gesamttiefe_neu = gesamttiefe_neu*10; menu = 5; }
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 6; }
  }

  if (menu == 5) {
    if (gesamttiefe_neu < 0) gesamttiefe_neu = 0;
    lcd.setCursor(30,0); lcdPrintNR(gesamttiefe_neu); lcd.print((char)0x7F);
    lcd.setCursor(36,0);
    menu = 4;
  }
  
  if (menu == 6) {
    lcd.setCursor(30,0); lcdPrintNR(gesamttiefe_neu); lcd.print(F(" "));
    menu = 8;
  }
  
  if (menu == 7) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    gesamtbreite_neu ++; menu = 8; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  gesamtbreite_neu --; menu = 8; } 
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); gesamtbreite_neu = gesamtbreite_neu/10; menu = 8; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  gesamtbreite_neu = gesamtbreite_neu*10; menu = 8; }
    if (LCDMenuLib_checkButtonEnter()) { menu = 15; }
  }

  if (menu == 8) {
    if (gesamtbreite_neu < 0) gesamtbreite_neu = 0;
    lcd.setCursor(30,1); lcdPrintNR(gesamtbreite_neu); lcd.print((char)0x7F);
    lcd.setCursor(36,1);
    menu = 7;
  }
  
  if (menu == 15) {
    save(101, gesamtbreite_neu);// gesamtbreite
    save(102, gesamttiefe_neu); // gesamttiefe
    save(103, gesamthoehe_neu); // gesamthoehe
    state = 0; // reinitialisieren
    menu = 99;
    animation();
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}

void FUNC_MinMax(void)
{ 
  static long minhoehe_neu, maxhoehe_neu, mintiefe_neu, maxtiefe_neu;
  if(!LCDML.FuncInit()) {
    cursorOn = true;
    menu = 0;
    minhoehe_neu = minhoehe;
    maxhoehe_neu = maxhoehe;
    mintiefe_neu = mintiefe;
    maxtiefe_neu = maxtiefe;
  }
  
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176Grenzwerte:"));
    lcd.setCursor(0,1); lcd.print(F("Min-h\357he:"));
    lcd.setCursor(20,0);lcd.print(F("Max-h\357he:"));
    lcd.setCursor(10,1); lcdPrintNR(minhoehe_neu);
    lcd.setCursor(30,0); lcdPrintNR(maxhoehe_neu);
    menu = 2;
  }

  if (menu == 1) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    minhoehe_neu ++; menu = 2; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  minhoehe_neu --; menu = 2; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); minhoehe_neu = minhoehe_neu / 10; menu = 2; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  minhoehe_neu = minhoehe_neu * 10; menu = 2; }
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 3; }
  }

  if (menu == 2) {
      if (minhoehe_neu < 0) minhoehe_neu = 0;
      lcd.setCursor(10,1); lcdPrintNR(minhoehe_neu); lcd.print((char)0x7F);
      lcd.setCursor(16,1);
      menu = 1;
  }

  if (menu == 3) {
    lcd.setCursor(10,1); lcdPrintNR(minhoehe_neu); lcd.print(F(" "));
    menu = 5;
  }

  if (menu == 4) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    maxhoehe_neu ++; menu = 5; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  maxhoehe_neu --; menu = 5; } 
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); maxhoehe_neu = maxhoehe_neu/10; menu = 5; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  maxhoehe_neu = maxhoehe_neu*10; menu = 5; }
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 6; }
  }

  if (menu == 5) {
    if (maxhoehe_neu < 0) maxhoehe_neu = 0;
    if (maxhoehe_neu > gesamthoehe) maxhoehe_neu = gesamthoehe;
    lcd.setCursor(30,0); lcdPrintNR(maxhoehe_neu); lcd.print((char)0x7F);
    lcd.setCursor(36,0);
    menu = 4;
  }

  if (menu == 6) {
    if (maxhoehe_neu < minhoehe_neu) maxhoehe_neu = minhoehe_neu;
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176Grenzwerte:"));
    lcd.setCursor(0,1); lcd.print(F("Min-tiefe:"));
    lcd.setCursor(20,0);lcd.print(F("Max-tiefe:"));
    lcd.setCursor(10,1); lcdPrintNR(mintiefe_neu);
    lcd.setCursor(30,0); lcdPrintNR(maxtiefe_neu);
    menu = 8;
  }

  if (menu == 7) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    mintiefe_neu ++; menu = 8; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  mintiefe_neu --; menu = 8; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); mintiefe_neu = mintiefe_neu / 10; menu = 8; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  mintiefe_neu = mintiefe_neu * 10; menu = 8; }
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 9; }
  }

  if (menu == 8) {
      if (mintiefe_neu < 0) mintiefe_neu = 0;
      lcd.setCursor(10,1); lcdPrintNR(mintiefe_neu); lcd.print((char)0x7F);
      lcd.setCursor(16,1);
      menu = 7;
  }

  if (menu == 9) {
    lcd.setCursor(10,1); lcdPrintNR(mintiefe_neu); lcd.print(F(" "));
    menu = 11;
  }

  if (menu == 10) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    maxtiefe_neu ++; menu = 11; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  maxtiefe_neu --; menu = 11; } 
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); maxtiefe_neu = maxtiefe_neu/10; menu = 11; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  maxtiefe_neu = maxtiefe_neu*10; menu = 11; }
    if (LCDMenuLib_checkButtonEnter()) { menu = 15; }
  }

  if (menu == 11) {
    if (maxtiefe_neu < 0) maxtiefe_neu = 0;
    if (maxtiefe_neu > gesamttiefe)  maxtiefe_neu = gesamttiefe;
    lcd.setCursor(30,0); lcdPrintNR(maxtiefe_neu); lcd.print((char)0x7F);
    lcd.setCursor(36,0);
    menu = 10;
  }
   
  if (menu == 15) {
    if (maxtiefe_neu < mintiefe_neu) maxtiefe_neu = mintiefe_neu;
    save(104, maxhoehe_neu);
    save(105, minhoehe_neu);
    save(106, maxtiefe_neu);
    save(107, mintiefe_neu);
    state = 0; // reinitialisieren
    menu = 99;
    animation();
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}

void FUNC_Spule(void)
{ 
  static float SPULE_neu;
  static byte auswahl;
  if(!LCDML.FuncInit()) {
    cursorOn = false;
    menu = 0;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176Spule:"));
    menu = 2;
  }

  if (menu == 1) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    auswahl = !auswahl; menu = 2; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  auswahl = !auswahl; menu = 2; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); auswahl = !auswahl; menu = 2; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  auswahl = !auswahl; menu = 2; }
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 3; }
  }

  if (menu == 2) {
      lcd.setCursor(24,0); if (auswahl) lcd.print(F("vorne ")); else lcd.print(F("hinten")); 
      lcd.print((char)0x7F); lcd.print(F(" "));
      lcd.setCursor(31,0);
      menu = 1;
  }
  if (menu == 3) {
    if (auswahl)  SPULE_neu = SPULEv; else SPULE_neu = SPULEh;
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176Spule:"));
    lcd.setCursor(0,1); lcd.print(F("Durchmesser"));
    lcd.setCursor(20,0); lcd.print(F("in mm:"));
    menu = 5;
  }

  if (menu == 4) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    SPULE_neu = SPULE_neu + 0.01; menu = 5; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  SPULE_neu = SPULE_neu - 0.01; menu = 5; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); SPULE_neu = SPULE_neu / 10; menu = 5; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  SPULE_neu = SPULE_neu * 10; menu = 5; }
    if (LCDMenuLib_checkButtonEnter()) { menu = 15; }
  }

  if (menu == 5) {
      if (SPULE_neu < 0) SPULE_neu = 0;
      lcd.setCursor(27,0); lcd.print(SPULE_neu); 
      lcd.print((char)0x7F); lcd.print(F(" "));
      menu = 4;
  }
  
  if (menu == 15) {
    unsigned long zwischenergebnis;
    zwischenergebnis = SPULE_neu * 1000;
    if (auswahl)  save(109, zwischenergebnis); else save(110, zwischenergebnis);// Spulendurchmesser vorne / hinten
    state = 0; // reinitialisieren
    menu = 99;
    animation();
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}

void FUNC_motoren(void)
{ 
  static long motorSpeed_neu;
  static byte gear_neu, ratio_neu;
  if(!LCDML.FuncInit()) {
    cursorOn = true;
    menu = 0;
    motorSpeed_neu = motorSpeed;
    gear_neu = log(gang) / log(2);
    ratio_neu = ratio;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor( 0,0); lcd.print(F("\176Motoren:"));
    lcd.setCursor( 0,1); lcd.print(F("Geschwind:"));
    lcd.setCursor(20,0); lcd.print(F("Gang:"));
    lcd.setCursor(20,1); lcd.print(F("Ratio:"));
    lcd.setCursor(10,1); lcdPrintNR(motorSpeed_neu);
    lcd.setCursor(30,0); lcdPrintNR(expo(2,gear_neu));
    lcd.setCursor(30,1); lcdPrintNR(ratio_neu);
    menu = 2;
  }

  if (menu == 1) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    motorSpeed_neu ++; menu = 2; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  motorSpeed_neu --; menu = 2; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); motorSpeed_neu = motorSpeed_neu / 10; menu = 2; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  motorSpeed_neu = motorSpeed_neu * 10; menu = 2; }
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 3; }
  }

  if (menu == 2) {
      if (motorSpeed_neu <    0) motorSpeed_neu = 0;
      if (motorSpeed_neu > 1400) motorSpeed_neu = 1400;
      lcd.setCursor(10,1); lcdPrintNR(motorSpeed_neu); lcd.print((char)0x7F);
      lcd.setCursor(16,1);
      menu = 1;
  }

  if (menu == 3) {
    lcd.setCursor(10,1); lcdPrintNR(motorSpeed_neu); lcd.print(F(" "));
    menu = 5;
  }

  if (menu == 4) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    gear_neu ++; menu = 5; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  gear_neu --; menu = 5; } 
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 6; }
  }

  if (menu == 5) {
    if (gear_neu == 255) gear_neu = 0;
    if (gear_neu > 4)    gear_neu = 4;
    lcd.setCursor(30,0); lcdPrintNR(expo(2,gear_neu)); lcd.print((char)0x7F);
    lcd.setCursor(36,0);
    menu = 4;
  }

  if (menu == 6) {
    lcd.setCursor(30,0); lcdPrintNR(expo(2,gear_neu)); lcd.print(F(" "));
    menu = 8;
  }

  if (menu == 7) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    ratio_neu ++; menu = 8; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  ratio_neu --; menu = 8; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); ratio_neu = ratio_neu - 10; menu = 8; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  ratio_neu = ratio_neu + 10; menu = 8; }
    if (LCDMenuLib_checkButtonEnter()) { menu = 15; }
  }

  if (menu == 8) {
      if (ratio_neu <   1) ratio_neu = 1;
      if (ratio_neu > 200) ratio_neu = 1; // Überlauf in Variable
      if (ratio_neu > 100) ratio_neu = 100;
      lcd.setCursor(30,1); lcdPrintNR(ratio_neu); lcd.print((char)0x7F);
      lcd.setCursor(36,1);
      menu = 7;
  }
    
  if (menu == 15) {
    save(108, motorSpeed_neu); // Geschwindigkeit
    gang = expo(2,gear_neu);
    if (EEPROM.read(1023) != gang) EEPROM.write(1023,gang);
    ratio = ratio_neu;
    if (EEPROM.read(1022) != ratio) EEPROM.write(1022,ratio);
    state = 0; // reinitialisieren
    menu = 99;
    animation();
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}

void FUNC_speicherLesen(void) {
  static int select_slot;
  if(!LCDML.FuncInit()) {
    select_slot = slot;
    cursorOn = false;
    menu = 0;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor( 0,0); lcd.print(F("\176Adresse auslesen:"));
    menu = 2;
  }

  if (menu == 1) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    select_slot++; menu = 2; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  select_slot--; menu = 2; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); select_slot = select_slot + 10; menu = 2; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  select_slot = select_slot - 10; menu = 2; }
    if (LCDMenuLib_checkButtonEnter()) { menu = 7; }
  }

  if (menu == 2) {
      if (select_slot < 0)   select_slot= 0;
      if (select_slot > 127) select_slot= 127;
      lcd.setCursor( 0,1); lcdPrintNR(select_slot);
      lcd.setCursor(20,0); lcdPrintNR(select_slot+128);
      lcd.setCursor(10,1); lcdPrintNR(load(select_slot));
      lcd.setCursor(30,0); lcdPrintNR(load(select_slot+128));
      menu = 1;
  }

  if (menu == 7) {
    menu = 99;
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}
