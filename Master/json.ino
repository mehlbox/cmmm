
void jsonWriteMemory(String POST_req) {
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(POST_req);
  if(root.success()) {
    slot = root["slot"];
    save_x_mm = root["x"];
    save_y_mm = root["y"];
  } else {
    if (DEBUG) Serial.println("parse json object failed");
  }
}

void jsonTarget(String POST_req) {
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(POST_req);
  if(root.success()) {
    set_x_mm = root["x"];
    set_y_mm = root["y"];
    state = 1; // start
  } else {
    if (DEBUG) Serial.println("parse json object failed");
  }
}

void jsonArea(String POST_req) {
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(POST_req);
  if(root.success()) {
    dimension_x  = root["dimension"]["x"];
    dimension_y  = root["dimension"]["y"];
    dimension_z  = root["dimension"]["z"];
    range_x_min  = root["range"]["x"]["min"];
    range_x_max  = root["range"]["x"]["max"];
    range_y_min  = root["range"]["y"]["min"];
    range_y_max  = root["range"]["y"]["max"];
  } else {
    if (DEBUG) Serial.println("parse json object failed");
  }
}

void jsonOption(String POST_req) {
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(POST_req);
  if(root.success()) {
    spool_front     = root["spool"]["front"];
    spool_back      = root["spool"]["back"];
    motorSpeedLimit = root["motorSpeedLimit"];
    stepping        = root["stepping"];
    gearRatio       = root["gearRatio"];
  } else {
    if (DEBUG) Serial.println("parse json object failed");
  }
}

byte jsonInvert(String POST_req) {
  byte invertMotor;
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(POST_req);
  if(root.success()) {
    invertMotor = root["invertMotor"]; // ob das geht????
    return invertMotor;
  } else {
    if (DEBUG) Serial.println("parse json object failed");
    return 99;
  }
}

void jsonCorrection(String POST_req) {
  byte motor;
  unsigned long data;
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(POST_req);
  if(root.success()) {
    motor = root["motor"];
    data  = root["data"];
    sendLong(data, motor, 7);
  } else {
    if (DEBUG) Serial.println("parse json object failed");
  }
}
