void network() {
  char char_in = 0;    // used to store a character from the Ethernet port
  String HTTP_req, POST_req;     // stores the received HTTP request
  EthernetClient client = server.available();
  if (client) {
    HTTP_req = ""; // delete string contents
    while (client.connected()) {
      if (client.available()) {
        char_in = client.read();  // get a single character from the Ethernet port
        HTTP_req += char_in;      // build a string out of the received characters
        if (char_in == '\n') {    // end of HTTP request reached
          Serial.print(millis()/1000); Serial.print(" "); Serial.print(HTTP_req);
          if (HTTP_req.startsWith("POST")) {
            byte n_count = 0; //counter for new line in row. two lines in a row will have the value of 3 in n_count
            while(client.available() && n_count != 3) { // skip until POST data starts
              char_in = client.read();
              if (char_in == '\n') { n_count=n_count+2; } else { n_count--; if (n_count == 255) n_count = 0; }
            }
            while(client.available()) { char_in = client.read(); POST_req += char_in; } // write POST data down
            //if (DEBUG) Serial.println(POST_req);
            }
            // respond to HTTP request
            client.write("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept\r\nConnection: close\r\nContent-Type: application/json;charset=utf-8\r\n\r\n");
            if (HTTP_req.startsWith("GET / ") ) {
              client.write("cmmm2\r\nnetwork connected");

            } else if (HTTP_req.startsWith("GET /i2cScan")) { // i2c scan
              i2cScan(&client);

            } else if (HTTP_req.startsWith("GET /readMemory")) {
              byte forSlot;
              unsigned long tempX;
              unsigned long tempY;
              client.write("{");
              for (forSlot = 0; forSlot <= 100; forSlot ++) {
                tempX = load(forSlot+128);
                tempY = load(forSlot);
                if (tempX != -1) {
                  client.write("\"");client.print(forSlot);
                  client.write("\":{\"x\":");client.print(tempX);
                  client.write(",\"y\":");client.print(tempY);
                  client.write("},");
                }
              }
              client.write("\"slot\":");client.print(slot);client.write("}"); // add current slot to 101

            } else if (HTTP_req.startsWith("GET /factorydefault")) { // factorydefault
              newHardware();
              client.write("default settings loaded. restarting...");
              client.stop();
              wdt_enable(WDTO_15MS); // restart
              while(true);

            } else if (HTTP_req.startsWith("GET /clearEEPROM")) { // factorydefault
              clearEEPROM();
              client.write("EEPROM is now clear. restarting...");
              client.stop();
              wdt_enable(WDTO_15MS); // restart
              while(true);

            } else if (HTTP_req.startsWith("GET /restart")) { // restart
              client.write("restarting...");
              client.stop();
              wdt_enable(WDTO_15MS); // restart
              while(true);

            } else if (HTTP_req.startsWith("GET /loop")) {
              client.write("{\"current\":{\"lengh\":{\"front\":");client.print(currfrontline_mm);
              client.write(",\"back\":");client.print(currbackline_mm);
              client.write("}},\"target\":{\"lengh\":{\"front\":");client.print(targetfrontline_mm);
              client.write(",\"back\":");client.print(targetbackline_mm);
              client.write("}},\"speed\":{\"lengh\":{\"front\":");client.print(stepsTOmmS(speedfrontline_steps,1));
              client.write(",\"back\":");client.print(stepsTOmmS(speedbackline_steps, 0));
              client.write("}}}");

            } else if (HTTP_req.startsWith("GET /currentLine")) {
              client.write("{\"current\":{\"lengh\":{\"front\":");client.print(currfrontline_mm);
              client.write(",\"back\":");client.print(currbackline_mm);
              client.write("},\"steps\":{\"front\":");client.print(currfrontline_steps);
              client.write(",\"back\":");client.print(currbackline_steps);
              client.write("}}}");

            } else if (HTTP_req.startsWith("GET /area")) {
              client.write("{\"dimension\":{\"z\":");client.print(dimension_z);
              client.write(",\"x\":");client.print(dimension_x);
              client.write(",\"y\":");client.print(dimension_y);
              client.write("},\"range\":{\"x\":{\"min\":");client.print(range_x_min);
              client.write(",\"max\":");client.print(range_x_max);
              client.write("},\"y\":{\"min\":");client.print(range_y_min);
              client.write(",\"max\":");client.print(range_y_max);
              client.write("}}}");

            } else if (HTTP_req.startsWith("GET /option")) {
              client.write("{\"spool\":{\"front\":");client.print(spool_front);
              client.write(",\"back\":");client.print(spool_back);
              client.write("},\"motorSpeedLimit\":");client.print(motorSpeedLimit);
              client.write(",\"gearRatio\":");client.print(EEPROM.read(1022));
              client.write(",\"stepping\":");client.print(EEPROM.read(1023));
              client.write("}");

            } else if (HTTP_req.startsWith("GET /all")) {
              client.write("{\"area\":{\"dimension\":{\"z\":");client.print(dimension_z);
              client.write(",\"x\":");client.print(dimension_x);
              client.write(",\"y\":");client.print(dimension_y);
              client.write("},\"range\":{\"x\":{\"min\":");client.print(range_x_min);
              client.write(",\"max\":");client.print(range_x_max);
              client.write("},\"y\":{\"min\":");client.print(range_y_min);
              client.write(",\"max\":");client.print(range_y_max);
              client.write("}}},");
              client.write("\"option\":{\"spool\":{\"front\":");client.print(spool_front);
              client.write(",\"back\":");client.print(spool_back);
              client.write("},\"motorSpeedLimit\":");client.print(motorSpeedLimit);
              client.write(",\"gearRatio\":");client.print(EEPROM.read(1022));
              client.write(",\"stepping\":");client.print(EEPROM.read(1023));
              client.write("},");
              client.write("\"line\":{\"current\":{\"lengh\":{\"front\":");client.print(currfrontline_mm);
              client.write(",\"back\":");client.print(currbackline_mm);
              client.write("}},\"target\":{\"lengh\":{\"front\":");client.print(targetfrontline_mm);
              client.write(",\"back\":");client.print(targetbackline_mm);
              client.write("}},\"speed\":{\"lengh\":{\"front\":");client.print(stepsTOmmS(speedfrontline_steps,1));
              client.write(",\"back\":");client.print(stepsTOmmS(speedbackline_steps, 0));
              client.write("}}}}");

            } else if (HTTP_req.startsWith("POST /target")) {
              jsonTarget(POST_req);
              client.write("{\"dimension\":{\"x\":");client.print(dimension_x);
              client.write(",\"y\":");client.print(dimension_y);
              client.write(",\"z\":");client.print(dimension_z);
              client.write("},\"range\":{\"x\":{\"min\":");client.print(range_x_min);
              client.write(",\"max\":");client.print(range_x_max);
              client.write("},\"y\":{\"min\":");client.print(range_y_min);
              client.write(",\"max\":");client.print(range_y_max);
              client.write("}}}");

            } else if (HTTP_req.startsWith("POST /writeMemory")) {
              jsonWriteMemory(POST_req);
              save(slot, save_y_mm);
              save(slot+128, save_x_mm);

            } else if (HTTP_req.startsWith("POST /correction")) {
              jsonCorrection(POST_req);

            } else if (HTTP_req.startsWith("POST /area")) {
              jsonArea(POST_req);
              save(101, dimension_x);
              save(102, dimension_y);
              save(103, dimension_z);
              save(104,     range_x_min);
              save(104+128, range_x_max);
              save(105,     range_y_min);
              save(105+128, range_y_max);

            } else if (HTTP_req.startsWith("POST /option")) {
              jsonOption(POST_req);
              save(108, motorSpeedLimit);
              save(109, spool_front);
              save(110, spool_back);
              EEPROM.write(1022, gearRatio);
              EEPROM.write(1023, stepping);

            } else if (HTTP_req.startsWith("POST /invert1")) {
              invertMotor(1);

            } else if (HTTP_req.startsWith("POST /invert2")) {
              invertMotor(2);

            } else if (HTTP_req.startsWith("POST /invert3")) {
              invertMotor(3);

            } else if (HTTP_req.startsWith("POST /invert4")) {
              invertMotor(4);
            }
            //delay(1);
          client.stop();
        }
      }
    }
  }
}
