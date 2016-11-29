void Timer_setup() {
  timer.every(CycTime, Timer_CTR);
}

void Timer_CTR() {
  if (Cycles == SecCycles) {
    Temp[4] = Temp_avg(4);
    Temp[5] = Temp_avg(5);
    SavaData_CheckCard();
    serial_log_RXD();
    if (LogPrint_en)
      serial_log_TXD();
    for (int i = 0; i < 4; i++) {
      Temp_steady[i] = Temp_check(i);
      Display_Progressbar(i);
      Display_ReadyLED(i);
      Display_RealTempDig(i);
      Display_ConstDig(i);
      //Dis_LEDtrigger(i);
    }
    Cycles = 0;
  }
  else
    Cycles++;

  for (int i = 0; i < 4; i++) {
    Button_check(i);
    LED_Switch(i);
    SPI_ADCdata[i * 2] = ADC_Read_channel(ChannelPin[i * 2]);
    SPI_ADCdata[i * 2 + 1] = ADC_Read_channel(ChannelPin[i * 2 + 1]);
    Display_PlotImg(i, false);
    SaveData_WriteIn_Data(i);
    HeatingTime_CounterRun(i);
  }
}


