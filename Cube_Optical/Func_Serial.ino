void Serial_setup() {
  wdt_reset();
  Serial_Log.begin(Baudrate_Log);
  Serial_Log.println("");
  Serial_Log.println("-----------------------------Start/Reset");
  Serial_Log.print("Version: ");
  Serial_Log.print(Version);
  Serial_Log.println(subVersion);
  serial_log_Custom();
}

void serial_log_TXD() {
  for (int i = 0; i < 4; i++) {
    Serial_Log.print("Well");
    Serial_Log.print(i + 1);
    Serial_Log.print(": Tr = ");
    Serial_Log.print(Temp[i], 1);
    Serial_Log.print(", Ttr = ");
    Serial_Log.print(Tar[i], 1);
    Serial_Log.print(", Tfix = ");
    Serial_Log.print(Temp_diff[i], 1);
    Serial_Log.print(", Vh(0-255) = ");
    Serial_Log.print(Volt[i], 0);
    Serial_Log.print(", Th(sec) = ");
    if (HeatingTime_Counter[i] == Heating_beg_tag)
      Serial_Log.println("Preheating");
    else if (HeatingTime_Counter[i] == Heating_fin_tag)
      Serial_Log.println("Timeout");
    else {
      Serial_Log.print(int(HeatingTime_Counter[i] / FuncFreq), DEC);
      Serial_Log.println(" sec");
    }
  }
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
      LogPrint_en = !LogPrint_en;
    else if (RXD == 'u')
      USB_debug_print = !USB_debug_print;
    else if (RXD == 'r')
      RTC_print = !RTC_print;
  }
}

void serial_log_Custom() {
  Serial_Log.print("Machine Number : ");
  Serial_Log.println(Machine_Number);
  Serial_Log.println("-----------------------------Tempture Fix");
  serial_log_Aligned("TempIC Fix(Well 1) = ");
  Serial_Log.println(TempIC_Diff_0);
  serial_log_Aligned("TempIC Fix(Well 2) = ");
  Serial_Log.println(TempIC_Diff_1);
  serial_log_Aligned("TempIC Fix(Well 3) = ");
  Serial_Log.println(TempIC_Diff_2);
  serial_log_Aligned("TempIC Fix(Well 4) = ");
  Serial_Log.println(TempIC_Diff_3);

  serial_log_Aligned("Boost Fix(Well 1) = ");
  Serial_Log.println(Boost1_Diff);
  serial_log_Aligned("Boost Fix(Well 2) = ");
  Serial_Log.println(Boost2_Diff);
  serial_log_Aligned("Boost Fix(Well 3) = ");
  Serial_Log.println(Boost3_Diff);
  serial_log_Aligned("Boost Fix(Well 4) = ");
  Serial_Log.println(Boost4_Diff);

  Serial_Log.println("-----------------------------PID Setting");
  Serial_Log.print("PID: (");
  Serial_Log.print(double(dKp), 0);
  Serial_Log.print(", ");
  Serial_Log.print(double(dKi), 0);
  Serial_Log.print(", ");
  Serial_Log.print(double(dKd), 0);
  Serial_Log.println(")");

  Serial_Log.println("-----------------------------Time Setting");
  serial_log_Aligned("Heating Time(sec) = ");
  Serial_Log.println(HeatingTime_Def / FuncFreq);
  serial_log_Aligned("Preheating Time(sec) = ");
  Serial_Log.println(PreHeatingTime_Def / FuncFreq);
  serial_log_Aligned("Warmup Time(sec) = ");
  Serial_Log.println(WarmUpTime_Def / FuncFreq);

  Serial_Log.println("-----------------------------Tempture Setting");
  serial_log_Aligned("Preheating Tempture = ");
  Serial_Log.println(PreHeatingTemp_Def);
  serial_log_Aligned("Standby Tempture = ");
  Serial_Log.println(StandbyTemp_Def);
  serial_log_Aligned("Max Heating Tempture = ");
  Serial_Log.println(HeatingTemp_Max_Def);
  serial_log_Aligned("Min Heating Tempture = ");
  Serial_Log.println(HeatingTemp_Min_Def);

  Serial_Log.println("-----------------------------Photodiode Setting");
  serial_log_Aligned("PD Value(Well 1) = ");
  Serial_Log.println(PD_Cons_0);
  serial_log_Aligned("PD Value(Well 2) = ");
  Serial_Log.println(PD_Cons_1);
  serial_log_Aligned("PD Value(Well 3) = ");
  Serial_Log.println(PD_Cons_2);
  serial_log_Aligned("PD Value(Well 4) = ");
  Serial_Log.println(PD_Cons_3);

  Serial_Log.println("-----------------------------Algorithm Setting");
  serial_log_Aligned("Delta Gate = ");
  Serial_Log.println(Dis_delta_Gate);
  serial_log_Aligned("Sigma Gate = ");
  Serial_Log.println(Dis_sigma_Gate);
  serial_log_Aligned("Channel A Result Gate =  ");
  Serial_Log.println(Dis_pA_Gate_Def);
  serial_log_Aligned("Channel B Result Gate =  ");
  Serial_Log.println(Dis_pB_Gate_Def);
  serial_log_Aligned("Max Result Ratio = ");
  Serial_Log.println(Dis_Ratio_Max);
  serial_log_Aligned("Min Result Ratio = ");
  Serial_Log.println(Dis_Ratio_Min);
  serial_log_Aligned("Result Imgage Type =  ");
  Serial_Log.println(Dis_ResultImg_Type);

  Serial_Log.println("-----------------------------Upper Fan Setting");
  serial_log_Aligned("Fan Tempture Gate0 = ");
  Serial_Log.println(Fan_Gate0_Temp);
  serial_log_Aligned("Fan Tempture Gate1 = ");
  Serial_Log.println(Fan_Gate1_Temp);
  serial_log_Aligned("Fan Tempture Gate2 = ");
  Serial_Log.println(Fan_Gate2_Temp);
  serial_log_Aligned("Fan Tempture Gate3 = ");
  Serial_Log.println(Fan_Gate3_Temp);

  serial_log_Aligned("Fan PWM LowPower0 = ");
  Serial_Log.println(Fan_PWM_LowPower0);
  serial_log_Aligned("Fan PWM LowPower1 = ");
  Serial_Log.println(Fan_PWM_LowPower1);
  serial_log_Aligned("Fan PWM FullPower = ");
  Serial_Log.println(Fan_PWM_FullPower);

  Serial_Log.println("-----------------------------Function Simulation");
  serial_log_Aligned("PD ADC Simulation = ");
  Serial_Log.println(ADC_simulation);
  serial_log_Aligned("Tempture Simulation = ");
  Serial_Log.println(Temp_simulation);
}

void serial_log_Aligned(String itemname) {
  int Aligned_size = 30;
  int len = itemname.length();
  int Space_size = Aligned_size - len;
  Serial_Log.print(itemname);
  for (int i = 0; i < Space_size; i++)
    Serial_Log.print(" ");
}

