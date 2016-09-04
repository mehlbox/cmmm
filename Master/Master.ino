/*
  lcd.print("\341"); //ä
  lcd.print("\357"); //ö
  lcd.print("\365"); //ü
  lcd.print("\342"); //ß
*/

/* include libs */
#include <Wire.h>
#include <LiquidCrystal.h>
#include <LCDMenuLib.h>
#include <EEPROM.h>

/* lcd dimension*/
#define _LCDMenuLib_LCD_cols             20
#define _LCDMenuLib_LCD_rows             4

/* pin settings for lcd*/  
#define _LCDMenuLib_LCD_rs               2
#define _LCDMenuLib_LCD_e                3
#define _LCDMenuLib_LCD_dat0             6
#define _LCDMenuLib_LCD_dat1             7
#define _LCDMenuLib_LCD_dat2             8
#define _LCDMenuLib_LCD_dat3             9 

/* lib config */
#define _LCDMenuLib_cfg_initscreen       0      /* 0=disable, 1=enable */
#define _LCDMenuLib_cfg_initscreen_time  5000   /* 0=time disabled, >0 wait time in ms */
#define _LCDMenuLib_cfg_scrollbar	 1      	    /* 0=no scrollbar, 1=complex scrollbar, 2=easy scrollbar */
#define _LCDMenuLib_cfg_lcd_standard	 0        /* 0=HD44780 standard / 1=HD44780U standard */
#define _LCDMenuLib_cfg_press_time       50     /* button press time in ms */

// Menue
#define _LCDMenuLib_cnt    14  // last element id


// Keypad
  unsigned long buttontimer;
  uint16_t value;  // analogpin for keypad
  #define _BUTTON_analog_enter_min     686     // Button Enter
  #define _BUTTON_analog_enter_max     706  
  #define _BUTTON_analog_up_min        553     // Button Up
  #define _BUTTON_analog_up_max        573   
  #define _BUTTON_analog_down_min      623     // Button Down
  #define _BUTTON_analog_down_max      643   
  #define _BUTTON_analog_back_min        0     // Button Back
  #define _BUTTON_analog_back_max        5   
  #define _BUTTON_analog_left_min      380     // Button Left
  #define _BUTTON_analog_left_max      400   
  #define _BUTTON_analog_right_min     654     // Button Right
  #define _BUTTON_analog_right_max     674 



/* init lcdmenulib */
LCDMenuLib_init(_LCDMenuLib_cnt);
/* LCDMenuLib_element(id, prev_layer_element, new_element_num, lang_string, callback_function) */
LCDMenuLib_add(0 , root         , 1  , "Aktuelle Position"  , FUNC_aktuellPos);
LCDMenuLib_add(1 , root         , 2  , "Position laden"     , FUNC_laden);
LCDMenuLib_add(2 , root         , 3  , "Pos. einstellen"    , FUNC_einstellen);
LCDMenuLib_add(3 , root         , 4  , "Pos. speichern"     , FUNC_speichern);
LCDMenuLib_add(4 , root         , 5  , "Zeige Schnurl\341nge", FUNC_zeigenSchnur);
LCDMenuLib_add(5 , root         , 6  , "Status"             , FUNC_status);
LCDMenuLib_add(6 , root         , 7  , "Motor Stop !!"      , FUNC_motorStop);
LCDMenuLib_add(7 , root         , 8  , "Admin Funktionen"   , FUNC_Admin);
LCDMenuLib_add(8 , root_8       , 1  , "Abweichung Schnur" , FUNC_abweichnung);
LCDMenuLib_add(9 , root_8       , 2  , "Drehrichtung"      , FUNC_drehrichtung);
LCDMenuLib_add(10, root_8       , 3  , "Aktionsbereich"    , FUNC_bereich);
LCDMenuLib_add(11, root_8       , 4  , "MinMax H\357he"    , FUNC_MinMax);
LCDMenuLib_add(12, root_8       , 5  , "Spulen"            , FUNC_Spule);
LCDMenuLib_add(13, root_8       , 6  , "Motoren"           , FUNC_motoren);
LCDMenuLib_add(14, root_8       , 7  , "Speicher auslesen" , FUNC_speicherLesen);
LCDMenuLib_createMenu(_LCDMenuLib_cnt);

float SPULEv, SPULEh;   // durchmesser Spule in mm
unsigned long gesamtbreite, gesamttiefe, gesamthoehe, maxhoehe, minhoehe, maxtiefe, mintiefe; // Geometrie Grenzen in mm

unsigned long motorSpeed;
unsigned long currfrontline_steps, currfrontline_mm, currbackline_steps, currbackline_mm; // Schritte
unsigned long targetfrontline_steps, targetfrontline_mm, targetbackline_steps, targetbackline_mm; // Schritte
unsigned long hoehe_mm, tiefe_mm; // Millimeter
unsigned long adminTimer, buttonClickTimer;
int fadeValue;
byte fadeState = true;

byte menu = 99;
byte slot; 
byte blinkState = LOW;
byte cursorOn = false;
byte busAction;
byte gang;
byte ratio;
byte hardwareLock;
byte demo;

byte state = 0;
// Mögliche Werte für State
// 0 = Initialisierung
// 1 = Neue Werte erhalten
// 2 = Motoren laufen
// 3 = Zielwert erreicht, nachschwingen abwarten, speichern
// 4 = Display nach 5 Sekundenaussschalten

void setup()
{ 
  pinMode(A2, INPUT); digitalWrite(A2, HIGH); // Spannung für die Abfrage ob Mischpult aus
  while(!digitalRead(A2)); // nicht aufwachen wenn Mischpult aus
  LCDMenuLib_setup(_LCDMenuLib_cnt);  /* Setup for LcdMenuLib */
  Wire.begin(); // start up i2c bus
  TWBR = 158; TWSR |= bit(TWPS0); // slow down bus clock
  LCDMenuLib_IS_startDirect();
  adminTimer = 300000; // vorsprung für Timer damit keine Admin Rechte
  pinMode(10, OUTPUT);// LCD Beleuchtung
  digitalWrite(10, HIGH); // Beleuchtung ein
  if (load(120) == -1 ) newHardware(); //EEPROM ist leer ... Standartwerte laden
  if(analogRead(A3) < 10) { // Demo Modus ohne überlagerte Fehlermeldung
    lcd.clear();
    lcd.setCursor( 5, 1);
    lcd.print(F("DEMO !!"));
    delay(1000);
    demo = true;
    FUNC_back();
  }
}

void loop() {
  static unsigned long runMillis, sleepMillis, i2cMillis, previousCursorMillis; // Timer Variablen
  LCDMenuLib_control_analog();        /* lcd menu control - config in tab "LCDML_control" */ 
  LCDMenuLib_loop();                  /* lcd function call */
  LCDbackground();                    /* lcd background light */

  if(millis() - previousCursorMillis >= 750) { // Blinkender Cursor wenn nicht im Menü
    previousCursorMillis = millis();
    if (blinkState == LOW) {
      blinkState = HIGH;
      if (menu != 99 && cursorOn == true) lcd.cursor();
    } else {
      if (menu != 99 &&  cursorOn) { blinkState = LOW; lcd.noCursor(); }
      if (menu == 99) lcd.noCursor();
    }   
  }

  if (state == 0) { // initialisierung (siehe auch Funktion newHardware())
    checkError();
    hoehe_mm = load(0);
    tiefe_mm = load(0+128);
    gesamtbreite = load(101);
    gesamttiefe  = load(102);
    gesamthoehe  = load(103);
    maxhoehe = load(104);
    minhoehe = load(105);
    maxtiefe  = load(106);
    mintiefe  = load(107);

    slot = load(120);
    motorSpeed = load(108);
    SPULEv = load(109) * 0.001;
    SPULEh = load(110) * 0.001;
    sendLong(motorSpeed, 0, 9);
    ratio = EEPROM.read(1022);
      Wire.beginTransmission(0); //sende Ratio
      Wire.write(11);
      Wire.write(ratio);
      Wire.endTransmission();
    gang = EEPROM.read(1023);
      Wire.beginTransmission(0); //sende Gang
      Wire.write(8);
      Wire.write(gang);
      Wire.endTransmission();
    //state = 1;
  }

  if (state == 1) { // Neue Werte erhalten
      checkError();
      CoordinateTOline(hoehe_mm, tiefe_mm); //Zielwerte berechnen
      sendTargetValue(); // Einstellungen via broadcasting versenden
      state = 2;
  }

  if(state == 2) sleepMillis = millis(); // motoren laufen
  
  if(millis() >= runMillis && state == 2 ) {
    runMillis = millis() + 1000; // 1 mal pro Sekunde
    getCurrent(11); // Hole aktuelle Schnurlänge vorne + hinten
    if (currfrontline_steps == targetfrontline_steps) {
      getCurrent(12);
      if (currfrontline_steps == targetfrontline_steps) {
        state = 3;
        }
      }
    }
  
  if(millis() >= i2cMillis) { // && state != 2
    i2cMillis = millis() + 1000; // 1 mal pro Sekunde
    checkError();
   }
  
  if(state == 3 && millis() - sleepMillis >= 1000) { // abwarten nach Position erreicht - fertig
    if (hoehe_mm != load(0    )) save(0,     hoehe_mm); // Aktuelle Position in EEPROM speichern
    if (tiefe_mm != load(0+128)) save(0+128, tiefe_mm); // Aktuelle Position in EEPROM speichern
    checkError();
    state = 4;
  }
  
  if(state != 2 ) { // Motoren stehen
    if( millis() - buttonClickTimer >= 5000 && fadeState) fadeState = false; // Keine Aktionen --> Licht aus
    if(fadeValue == 0) lcd.clear(); // Licht ist komplett aus. Display löschen.
  }
  
  if(!digitalRead(A2) && hardwareLock == 1) { // Mischpult ausgeschaltet. Mikrofone anheben bzw. Position 100 laden.
    long temp_hoehe, temp_tiefe;
    checkError();
    hardwareLock = 0;
    // Tiefe nicht ändern
    temp_hoehe = load(100);
    if (temp_hoehe == -1) temp_hoehe = 3000;
    if (temp_hoehe > maxhoehe) temp_hoehe = maxhoehe;
    if (temp_hoehe < minhoehe) temp_hoehe = minhoehe;
    hoehe_mm = temp_hoehe;
    state = 1;
    fadeState = true;
  }
  
  if(state == 4 && fadeValue == 0) {
    while(!digitalRead(A2)); // nicht wieder aufwachen wenn Mischpult aus
    checkError();
  }
  
  if(digitalRead(A2) && hardwareLock == 0) { // Mischpult eingeschaltet.. letzten Slot laden
    long temp_hoehe, temp_tiefe;
    checkError();
    hardwareLock = 1;
    slot = load(120);
    temp_hoehe = load(slot);
    temp_tiefe = load(slot+128);
    if (temp_hoehe == -1) temp_hoehe = 2200;
    if (temp_hoehe > maxhoehe) temp_hoehe = maxhoehe;
    if (temp_hoehe < minhoehe) temp_hoehe = minhoehe;
    if (temp_tiefe == -1) temp_tiefe = 5000;
    if (temp_tiefe > gesamttiefe) temp_tiefe = gesamttiefe;
    hoehe_mm = temp_hoehe;
    tiefe_mm = temp_tiefe;
    state = 1;
    FUNC_back(); // Menü neu aufbauen.
    fadeState = true;
  }
}

void checkError() { //selbstdiagnose
  byte fehler;
  int stat1, stat3, span1, span3;
  static unsigned long errorMillis;
  if (demo == false) { //keine Fehler im Demo prüfen.
    stat1 = motorstatus(1);
    stat3 = motorstatus(3);
    span1 = spannung(1);
    span3 = spannung(3);
    if (stat1 == '#' || stat3 == '#' || span1 < 8 || span3 < 8) { // Angabe Spannung in Volt
      fehler = true;
      lcd.noCursor();
      lcd.clear();
      lcd.setCursor(0,0); lcd.print(F("Fehler!!!"));
      lcd.setCursor(8,1); lcd.print(F("Links Rechts"));
      digitalWrite(10, HIGH);
      while(fehler) {
        if (stat1 != '#' && stat3 != '#' && span1 >= 9 && span3 >= 9) { fehler = false; FUNC_back(); } // Fehler behoben
        if (millis() >= errorMillis && fehler == true) {
          Wire.beginTransmission(0); Wire.write(0); Wire.endTransmission(); // alle noch erreichbaren Motoren anhalten
          errorMillis = millis() + 100;
          stat1 = motorstatus(1);
          stat3 = motorstatus(3);
          span1 = spannung(1);
          span3 = spannung(3);
          lcd.setCursor(20,0); lcd.print(F("Verbi.: "));
            if (stat1 == '#') { lcd.print(F("Ni OK ")); } else {lcd.print(F(" OK   ")); }
            if (stat3 == '#') { lcd.print(F("Ni OK ")); } else {lcd.print(F(" OK   ")); }
          lcd.setCursor(20,1); lcd.print(F("Spann.: "));
            if (span1  <  9 ) { lcd.print(F("Ni OK " )); } else {lcd.print(F(" OK   " )); }
            if (span3  <  9 ) { lcd.print(F("Ni OK " )); } else {lcd.print(F(" OK   " )); }
        }
      }
    }
  } else {
    Wire.beginTransmission(0); Wire.write(0); Wire.endTransmission(); // Motoren im Demo Modus permanent Stop Signal schicken.
    if (millis() > 60000) demo = false; // Demo Modus nach einer Minuten verlassen.
  }
}
