void Timer_setup() {
  timer.every(PIDTime, Timer_PID);
  timer.every(secTime, Timer_sec);
}

void  Timer_PID() {
  PID_Control(PIDnum);
  PIDnum = (PIDnum + 1) % 4;

}

void Timer_sec() {
  Temp[4] = Temp_avg(4);
  Temp[5] = Temp_avg(5);
  
  serial_log_RXD();
  if (LogPrint_en)
    serial_log_TXD();

  for (int i = 0; i < 4; i++) {
    Temp_steady[i] = Temp_check(i);
    HeatingTime_CounterRun(i);
    SPI_ADCdata[i * 2] = SPI_Read_channel(ChannelPin[i * 2]);
    SPI_ADCdata[i * 2 + 1] = SPI_Read_channel(ChannelPin[i * 2 + 1]);
  }
}

