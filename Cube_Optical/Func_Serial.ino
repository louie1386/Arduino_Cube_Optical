void Serial_setup() {
  Serial_Log.begin(Baudrate);
  Serial_Log.println("");
  Serial_Log.print("Start/Reset (Version: ");
  Serial_Log.print(Version);
  Serial_Log.println(")");
  Serial_Log.print("PID: (");
  serial_log_DoubleAlign(dKp);
  Serial_Log.print(", ");
  serial_log_DoubleAlign(dKi);
  Serial_Log.print(", ");
  serial_log_DoubleAlign(dKd);
  Serial_Log.println(")");
}

void serial_log_DoubleAlign(double value) {
  if (value > 0)
  {
    if (value < 10)
      Serial_Log.print("  ");
    else if (value < 100)
      Serial_Log.print(" ");
  }
  Serial_Log.print(value);
}

void serial_log_TXD() {
  for (int i = 0; i < 4; i++) {
    Serial_Log.print("Ch");
    Serial_Log.print(i + 1);
    Serial_Log.print(":[ ");
    serial_log_DoubleAlign(Temp[i]);
    Serial_Log.print(" <");
    serial_log_DoubleAlign(Tar[i]);
    Serial_Log.print(", ");
    serial_log_DoubleAlign(Temp_diff[i]);
    Serial_Log.print(">, ");
    serial_log_DoubleAlign(Volt[i]);
    Serial_Log.print("(byte-V)]--Heating Time(sec): ");
    serial_log_DoubleAlign(HeatingTime_Counter[i]);
    Serial_Log.println(" sec");
  }
  
  Serial_Log.print("PD-ADC: (");
  for (int i = 0; i < 8; i++) {
    Serial_Log.print((double(SPI_ADCdata[i])*4.096*3/16384));
    Serial_Log.print("V, ");
  }
  Serial_Log.println(")");
  
  Serial_Log.print("Lower Temp: ");
  Serial_Log.println(Temp[4]);
  Serial_Log.print("Upper Temp: ");
  Serial_Log.println(Temp[5]);
  Serial_Log.println("");
}

void serial_log_RXD() {
  while (Serial_Log.available() > 0) {
    char RXD = Serial_Log.read();
    if (RXD == 'p')
      LogPrint_en = true;
    else if (RXD == 'n')
      LogPrint_en = false;
  }
}

