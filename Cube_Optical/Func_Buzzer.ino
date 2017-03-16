void Buzzer_setup() {
  wdt_reset();
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, LOW);
  Serial_Log.println("-----------------------------Buzzer Test");
  buzzer_once();
}

void buzzer_once() {
  Serial_Log.println("Buzzer Bi~~~~~~~");
  digitalWrite(Buzzer, HIGH);
  delay(buzzer_ms);
  digitalWrite(Buzzer, LOW);
  //  tone(Buzzer, buzzer_Hz, buzzer_ms);
  //  buzzer_tone(buzzer_Hz, buzzer_ms);
}

void buzzer_tone(int Hz, int Time) {
  double Tcycle = 1000 / double(Hz);
  double X = 255 * Tcycle / 2;
  for (int i = 0; i < Time; i++) {
    analogWrite(Buzzer, 255 - int(X));
    delay(1);
  }
  analogWrite(Buzzer, 0);
}

void buzzer_times(int Times, int offms) {
  buzzer_once();
  for (int i = 0; i < (Times - 1); i++) {
    delay(offms);
    buzzer_once();
  }
}

