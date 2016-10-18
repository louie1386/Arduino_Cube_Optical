void Button_setup() {
  pinMode(Button_0, INPUT);
  pinMode(Button_1, INPUT);
  pinMode(Button_2, INPUT);
  pinMode(Button_3, INPUT);
}

bool button_read(int pin) {
  return (digitalRead(pin));
}

void button_check(int ch) {
  button[ch] = button_read(button_pin[ch]);
  if (button[ch] == true) {
    button_disable_counter[ch] = button_disable_delay;
    if (Heating_Begin[ch] == false) {
      buzzer_once();
      if (Temp_steady[ch] == true) {
        HeatingTime_Counter[ch] = HeatingTime[ch];
        Tar[ch] = PreHeatingTemp[ch];
        Heating_Begin[ch] = true;
      }
    }
  }
  else if (button[ch] == false) {
    if (button_disable_counter[ch] > 0) {
      button_disable_counter[ch]--;
    }
    else {
      Heating_Begin[ch] = false;
      HeatingTime_Counter[ch] = (-1);
      Tar[ch] = PreHeatingTemp[ch];
    }
  }
}
