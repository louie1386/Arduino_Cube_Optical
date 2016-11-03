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
        HeatingTime_Counter[ch] = HeatingTime[ch] * SecCycles;
        Tar[ch] = PreHeatingTemp[ch];
        Heating_Begin[ch] = true;
        Display_ResultImg(ch, true);
        Display_PlotImg(ch, true);
        SavaData_OpenFile(ch);
      }
    }
  }
  else if (button[ch] == false) {
    if (button_disable_counter[ch] > 0) {
      button_disable_counter[ch]--;
    }
    else {
      if (Heating_Begin[ch])
        SavaData_CloseFile(ch);
      Heating_Begin[ch] = false;
      HeatingTime_Counter[ch] = Heating_beg_tag;
      Tar[ch] = PreHeatingTemp[ch];
    }
  }
}

