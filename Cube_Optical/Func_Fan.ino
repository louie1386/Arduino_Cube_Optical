void Fan_setup() {
  wdt_reset();
  pinMode(Fan, OUTPUT);
  digitalWrite(Fan, LOW);
  Fan_PWM(Fan_PWM_LowPower0);
}

void Fan_PWM(int pwm) {
  analogWrite(Fan, pwm);
  Fan_PWM_now = pwm;
}

void Fan_Control() {
  bool Fan_Eable = false;
  for (int i = 0; i < 4; i++)
    Fan_Eable = Fan_Eable + button[i];

  if (Fan_Eable) {
    if (Temp[5] >= Fan_Gate2_Temp)
      Fan_PWM(Fan_PWM_FullPower);
    else
      Fan_PWM(Fan_PWM_LowPower1);
  }
  else {
    if (Temp[5] >= Fan_Gate3_Temp)
      Fan_PWM(Fan_PWM_FullPower);
    else {
      if (Fan_PWM_now == Fan_PWM_FullPower)
        Fan_PWM(Fan_PWM_LowPower1);
      else if (Fan_PWM_now == Fan_PWM_LowPower0 && Temp[5] >= Fan_Gate1_Temp)
        Fan_PWM(Fan_PWM_LowPower1);
      else if (Fan_PWM_now == Fan_PWM_LowPower1 && Temp[5] < Fan_Gate0_Temp)
        Fan_PWM(Fan_PWM_LowPower0);
    }
  }
}

