void Timer_setup() {
  wdt_reset();
  timer.every(CycTime, Timer_CTR);
}

void Timer_CTR() {
  FuncFreq_num++;
  if (FuncFreq_num == FuncFreq)FuncFreq_num = 0;
  if (FuncFreq_num < 4 && LED_onoff[FuncFreq_num] == false) {
    if (Save_data_Ready[FuncFreq_num]) {
      if (Save_data_print) {
        for (int i = 0; i < (int)(Save_data_str[FuncFreq_num][SaveDataCharSize - 1]); i++)
          Serial_Log.print(char(Save_data_str[FuncFreq_num][i]));
        Serial_Log.println();
      }
      if (USB_Module == true && USB_Disk_In == true)
        SaveData_WriteInData(FuncFreq_num);
      if (HeatingTime_Counter[FuncFreq_num] == Heating_fin_tag)
        Save_data_output_finish[FuncFreq_num] = true;
      Save_data_Ready[FuncFreq_num] = false;
    }
  }
  else if (FuncFreq_num == 4) {
    SavaData_CheckConnect();
    Temp[4] = Temp_avg(4);
    Temp[5] = Temp_avg(5);
    serial_log_RXD();
    if (LogPrint_en)
      serial_log_TXD();
    Dis_Settrigger();
    Fan_Control();
  }
  else if (FuncFreq_num == 5) {
    for (int i = 0; i < 4; i++) {
      Display_Progressbar(i);
      Display_RealTempDig(i);
      Display_ConstDig(i);
      Dis_LEDtrigger(i);
      Display_Status(i);
    }
  }
  else if (FuncFreq_num == 6) {
    for (int i = 0; i < 4; i++) {
      if (Save_data_Judgment_output[i] == true && Save_data_output_finish[i] == true) {
        SaveData_WriteInJudgment(i, Save_data_Judgment[i]);
        Save_data_Judgment[i] = 0;
        Save_data_Judgment_output[i] = false;
        Save_data_output_finish[i] = false;
      }
    }
  }
  else if (FuncFreq_num == 7) {

  }
  else if (FuncFreq_num == 8) {

  }
  else if (FuncFreq_num == 9) {

  }

  for (int i = 0; i < 4; i++) {
    Temp_steady[i] = Temp_check(i);
    Button_check(i);
    LED_Switch(i);
    SPI_ADCdata[i * 2] = ADC_Get_Data(i * 2);
    SPI_ADCdata[i * 2 + 1] = ADC_Get_Data(i * 2 + 1);
    Display_PlotImg(i, false);
    HeatingTime_CounterRun(i);
  }
}

void Timer_Test(int well) {
  unsigned long testtime = millis();
  unsigned long realtime;
  int timecouter = HeatingTime_Counter[well];
  if (timecouter == HeatingTime_Def) {
    TimeStart[well] = testtime;
    Serial_Log.print("Well ");
    Serial_Log.print(well + 1);
    Serial_Log.println(" Start!");
  }
  else if (timecouter == 0) {
    TimeEnd[well] = testtime;
    realtime = TimeEnd[well] - TimeStart[well];
    Serial_Log.print("Well ");
    Serial_Log.print(well + 1);
    Serial_Log.print(" Real Time: ");
    Serial_Log.print(double(realtime) / 1000, 3);
    Serial_Log.println(" Sec");
  }
}

