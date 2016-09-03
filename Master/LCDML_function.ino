void FUNC_laden(void) {
  static int select_slot;
  static float temp_hoehe, temp_tiefe;
  if(!LCDML.FuncInit()) {
    select_slot = slot;
    menu = 0;
    cursorOn = true;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor( 0,0); lcd.print(F("\176laden:"));
    lcd.setCursor( 0,1); lcd.print(F("H\357he:"));
    lcd.setCursor(20,0); lcd.print(F("Raumtiefe:"));
    lcd.setCursor(17,0); lcd.print((char)0x7F);
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
      if (select_slot < 1)   select_slot= 100;
      if (select_slot > 100) select_slot= 1;
      temp_hoehe = load(select_slot);
      temp_tiefe = load(select_slot+128);
      if (temp_hoehe == -1) temp_hoehe = 2500;
      if (temp_hoehe > maxhoehe) temp_hoehe = maxhoehe;
      if (temp_hoehe < minhoehe) temp_hoehe = minhoehe;
      if (temp_tiefe == -1) temp_tiefe = 0;
      if (temp_tiefe > gesamttiefe) temp_tiefe = gesamttiefe;
      lcd.setCursor(10,0); lcdspace(select_slot);
      lcd.setCursor(10,1); lcdspace(temp_hoehe);
      lcd.setCursor(30,0); lcdspace(temp_tiefe);
      lcd.setCursor(16,0);
      menu = 1;
  }

  if (menu == 7) {
    slot = select_slot;
    save(107,slot);
    hoehe_mm = temp_hoehe;
    tiefe_mm = temp_tiefe;
    state = 1;
    animation();
    menu = 99;
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }

}

void FUNC_einstellen(void)
{ 
  static long hoehe_neu, tiefe_neu;
  if(!LCDML.FuncInit()) {
    menu = 0;
    cursorOn = true;
    hoehe_neu = hoehe_mm;
    tiefe_neu = tiefe_mm;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176einstellen in mm:"));
    lcd.setCursor(0,1); lcd.print(F("H\357he:"));
    lcd.setCursor(20,0);lcd.print(F("Raumtiefe:"));
    lcd.setCursor(10,1); lcdspace(hoehe_neu);
    lcd.setCursor(30,0); lcdspace(tiefe_neu);
    menu = 2;
  }

  if (menu == 1) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    hoehe_neu ++; menu = 2; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  hoehe_neu --; menu = 2; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); hoehe_neu = hoehe_neu / 10; menu = 2; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  hoehe_neu = hoehe_neu * 10; menu = 2; }
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 3; }
  }

  if (menu == 2) {
      if (hoehe_neu < 0) hoehe_neu = 0;
      if (hoehe_neu > maxhoehe) hoehe_neu = maxhoehe;
      lcd.setCursor(10,1); lcdspace(hoehe_neu); lcd.print((char)0x7F);
      lcd.setCursor(16,1);
      menu = 1;
  }

  if (menu == 3) {
    if (hoehe_neu < minhoehe) hoehe_neu = minhoehe;
    lcd.setCursor(10,1); lcdspace(hoehe_neu); lcd.print(F(" "));
    menu = 5;
  }

  if (menu == 4) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    tiefe_neu ++; menu = 5; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  tiefe_neu --; menu = 5; } 
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); tiefe_neu = tiefe_neu/10; menu = 5; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  tiefe_neu = tiefe_neu*10; menu = 5; }
    if (LCDMenuLib_checkButtonEnter()) { menu = 7; }
  }

  if (menu == 5) {
    if (tiefe_neu < 0) tiefe_neu = 0;
    if (tiefe_neu > gesamttiefe) tiefe_neu = gesamttiefe;
    lcd.setCursor(30,0); lcdspace(tiefe_neu); lcd.print((char)0x7F);
    lcd.setCursor(36,0);
    menu = 4;
  }

  if (menu == 7) {
    hoehe_mm = hoehe_neu;
    tiefe_mm = tiefe_neu;
    state = 1;
    menu = 99;
    animation();
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}

void FUNC_aktuellPos(void)
{ 
  if(!LCDML.FuncInit()) {
    menu = 0;
    cursorOn = false;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176Position in mm:"));
    lcd.setCursor(0,1); lcd.print(F("H\357he:"));
    lcd.setCursor(20,0);lcd.print(F("Raumtiefe:"));
    lcd.setCursor(10,1); lcdspace(hoehe_mm);
    lcd.setCursor(30,0); lcdspace(tiefe_mm);
    menu = 1;
  }

  if (menu == 1) {
    if (LCDMenuLib_checkButtonEnter()) { LCDMenuLib_resetButtonEnter(); menu = 7; }
  }

  if (menu == 7) {
    state = 1;
    menu = 99;
    animation();
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}

void FUNC_speichern(void) {
  static int select_slot;
  if(!LCDML.FuncInit()) {
    select_slot = slot;
    menu = 0;
    cursorOn = true;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176speichern in:"));
    lcd.setCursor(0,1); lcd.print(F("H\357he:"));
    lcd.setCursor(20,0);lcd.print(F("Raumtiefe:"));
    lcd.setCursor(10,1); lcdspace(hoehe_mm);
    lcd.setCursor(30,0); lcdspace(tiefe_mm);
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
      if (select_slot < 1)   select_slot= 100;
      if (select_slot > 100) select_slot= 1;
      lcd.setCursor(14,0); lcdmspace(select_slot); lcd.print((char)0x7F);
      lcd.setCursor(16,0);
      menu = 1;
  }

  if (menu == 7) {
    slot = select_slot;
    save(107, slot);
    save(slot    , hoehe_mm);
    save(slot+128, tiefe_mm);
    menu = 99;
    animation();
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}

void FUNC_status()
{
static unsigned long timer;
  if(!LCDML.FuncInit())
  {
    cursorOn = false;
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("Status: ")); lcd.print(state);
    lcd.setCursor(2,1); lcd.print(F("Slot: "));   lcd.print(slot);
    lcd.setCursor(20,0); lcd.print(F("L: "));
    lcd.setCursor(20,1); lcd.print(F("R: "));
  }
    if (millis() - timer >= 500) { // I2C BUS NICHT ZUMÜLLEN
    timer = millis();
      for (int n=1; n<=4; n++) {
        if (n == 1) lcd.setCursor(16,0);
        if (n == 2) lcd.setCursor(36,1);
        if (n == 3) lcd.setCursor(19,0);
        if (n == 4) lcd.setCursor(39,1);
        lcd.print(motorstatus(n));
      }
    lcd.setCursor(23,0); lcd.print(spannung(1),1); lcd.print(F(" Volt"));
    lcd.setCursor(23,1); lcd.print(spannung(3),1); lcd.print(F(" Volt"));      
    }
  LCDMenuLib_checkButtonEnter();
  LCDML.FuncEnd(0, 1, 0, 0, 0, 0); // (direct, enter, up, down, left, right)     
}

void FUNC_zeigenSchnur(void)
{ 
  static unsigned long timer;
  static byte unit, addr; // 0 = schritte, 1 = mm
  if (addr == 0) addr = 11;
  if(!LCDML.FuncInit()) {
    cursorOn = false;
    menu = 0;
    timer = 0;
  } 
  if (menu == 0) {
    lcd.clear();
    menu = 1;
  }

  if (menu == 1) {
      lcd.setCursor( 5,1); lcd.print(F("soll")); lcd.setCursor(14,1); lcd.print(F("ist")); 
      lcd.setCursor(20,0); lcd.print(F("V:"));
      lcd.setCursor(20,1); lcd.print(F("H:"));
      menu = 2;
  }
  
  if (menu == 2) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    unit = !unit; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  unit = !unit; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); if (addr == 11) {addr = 12;} else {addr = 11;} }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  if (addr == 11) {addr = 12;} else {addr = 11;} }
    if (LCDMenuLib_checkButtonEnter()) { menu = 99; }
    if (millis() - timer > 500) { // I2C BUS NICHT ZUMÜLLEN
      timer = millis();
      getCurrent(addr);
      if (!unit) {
        lcd.setCursor(0,0); lcd.print(F("\176Schritte ")); if (addr == 11) {lcd.print(F("links     "));} else {lcd.print(F("rechts    "));}
        lcd.setCursor(0,1); lcd.print(F(" "));
        lcd.setCursor(22,0); lcdspace(targetfrontline_steps); lcd.print(F(" ")); lcdspace(currfrontline_steps);
        lcd.setCursor(22,1); lcdspace(targetbackline_steps) ; lcd.print(F(" ")); lcdspace(currbackline_steps) ;
      }      
      if (unit) {
        lcd.setCursor(0,0); lcd.print(F("\176L\341nge Schnur in mm:"));
        lcd.setCursor(0,1); if (addr == 11) {lcd.print(F("L"));} else {lcd.print(F("R"));}
        lcd.setCursor(22,0); lcdspace(targetfrontline_mm); lcd.print(F(" ")); lcdspace(currfrontline_mm); lcd.print(F("  "));
        lcd.setCursor(22,1); lcdspace(targetbackline_mm) ; lcd.print(F(" ")); lcdspace(currbackline_mm) ; lcd.print(F("  "));
      }
    }
  }
  
  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}

void FUNC_abweichnung(void)
{ 
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
    if (selected_steps < -1000) selected_steps=-1000;
    if (selected_steps >  1000) selected_steps= 1000;
    lcd.setCursor(28,0); lcdspace(selected_steps); lcd.print((char)0x7F);
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
    lcd.setCursor(10,1); lcdspace(gesamthoehe_neu);
    lcd.setCursor(30,0); lcdspace(gesamttiefe_neu);
    lcd.setCursor(30,1); lcdspace(gesamtbreite_neu);
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
      lcd.setCursor(10,1); lcdspace(gesamthoehe_neu); lcd.print((char)0x7F);
      lcd.setCursor(16,1);
      menu = 1;
  }

  if (menu == 3) {
    lcd.setCursor(10,1); lcdspace(gesamthoehe_neu); lcd.print(F(" "));
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
    lcd.setCursor(30,0); lcdspace(gesamttiefe_neu); lcd.print((char)0x7F);
    lcd.setCursor(36,0);
    menu = 4;
  }
  
  if (menu == 6) {
    lcd.setCursor(30,0); lcdspace(gesamttiefe_neu); lcd.print(F(" "));
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
    lcd.setCursor(30,1); lcdspace(gesamtbreite_neu); lcd.print((char)0x7F);
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
  static long minhoehe_neu, maxhoehe_neu;
  if(!LCDML.FuncInit()) {
    cursorOn = true;
    menu = 0;
    minhoehe_neu = minhoehe;
    maxhoehe_neu = maxhoehe;
  }
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176Grenzwerte:"));
    lcd.setCursor(0,1); lcd.print(F("Min-h\357he:"));
    lcd.setCursor(20,0);lcd.print(F("Max-h\357he:"));
    lcd.setCursor(10,1); lcdspace(minhoehe_neu);
    lcd.setCursor(30,0); lcdspace(maxhoehe_neu);
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
      lcd.setCursor(10,1); lcdspace(minhoehe_neu); lcd.print((char)0x7F);
      lcd.setCursor(16,1);
      menu = 1;
  }

  if (menu == 3) {
    lcd.setCursor(10,1); lcdspace(minhoehe_neu); lcd.print(F(" "));
    menu = 5;
  }

  if (menu == 4) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    maxhoehe_neu ++; menu = 5; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  maxhoehe_neu --; menu = 5; } 
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); maxhoehe_neu = maxhoehe_neu/10; menu = 5; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  maxhoehe_neu = maxhoehe_neu*10; menu = 5; }
    if (LCDMenuLib_checkButtonEnter()) { menu = 15; }
  }

  if (menu == 5) {
    if (maxhoehe_neu < 0) maxhoehe_neu = 0;
    if (maxhoehe_neu < minhoehe_neu) maxhoehe_neu = minhoehe_neu;
    lcd.setCursor(30,0); lcdspace(maxhoehe_neu); lcd.print((char)0x7F);
    lcd.setCursor(36,0);
    menu = 4;
  }
    
  if (menu == 15) {
    save(104, maxhoehe_neu); // maxhoehe
    save(105, minhoehe_neu); // minhoehe
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
    cursorOn = true;
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
      lcd.setCursor(31,0);
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

/*
void FUNC_Spule_alt(void)
{ 
  static float SPULE_neu;
  if(!LCDML.FuncInit()) {
    cursorOn = true;
    menu = 0;
    SPULE_neu = SPULE;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176Spule:"));
    lcd.setCursor(0,1); lcd.print(F("Durchmesser"));
    lcd.setCursor(20,0); lcd.print(F("in mm:"));
    menu = 2;
  }

  if (menu == 1) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    SPULE_neu = SPULE_neu + 0.01; menu = 2; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  SPULE_neu = SPULE_neu - 0.01; menu = 2; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); SPULE_neu = SPULE_neu / 10; menu = 2; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  SPULE_neu = SPULE_neu * 10; menu = 2; }
    if (LCDMenuLib_checkButtonEnter()) { menu = 15; }
  }

  if (menu == 2) {
      if (SPULE_neu < 0) SPULE_neu = 0;
      lcd.setCursor(27,0); lcd.print(SPULE_neu); 
      lcd.print((char)0x7F); lcd.print(F(" "));
      lcd.setCursor(31,0);
      menu = 1;
  }
    
  if (menu == 15) {
    unsigned long zwischenergebnis;
    zwischenergebnis = SPULE_neu * 1000;
    save(106, zwischenergebnis);// Spulendurchmesser
    state = 0; // reinitialisieren
    menu = 99;
    animation();
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}
*/

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
    lcd.setCursor(10,1); lcdspace(motorSpeed_neu);
    lcd.setCursor(30,0); lcdspace(expo(2,gear_neu));
    lcd.setCursor(30,1); lcdspace(ratio_neu);
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
      lcd.setCursor(10,1); lcdspace(motorSpeed_neu); lcd.print((char)0x7F);
      lcd.setCursor(16,1);
      menu = 1;
  }

  if (menu == 3) {
    lcd.setCursor(10,1); lcdspace(motorSpeed_neu); lcd.print(" ");
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
    lcd.setCursor(30,0); lcdspace(expo(2,gear_neu)); lcd.print((char)0x7F);
    lcd.setCursor(36,0);
    menu = 4;
  }

  if (menu == 6) {
    lcd.setCursor(30,0); lcdspace(expo(2,gear_neu)); lcd.print(" ");
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
      lcd.setCursor(30,1); lcdspace(ratio_neu); lcd.print((char)0x7F);
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
      lcd.setCursor( 0,1); lcdspace(select_slot);
      lcd.setCursor(20,0); lcdspace(select_slot+128);
      lcd.setCursor(10,1); lcdspace(load(select_slot));
      lcd.setCursor(30,0); lcdspace(load(select_slot+128));
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

void FUNC_Admin(void){  // Zugangssperre wenn Admin nicht freigeschalten   
  if (millis() - adminTimer > 300000) LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
}

void FUNC_back(void){
  LCDML.FuncInit();            // setup function 
  LCDML.Button_quit(2);        // quit button   
  LCDML.FuncEnd(1,0,0,0,0,0);  // direct func end
}

void FUNC_motorStop(void){
  //LCDML.FuncInit();            // setup function 
  //LCDML.Button_quit(2);        // quit button   
  //LCDML.FuncEnd(1,0,0,0,0,0);  // direct func end
      Wire.beginTransmission(0);
      Wire.write(0);
      Wire.endTransmission();
}

