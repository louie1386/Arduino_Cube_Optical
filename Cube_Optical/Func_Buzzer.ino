void Buzzer_setup() {
  buzzer_once();
}

void buzzer_once() {
  tone(Buzzer, buzzer_Hz, buzzer_ms);
}

