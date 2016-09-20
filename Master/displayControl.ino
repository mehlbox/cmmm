/* analog menu control */
void LCDMenuLib_control_analog() {
value = analogRead(A3);
if ( (value < 902 && value > 882) && (millis() - buttontimer < 300) ) buttontimer = millis() + 301; // schnelles klicken ermöglichen
if (millis() - buttontimer > 300) { // knopf festhalten begrenzen
  if(LCDML.Timer(g_LCDMenuLib_press_time, _LCDMenuLib_cfg_press_time)) {
         if(value >= _BUTTON_analog_enter_min && value <= _BUTTON_analog_enter_max) {LCDML.Button_enter(); buttonClickTimer = millis();}
    else if(value >= _BUTTON_analog_up_min && value    <= _BUTTON_analog_up_max)    {LCDML.Button_up_down_left_right(_LCDMenuLib_button_up); buttonClickTimer = millis();}
    else if(value >= _BUTTON_analog_down_min && value  <= _BUTTON_analog_down_max)  {LCDML.Button_up_down_left_right(_LCDMenuLib_button_down); buttonClickTimer = millis();}
    else if(value >= _BUTTON_analog_left_min && value  <= _BUTTON_analog_left_max)  {LCDML.Button_up_down_left_right(_LCDMenuLib_button_left); buttonClickTimer = millis();}
    else if(value >= _BUTTON_analog_right_min && value <= _BUTTON_analog_right_max) {LCDML.Button_up_down_left_right(_LCDMenuLib_button_right); buttonClickTimer = millis();}
    
    if(value >= _BUTTON_analog_back_min && value <= _BUTTON_analog_back_max) {      // control quit
      menu = 99;
      LCDML.Button_quit();
      buttonClickTimer = millis();
    }
    
    if(value >= 235 && value <= 255 ) { // schaltet Admin Fubnktionen frei
      lcd.clear();
      lcd.setCursor( 5, 1);
      lcd.print(F("ADMIN !!"));
      delay(500);
      adminTimer = millis();
      buttonClickTimer = millis();
      FUNC_back();
    }
    buttontimer = millis();
  }
  if ( millis() - buttonClickTimer < 50 && fadeValue == 0) FUNC_back();  // aktualisiert Displayanzeige wenn aus war
  if ( millis() - buttonClickTimer < 50 && !fadeState) fadeState = true; // schaltet das Licht wieder ein
}
}

void LCDbackground() {
  static unsigned long fadeTimer;
  if (fadeState && millis() - fadeTimer > 10 && fadeValue < 255){ // fade in Displayhelligkeit
    fadeTimer = millis();
    fadeValue ++;
    analogWrite(_lightPin, fadeValue);
  }

  if (!fadeState && millis() - fadeTimer > 50 && fadeValue > 0){ // fade out Displayhelligkeit
    fadeTimer = millis();
    fadeValue --;
    analogWrite(_lightPin, fadeValue);
  }
}

void checkButtonsInt(unsigned int* buttonCalc, byte menuAction, byte menuEnter, byte menuEnterReset) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    *buttonCalc = *buttonCalc + 1 ; menu = menuAction; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  *buttonCalc = *buttonCalc - 1 ; menu = menuAction; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); *buttonCalc = *buttonCalc - 10; menu = menuAction; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  *buttonCalc = *buttonCalc + 10; menu = menuAction; }
    if (LCDMenuLib_checkButtonEnter()) {
      menu = menuEnter;
      if (menuEnterReset) LCDMenuLib_resetButtonEnter();
      }
}

void checkButtonsLong(unsigned long* buttonCalc, byte menuAction, byte menuEnter, byte menuEnterReset) {
    if (LCDMenuLib_checkButtonUp())    { LCDMenuLib_resetButtonUp();    *buttonCalc = *buttonCalc + 1 ; menu = menuAction; }
    if (LCDMenuLib_checkButtonDown())  { LCDMenuLib_resetButtonDown();  *buttonCalc = *buttonCalc - 1 ; menu = menuAction; }
    if (LCDMenuLib_checkButtonRight()) { LCDMenuLib_resetButtonRight(); *buttonCalc = *buttonCalc / 10; menu = menuAction; }
    if (LCDMenuLib_checkButtonLeft())  { LCDMenuLib_resetButtonLeft();  *buttonCalc = *buttonCalc * 10; menu = menuAction; }
    if (LCDMenuLib_checkButtonEnter()) {
      menu = menuEnter;
      if (menuEnterReset) LCDMenuLib_resetButtonEnter();
      }
}
