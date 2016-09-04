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

int expo(int m, int n) {  // berechnet m hoch n
  int i, x;
  if (n == 0) x = 1; else x = m;
  for (i = 1; i < n; i++) x = x * m;
  return x;
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