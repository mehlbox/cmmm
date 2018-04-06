void checkLimits(unsigned long* tempHeight, unsigned long* tempDepth) {
  if (*tempHeight == -1) *tempHeight = range_y_max / 2;
  if (*tempDepth == -1) *tempDepth = range_x_max / 2;

  if (*tempHeight > range_y_max) *tempHeight = range_y_max;
  if (*tempHeight < range_y_min) *tempHeight = range_y_min;

  if (*tempDepth > range_x_max) *tempDepth = range_x_max;
  if (*tempDepth < range_x_min) *tempDepth = range_x_min;
}

int caladdr(byte motor) { // Adresse berechnen
  int value = 99;
  if ( motor == 0) value = 0; // broadcast
  if ( motor == 1) value = 11; // left
  if ( motor == 2) value = 11; // left
  if ( motor == 3) value = 12; // right
  if ( motor == 4) value = 12; // right
  return value;
}

byte calmotor(byte motor) { // Motor berechnen
  byte value = 99;
  if ( motor == 1) value = 1; // front
  if ( motor == 2) value = 2; // back
  if ( motor == 3) value = 1; // front
  if ( motor == 4) value = 2; // back
  return value;
}

unsigned long mmTOsteps(float mm, byte voh) { // umrechnung //vorne oder hinten
  static float x, SPULE;
  if (voh) SPULE = spool_front; else SPULE = spool_back;
  x = mm / (SPULE/1000 * 3.141592 / 200) * gearRatio * stepping;
  if (isnan(x)) x = 0; // Not a number
  x = x + 0.5; // weil bei float in long immer abgerundet wird
  return x;
}

unsigned long stepsTOmm(unsigned long steps, byte voh) { // umrechnung
  static float x, SPULE;
  if (voh) SPULE = spool_front; else SPULE = spool_back;
  x = steps * SPULE/1000 * 3.141592 / 200 / gearRatio / stepping;
  if (isnan(x)) x = 0; // Not a number
  x = x + 0.5; // weil bei float in long immer abgerundet wird
  return x;
}

long stepsTOmmS(long steps, byte voh) { // umrechnung SIGNED (minus)
  long x, SPULE;
  if (voh) SPULE = spool_front; else SPULE = spool_back;
  x = steps * SPULE/1000 * 3.141592 / 200 / gearRatio / stepping;
  if (isnan(x)) x = 0; // Not a number
  //x = x + 0.5; // weil bei float in long immer abgerundet wird
  return x;
}

int expo(int m, int n) {  // berechnet m hoch n
  static int i, x;
  if (n == 0) x = 1; else x = m;
  for (i = 1; i < n; i++) x = x * m;
  return x;
}


//  ###################################################################################################
//  # Berechnungen werden in float durchgeführt und danach durch einsetzen in long Variable gerundet! #
//  ###################################################################################################

void CoordinateTOline(float F_height_mm, float F_depth_mm) { // Umrechnung von Koordinaten auf Schnurlänge in mm (Zielwerte)
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
  static float a, b, c, d, h, w;
  w = dimension_z * 0.5;
  h = dimension_y - F_height_mm;  // Abstand vom Boden
  a = dimension_x - F_depth_mm;   // Abstand von der Wand
  c = sqrt(sq(a)+sq(h)+sq(w));
  targetbackline_mm = c + 0.5; // weil bei float in long immer abgerundet wird
  b = dimension_x - a;
  d = sqrt(sq(b)+sq(h)+sq(w)); // Inhalt der Wurzel
  targetfrontline_mm = d + 0.5; // weil bei float in long immer abgerundet wird

  targetfrontline_steps = mmTOsteps(targetfrontline_mm,1);
  targetbackline_steps  = mmTOsteps(targetbackline_mm,0);
}

// 200 Schritte pro Umdrehung
// gearRatio = 27 Getriebe (untersetzung) auf Motor
// SPULE Durchmesser
