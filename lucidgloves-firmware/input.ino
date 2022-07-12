int maxFingers[5] = {0,0,0,0,0};
int minFingers[5] = {ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX, ANALOG_MAX};

void setupInputs(){
  pinMode(PIN_JOY_BTN, INPUT_PULLUP);
  pinMode(PIN_A_BTN, INPUT_PULLUP);
  pinMode(PIN_B_BTN, INPUT_PULLUP);

  #if !TRIGGER_GESTURE
  pinMode(PIN_TRIG_BTN, INPUT_PULLUP);
  #endif

  #if !GRAB_GESTURE
  pinMode(PIN_GRAB_BTN, INPUT_PULLUP);
  #endif

  #if !PINCH_GESTURE
  pinMode(PIN_PNCH_BTN, INPUT_PULLUP);
  #endif

  #if USING_CALIB_PIN
  pinMode(PIN_CALIB, INPUT_PULLUP);
  #endif
}

int* getFingerPositions(bool calibrating, bool reset){
  int rawFingers[5] = {analogRead(PIN_PINKY), analogRead(PIN_RING), analogRead(PIN_MIDDLE), analogRead(PIN_INDEX), NO_THUMB?0:analogRead(PIN_THUMB)};
  
  //flip pot values if needed
  #if FLIP_POTS
  for (int i = 0; i <5; i++){
    rawFingers[i] = ANALOG_MAX - rawFingers[i];
  }
  #endif

  //reset max and mins as needed
  if (reset){
    for (int i = 0; i <5; i++){
      maxFingers[i] = 0;
      minFingers[i] = ANALOG_MAX;
    }
  }
  
  //if during the calibration sequence, make sure to update max and mins
  if (calibrating){
    for (int i = 0; i <5; i++){
      if (rawFingers[i] > maxFingers[i])
        maxFingers[i] = rawFingers[i];
      if (rawFingers[i] < minFingers[i])
        minFingers[i] = rawFingers[i];
    }
  }

  static int calibrated[5] = {511,511,511,511,511};
  
  for (int i = 0; i<5; i++){
    if (minFingers[i] != maxFingers[i]){
      calibrated[i] = (int)((float)(rawFingers[i] - minFingers[i]) / (float)(maxFingers[i] - minFingers[i]) * ANALOG_MAX);
    }
    else {
      calibrated[i] = ANALOG_MAX / 2;
    }
  }
  return calibrated;
  
}

int analogReadDeadzone(byte pin){
  int raw = analogRead(pin);
  if (abs(ANALOG_MAX/2 - raw) < JOYSTICK_DEADZONE * ANALOG_MAX / 100)
    return ANALOG_MAX/2;
  else
    return raw;
}

int getJoyX(){
  #if JOYSTICK_BLANK
  return ANALOG_MAX/2;
  #elif JOY_FLIP_X
  return ANALOG_MAX - analogReadDeadzone(PIN_JOY_X);
  #else
  return analogReadDeadzone(PIN_JOY_X);
  #endif
}

int getJoyY(){
  #if JOYSTICK_BLANK
  return ANALOG_MAX/2;
  #elif JOY_FLIP_Y
  return ANALOG_MAX - analogReadDeadzone(PIN_JOY_Y);
  #else
  return analogReadDeadzone(PIN_JOY_Y);
  #endif
}

bool getButton(byte pin){
  return digitalRead(pin) != HIGH;
}
