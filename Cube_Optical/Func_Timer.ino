void Timer_setup() {
  timer.every(PIDTime, Timer_PID);
  timer.every(CycTime, Timer_CTR);
}

void  Timer_PID() {
  PID_Control(PIDnum);
  PIDnum = (PIDnum + 1) % 4;
}

void Timer_CTR() {
  for (int i = 0; i < 4; i++) {
    LED_Switch(i);
    SPI_ADCdata[i * 2] = SPI_Read_channel(ChannelPin[i * 2]);
    SPI_ADCdata[i * 2 + 1] = SPI_Read_channel(ChannelPin[i * 2 + 1]);
    Display_PlotImg(i, false);
    SaveData_WriteIn_Data(i);

    Temp_steady[i] = Temp_check(i);
    HeatingTime_CounterRun(i);
    Display_Progressbar(i);
    Display_ReadyLED(i);
    Display_RealTempDig(i);
    Display_ConstDig(i);
  }

  if (Cycles == SecCycles) {
    Temp[4] = Temp_avg(4);
    Temp[5] = Temp_avg(5);
    SavaData_CheckCard();
    serial_log_RXD();
    if (LogPrint_en)
      serial_log_TXD();
    Cycles = 0;
  }
  Cycles++;
}


