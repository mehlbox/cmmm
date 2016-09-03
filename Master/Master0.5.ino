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

/* settings for lcd */
#define _LCDMenuLib_LCD_cols             20
#define _LCDMenuLib_LCD_rows             4

/* pin settings */  
#define _LCDMenuLib_LCD_rs               2
#define _LCDMenuLib_LCD_e                3
#define _LCDMenuLib_LCD_dat0             6
#define _LCDMenuLib_LCD_dat1             7
#define _LCDMenuLib_LCD_dat2             8
#define _LCDMenuLib_LCD_dat3             9 

/* lib config */
#define _LCDMenuLib_cfg_initscreen       0      /* 0=disable, 1=enable */
#define _LCDMenuLib_cfg_initscreen_time  5000   /* 0=time disabled, >0 wait time in ms */
#define _LCDMenuLib_cfg_scrollbar	 1      	/* 0=no scrollbar, 1=complex scrollbar, 2=easy scrollbar */
#define _LCDMenuLib_cfg_lcd_standard	 0      /* 0=HD44780 standard / 1=HD44780U standard */
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
unsigned long gesamtbreite, gesamttiefe, gesamthoehe, maxhoehe, minhoehe; // Geometrie Grenzen in mm

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
byte pultState;
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
  pinMode(A2, INPUT); digitalWrite(A2, HIGH); // für die Abfrage ob Mischpult aus
  while(!digitalRead(A2)); // nicht aufwachen wenn Mischpult aus
  LCDMenuLib_setup(_LCDMenuLib_cnt);  /* Setup for LcdMenuLib */
  Wire.begin(); // start up i2c bus
  TWBR = 158; TWSR |= bit(TWPS0); // slow down bus clock
  LCDMenuLib_IS_startDirect();
  adminTimer = 300000; // vorsprung für Timer damit keine Admin Rechte
  pinMode(10, OUTPUT);// LCD Beleuchtung
  digitalWrite(10, HIGH); // Beleuchtung ein
  if (load(106) == -1 ) newHardware(); //EEPROM ist leer ... Standartwerte laden
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

    slot = load(107);
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
  
  if(!digitalRead(A2) && pultState == 1) { // Mischpult ausgeschaltet. Mikrofone anheben.
    pultState = 0;
    hoehe_mm = hoehe_mm + 1000;
    if (hoehe_mm == -1) hoehe_mm = 2000;
    if (hoehe_mm > maxhoehe) hoehe_mm = maxhoehe;
    if (hoehe_mm < minhoehe) hoehe_mm = minhoehe;
    checkError();
    state = 1;
    fadeState = true;
  }
  
  if(state == 4 && fadeValue == 0) {
    while(!digitalRead(A2)); // nicht wieder aufwachen wenn Mischpult aus
    checkError();
  }
  
  if(digitalRead(A2) && pultState == 0) { // Mischpult eingeschaltet.. letzten Slot laden
    long temp_hoehe, temp_tiefe;
    checkError();
    pultState = 1;
    slot = load(107);
    temp_hoehe = load(slot);
    temp_tiefe = load(slot+128);
    if (temp_hoehe == -1) temp_hoehe = 2500;
    if (temp_hoehe > maxhoehe) temp_hoehe = maxhoehe;
    if (temp_hoehe < minhoehe) temp_hoehe = minhoehe;
    if (temp_tiefe == -1) temp_tiefe = 0;
    if (temp_tiefe > gesamttiefe) temp_tiefe = gesamttiefe;
    hoehe_mm = temp_hoehe;
    tiefe_mm = temp_tiefe;
    state = 1;
    fadeState = true;
  }
}

char motorstatus(int motor) { // Motor Statusabfrage
  char stat;
  int addr;
    addr = caladdr(motor);
    motor = calmotor(motor);
    busAction = 6;
    Wire.beginTransmission(addr);
    Wire.write(busAction);
    Wire.write(motor);
    Wire.endTransmission();
    Wire.requestFrom(addr, 1);
    switch (Wire.read()) {
      case -1:
        stat = '#'; // Keine Verbindung
        break;
      case 1:
        stat = '+'; // Motor dreht vorwärts
        break;
      case 2:
        stat = '-'; // Motor dreht rückwärts
        break;
      case 3:
        stat = 'O'; // Motor steht
        break;
      }
    return stat;
}

float spannung(int motor) { // Motor Statusabfrage
    char stat;
    int addr;
    long longVoltage;
    addr = caladdr(motor);
    busAction = 12;
    Wire.beginTransmission(addr);
    Wire.write(busAction);
    Wire.endTransmission();
    Wire.requestFrom(addr, 4);
    unsigned long mem[4];
    mem[0] = Wire.read();
    mem[1] = Wire.read(); mem[1] =  mem[1] << 8;
    mem[2] = Wire.read(); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
    mem[3] = Wire.read(); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
    longVoltage = mem[0] |= mem[1] |= mem[2] |= mem[3];
    longVoltage = longVoltage * 0.000001;
    if (longVoltage > 2) longVoltage = longVoltage + 0.45; // 
    return longVoltage;
}

void getCurrent(int addr) { // Aktuelle Werte holen via I2C
  if (busAction != 14 && busAction != 4) {
    busAction = 14;
    Wire.beginTransmission(0);
    Wire.write(busAction);
    Wire.endTransmission();
  }
  Wire.requestFrom(addr, 8);
  unsigned long mem[8];
  mem[0] = Wire.read();
  mem[1] = Wire.read(); mem[1] =  mem[1] << 8;
  mem[2] = Wire.read(); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
  mem[3] = Wire.read(); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
  currfrontline_steps = mem[0] |= mem[1] |= mem[2] |= mem[3];
  mem[4] = Wire.read();
  mem[5] = Wire.read(); mem[5] =  mem[5] << 8;
  mem[6] = Wire.read(); mem[6] =  mem[6] << 8; mem[6] =  mem[6] << 8;
  mem[7] = Wire.read(); mem[7] =  mem[7] << 8; mem[7] =  mem[7] << 8; mem[7] =  mem[7] << 8;
  currbackline_steps = mem[4] |= mem[5] |= mem[6] |= mem[7];
  currfrontline_mm = stepsTOmm(currfrontline_steps,1);
  currbackline_mm  = stepsTOmm(currbackline_steps,0);
}

unsigned long getlong(byte motor, byte action) { // Long Wert holen via I2C
  busAction = action;
  Wire.beginTransmission(caladdr(motor));
  Wire.write(action);
  Wire.write(calmotor(motor));
  Wire.endTransmission();
  Wire.requestFrom(caladdr(motor), 4);
  long mem[4];
  long data;
  mem[0] = Wire.read();
  mem[1] = Wire.read(); mem[1] =  mem[1] << 8;
  mem[2] = Wire.read(); mem[2] =  mem[2] << 8; mem[2] =  mem[2] << 8;
  mem[3] = Wire.read(); mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8; mem[3] =  mem[3] << 8;
  data = mem[0] |= mem[1] |= mem[2] |= mem[3];
  return data;
}

void sendTargetValue() { // Zielwerte senden
    busAction = 3;
    byte data[9]; 
    data[0]= busAction; // action
    data[1]=(targetfrontline_steps      ) & 0xFF; // vordere Schnur Zielwert
    data[2]=(targetfrontline_steps >>  8) & 0xFF;
    data[3]=(targetfrontline_steps >> 16) & 0xFF;
    data[4]=(targetfrontline_steps >> 24) & 0xFF;
    data[5]=(targetbackline_steps      ) & 0xFF; // hintere Schnur Zielwert
    data[6]=(targetbackline_steps >>  8) & 0xFF;
    data[7]=(targetbackline_steps >> 16) & 0xFF;
    data[8]=(targetbackline_steps >> 24) & 0xFF;
    Wire.beginTransmission(0); //broadcast
    Wire.write(data,9);
    Wire.endTransmission();
}

void sendLong(long senddata, byte motor, byte action) { // Long Wert senden
    busAction = action;
    byte data[6];
    data[0]=action;
    data[1]=calmotor(motor);
    data[2]=(senddata      ) & 0xFF;
    data[3]=(senddata >>  8) & 0xFF;
    data[4]=(senddata >> 16) & 0xFF;
    data[5]=(senddata >> 24) & 0xFF;
    Wire.beginTransmission(caladdr(motor));
    Wire.write(data,6);
    Wire.endTransmission();
}

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

int caladdr(byte motor) { // Adresse berechnen
  if ( motor == 0) return 0; // broadcast
  if ( motor == 1) return 11;
  if ( motor == 2) return 11;
  if ( motor == 3) return 12;
  if ( motor == 4) return 12;
}

byte calmotor(byte motor) { // Motor berechnen
  if ( motor == 1) return 1;
  if ( motor == 2) return 2;
  if ( motor == 3) return 1;
  if ( motor == 4) return 2;
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

int lcdspace(long x) { // Anzahl Leerzeichen vor Zahlen
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

int lcdmspace(long x) { // Anzahl Leerzeichen vor kleinen Zahlen
  int space = 0;
    if( abs(x) < 10) space++;
    if( abs(x) < 100) space++;
    for (int n = 1; n <= space; n++) lcd.print(F(" "));
    lcd.print(x);
}

//  ###################################################################################################
//  # Berechnungen werden in float durchgeführt und danach durch einsetzen in long Variable gerundet! #
//  ###################################################################################################

unsigned long CoordinateTOline(float F_hoehe_mm, float F_tiefe_mm) { // Umrechnung von Koordinaten auf Schnurlänge in mm (Zielwerte)
//
//      a       b(tiefe)
//   +-----+---------------+
//    \    |            /
//     \   |h        /
//    c \  |      / d
//       \ |   /
//         + 
//
//--------------------------+    Kante Bühne = nullpunkt
//                          |
//   Wand   --> //
//   Schnur --> c und d
//   Decke  --> a und b
  float a, b, c, d, h, w;
  static unsigned long alt;
  w = gesamtbreite * 0.5; 
  h = gesamthoehe - F_hoehe_mm;   // Abstand vom Boden
  a = gesamttiefe - F_tiefe_mm;   // Abstand von der Wand
  c = sqrt(sq(a)+sq(h)+sq(w));
  targetbackline_mm = c + 0.5; // weil bei float in long immer abgerundet wird
  b = gesamttiefe - a;
  d = sqrt(sq(b)+sq(h)+sq(w)); // Inhalt der Wurzel
  targetfrontline_mm = d + 0.5; // weil bei float in long immer abgerundet wird
  
  targetfrontline_steps = mmTOsteps(targetfrontline_mm,1);
  targetbackline_steps  = mmTOsteps(targetbackline_mm,0);
}


// 200 Schritte pro Umdrehung
// ratio = 27 Getriebe (untersetzung) auf Motor
// SPULE Durchmesser

unsigned long mmTOsteps(float mm, byte voh) { // umrechnung //vorne oder hinten
  float x, SPULE;
  if (voh) SPULE = SPULEv; else SPULE = SPULEh;
  x = mm / (SPULE * 3.141592 / 200) * ratio * gang;
  if (isnan(x)) x = 0; // Not a number
  x = x + 0.5; // weil bei float in long immer abgerundet wird
  return x;
}

unsigned long stepsTOmm(unsigned long steps, byte voh) { // umrechnung
  float x, SPULE;
  if (voh) SPULE = SPULEv; else SPULE = SPULEh;
  x = steps * (SPULE * 3.141592 / 200) / ratio / gang;
  if (isnan(x)) x = 0; // Not a number
  x = x + 0.5; // weil bei float in long immer abgerundet wird
  return x;
}

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
  save(0,  2500);// Aktuelle Position
  save(128,   0);// Aktuelle Position
  save(101, 0);    // gesamtbreite
  save(102, 8000); // gesamttiefe
  save(103, 5800); // gesamthoehe
  save(104, 4000); // maxhoehe
  save(105, 1000); // minhoehe
  save(106, 29500);// Spulendurchmesser-alt / trotzdem noch newHardware Erkennung
  save(107, 1);    // Aktuelle gewählter Slot
  save(108, 1000); // Geschwindigkeit
  save(109, 29500);// Spulendurchmesser vorne
  save(110,170000);// Spulendurchmesser hinten
  EEPROM.write(1022,27); // Ratio. Teil von Slot 255
  EEPROM.write(1023,1); // Gang.  Teil von Slot 255
}

int expo(int m, int n) {  // berechnet m hoch n
  int i, x;
  if (n == 0) x = 1; else x = m;
  for (i = 1; i < n; i++) x = x * m;
  return x;
}

void checkError() { //selbstdiagnose
  byte fehler;
  int stat1, stat3, span1, span3;
  static unsigned long errorMillis;
  if (demo == false) {
    stat1 = motorstatus(1);
    stat3 = motorstatus(3);
    span1 = spannung(1);
    span3 = spannung(3);
    if (stat1 == '#' || stat3 == '#' || span1 < 8 || span3 < 8) {
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
    Wire.beginTransmission(0); Wire.write(0); Wire.endTransmission(); // Motoren im Demo Modus permanent stoppen.
    if (millis() > 60000) demo = false; // Demo Modus nach einer Minuten verlassen.
  }
}
