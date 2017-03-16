void Button_setup() {
  wdt_reset();
  pinMode(Button_0, INPUT);
  pinMode(Button_1, INPUT);
  pinMode(Button_2, INPUT);
  pinMode(Button_3, INPUT);
}

bool Button_read(int pin) {
  return (digitalRead(pin));
}

void Button_check(int ch) {
  button[ch] = Button_read(button_pin[ch]);
  if (button[ch] == true) {
    button_disable_counter[ch] = button_disable_delay;
    if (Heating_Begin[ch] == false) {
      buzzer_once();
      if (Temp_steady[ch] == true) {
        HeatingTime_Counter[ch] = HeatingTime[ch];
        Tar[ch] = PreHeatingTemp[ch] + BoostTemp_Diff[ch];
        Heating_Begin[ch] = true;
        Heating_Ready[ch] = true;
        Display_PlotImg(ch, true);
        Display_ResultImg(ch, true);
        if (USB_Module == true && USB_Disk_In == true)
          SavaData_CreateFile(ch);
      }
    }
  }
  else if (button[ch] == false) {
    if (button_disable_counter[ch] > 0) {
      button_disable_counter[ch]--;
    }
    else {
      if (Heating_Begin[ch]) {
        //        SavaData_CloseFile(ch);
      }
      Heating_Begin[ch] = false;
      HeatingTime_Counter[ch] = Heating_beg_tag;
      if (Heating_Ready[ch])
        Tar[ch] = StandbyTemp_Def;
      else {
        if (Temp_steady[ch] == true) {
          if (WarmUpTime_Counter[ch])
            WarmUpTime_Counter[ch]--;
          else {
            Tar[ch] = StandbyTemp_Def;
            Heating_Ready[ch] = true;
          }
        }
        else
          Tar[ch] = PreHeatingTemp[ch] + BoostTemp_Diff[ch];
      }
    }
  }
}

