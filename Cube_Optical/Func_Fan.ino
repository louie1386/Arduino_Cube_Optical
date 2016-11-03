void Fan_setup() {
  pinMode(Fan, OUTPUT);
  digitalWrite(Fan, LOW);
  Fan_On();
}

void Fan_On() {
  digitalWrite(Fan, HIGH);
}

void Fan_Off() {
  digitalWrite(Fan, LOW);
}

