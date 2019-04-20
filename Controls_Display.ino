/* This section controls the activation of the pump, brew valve, and steam valve
*/

void setupControls() {
  pinMode(BREW, OUTPUT);
  pinMode(STEAM, OUTPUT);
  pinMode(PUMP, OUTPUT);
  digitalWrite(BREW, LOW);
  digitalWrite(STEAM, LOW);
  digitalWrite(PUMP, LOW);
}

void preinfControl() {
  brewState = digitalRead(BREW);
  if (brewState == 1) {
    if (millis() - brewStarted >= PREINF_DELAY) {
      digitalWrite(PUMP, HIGH);
      brewState = 0;
    }
    else{
      brewState = 1;
    }
  }
}

void bt0PopCallback(void *ptr)
{
  uint32_t dual_state;
  NexDSButton *btn = (NexDSButton *)ptr;
  memset(buffer, 0, sizeof(buffer));
  bt0.getValue(&dual_state);
  if (dual_state) {
    group1Timer.startTimer();
    digitalWrite(BREW, HIGH);
    brewStarted = millis ();
  } else {
    digitalWrite(BREW, LOW);
    digitalWrite(PUMP, LOW);
    group1Timer.stopTimer();
  }
}
void bt1PopCallback(void *ptr)
{
  uint32_t dual_state;
  NexDSButton *btn = (NexDSButton *)ptr;
  memset(buffer, 0, sizeof(buffer));
  bt1.getValue(&dual_state);
  if (dual_state) {
    digitalWrite(STEAM, HIGH);
  } else {
    digitalWrite(STEAM, LOW);
  }
}

void nextionSendCommand(const char* cmd)
{
  Serial2.print(cmd);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
}

void nextionSetText(String component, String txt)
{
  String componentText = component + ".txt=\"" + txt + "\"";
  nextionSendCommand(componentText.c_str());
}

void boolToText (String) {
  if (stats == "1") {
    stats = "ON";
  }
  else {
    stats = "OFF";
  }
}

void nextionUpdateStatus() {
  stats = digitalRead(STEAM);
  boolToText(stats);
  nextionSetText("t0", stats);
  stats = digitalRead(BREW);
  boolToText(stats);
  nextionSetText("t1", stats);
  stats = digitalRead(PUMP);
  boolToText(stats);
  nextionSetText("t2", stats);
  stats = digitalRead(HEAT);
  boolToText(stats);
  nextionSetText("t3", stats);
  stats = digitalRead(AUTOFILL);
  boolToText(stats);
  nextionSetText("t4", stats);
  stats = getTemp();
  nextionSetText("t5", stats);
  stats = "";
  timer = group1Timer.readTimer();
  nextionSetText("t9", timer);
}

void nextionUpdateSettings() {
  settings = getTargetTemp();
  nextionSetText("t7", settings);
  settings = PREINF_DELAY/1000;
  nextionSetText("t8", settings);
  settings = "";
}

