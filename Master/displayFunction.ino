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

void FUNC_back(void){
  LCDML.FuncInit();            // setup function 
  LCDML.Button_quit(2);        // quit button   
  LCDML.FuncEnd(1,0,0,0,0,0);  // direct func end
}

void FUNC_motorStop(void){
      Wire.beginTransmission(0);
      Wire.write(0);
      Wire.endTransmission();
}


