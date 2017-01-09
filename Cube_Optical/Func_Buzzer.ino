void Buzzer_setup() {
  wdt_reset();
  buzzer_once();
}

void buzzer_once() {
  Serial_Log.println("b~~~~~~~");
  tone(Buzzer, buzzer_Hz, buzzer_ms);
}

