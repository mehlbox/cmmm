void FUNC_aktuellPos(void) { 
  if(!LCDML.FuncInit()) {
    menu = 0;
    cursorOn = false;
  } 
  if (menu == 0) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(F("\176Position in mm:"));
    lcd.setCursor(0,1); lcd.print(F("H\357he:"));
    lcd.setCursor(20,0);lcd.print(F("Raumtiefe:"));
    lcd.setCursor(10,1); lcdPrintNR(hoehe_mm);
    lcd.setCursor(30,0); lcdPrintNR(tiefe_mm);
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

void FUNC_laden(void) {
  static int select_slot;
  static unsigned long tempHeight, tempDepth;
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
      tempHeight = load(select_slot);
      tempDepth  = load(select_slot+128);
      checkLimits(&tempHeight, &tempDepth);
      lcd.setCursor(10,0); lcdPrintNR(select_slot);
      lcd.setCursor(10,1); lcdPrintNR(tempHeight);
      lcd.setCursor(30,0); lcdPrintNR(tempDepth);
      nameSlot(select_slot);
      lcd.setCursor(16,0);
      menu = 1;
  }

  if (menu == 7) {
    slot = select_slot;
    save(120,slot);
    hoehe_mm = tempHeight;
    tiefe_mm = tempDepth;
    state = 1;
    animation();
    menu = 99;
  }

  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }

}

void FUNC_einstellen(void) { 
  static unsigned long tempHeight, tempDepth;
  if(!LCDML.FuncInit()) {
    menu = 0;
    cursorOn = true;
    tempHeight = hoehe_mm;
    tempDepth = tiefe_mm;
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
      if (tempHeight > maxhoehe) tempHeight = maxhoehe;
      lcd.setCursor(10,1); lcdPrintNR(tempHeight); lcd.print((char)0x7F);
      lcd.setCursor(16,1);
      menu = 1;
  }

  if (menu == 3) {
    if (tempHeight < minhoehe) tempHeight = minhoehe;
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
    if (tempDepth > maxtiefe) tempDepth = maxtiefe;
    lcd.setCursor(30,0); lcdPrintNR(tempDepth); lcd.print((char)0x7F);
    lcd.setCursor(36,0);
    menu = 4;
  }

  if (menu == 7) {
    if (tempDepth < mintiefe) tempDepth = mintiefe;
    checkLimits(&tempHeight, &tempDepth);
    hoehe_mm = tempHeight;
    tiefe_mm = tempDepth;
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
  static unsigned long tempHeight, tempDepth;
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
    lcd.setCursor(10,1); lcdPrintNR(hoehe_mm);
    lcd.setCursor(30,0); lcdPrintNR(tiefe_mm);
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
      tempHeight = load(select_slot);
      tempDepth = load(select_slot+128);
      checkLimits(&tempHeight, &tempDepth);
      lcd.setCursor(14,0); lcdmPrintNR(select_slot); lcd.print((char)0x7F);
      lcd.setCursor(10,1); lcdPrintNR(tempHeight);
      lcd.setCursor(30,0); lcdPrintNR(tempDepth);
      nameSlot(select_slot);
      lcd.setCursor(16,0);
      menu = 1;
  }

  if (menu == 7) {
    slot = select_slot;
    save(120, slot);
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
      if (unit) {
        lcd.setCursor(0,0); lcd.print(F("\176Schritte ")); if (addr == 11) {lcd.print(F("links     "));} else {lcd.print(F("rechts    "));}
        lcd.setCursor(0,1); lcd.print(F(" "));
        lcd.setCursor(22,0); lcdPrintNR(targetfrontline_steps); lcd.print(F(" ")); lcdPrintNR(currfrontline_steps);
        lcd.setCursor(22,1); lcdPrintNR(targetbackline_steps) ; lcd.print(F(" ")); lcdPrintNR(currbackline_steps) ;
      }      
      if (!unit) {
        lcd.setCursor(0,0); lcd.print(F("\176L\341nge Schnur in mm:"));
        lcd.setCursor(0,1); if (addr == 11) {lcd.print(F("L"));} else {lcd.print(F("R"));}
        lcd.setCursor(22,0); lcdPrintNR(targetfrontline_mm); lcd.print(F(" ")); lcdPrintNR(currfrontline_mm); lcd.print(F("  "));
        lcd.setCursor(22,1); lcdPrintNR(targetbackline_mm) ; lcd.print(F(" ")); lcdPrintNR(currbackline_mm) ; lcd.print(F("  "));
      }
    }
  }
  
  if (menu == 99) {
    cursorOn = false;
    LCDML.FuncEnd(1, 0, 0, 0, 0, 0);  // (direct, enter, up, down, left, right)
  }
}

void FUNC_status() {
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

void FUNC_motorStop(void){
      Wire.beginTransmission(0);
      Wire.write(0);
      Wire.endTransmission();
}

void FUNC_back(void){
  LCDML.FuncInit();            // setup function 
  LCDML.Button_quit(2);        // quit button   
  LCDML.FuncEnd(1,0,0,0,0,0);  // direct func end
}

