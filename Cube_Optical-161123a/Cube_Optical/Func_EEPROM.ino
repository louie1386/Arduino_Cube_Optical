void EEPROM_setup() {
  Serial_Log.begin(Baudrate_Log);
  unsigned char WriteInByte;
  EEPROM.get(EEPROM_WriteIn_addr, WriteInByte);
  if (WriteInByte == EEPROM_WriteInByte) {
    EEPROM_writeall();
    Serial_Log.println("Write New EEPROM");
  }
  else {
    EEPROM_readall();
    Serial_Log.println("Read EEPROM");
  }
  EEPROM.put(EEPROM_WriteIn_addr, 0xAA);
}

void EEPROM_clear() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.update(i, 0xFF);
  }
}

void EEPROM_readall() {
  for (int i = 0; i < 4; i++) {
    unsigned int HeatingTime_Addr = HeatingTime_addr[i];
    unsigned int ResponseTime_Addr = ResponseTime_addr[i];
    unsigned int PreHeatingTemp_Addr = PreHeatingTemp_addr[i];
    unsigned int HeatingTemp_Max_Addr = HeatingTemp_Max_addr[i];
    unsigned int HeatingTemp_Min_Addr = HeatingTemp_Min_addr[i];
    unsigned int Temp_diff_Addr = Temp_diff_addr[i];
    unsigned int PD_Cons_Addr = PD_Cons_addr[i];
    double heatingTime, responseTime, preHeatingTemp, heatingTemp_Max, heatingTemp_Min, temp_diff, pd_Cons;

    EEPROM.get(HeatingTime_Addr, heatingTime);
    EEPROM.get(ResponseTime_Addr, responseTime);
    EEPROM.get(PreHeatingTemp_Addr, preHeatingTemp);
    EEPROM.get(HeatingTemp_Max_Addr, heatingTemp_Max);
    EEPROM.get(HeatingTemp_Min_Addr, heatingTemp_Min);
    EEPROM.get(Temp_diff_Addr, temp_diff);
    EEPROM.get(PD_Cons_Addr, pd_Cons);
    if (EEPROM_readdef_eable) {
      if (heatingTime > 0 && heatingTime <= HeatingTime_Def)
        HeatingTime[i] = heatingTime;
      if (responseTime > 0 && responseTime <= HeatingTime[i])
        ResponseTime[i] = responseTime;
      if (preHeatingTemp > 0 && preHeatingTemp <= MaxTemp)
        PreHeatingTemp[i] = preHeatingTemp;
      if (heatingTemp_Max >= HeatingTemp_Min[i] && heatingTemp_Max <= PreHeatingTemp[i])
        HeatingTemp_Max[i] = heatingTemp_Max;
      if (heatingTemp_Min >= 0 && heatingTemp_Min <= HeatingTemp_Max[i])
        HeatingTemp_Min[i] = heatingTemp_Min;
      if (temp_diff >= (-50) && temp_diff <= 50)
        Temp_diff[i] = temp_diff;
      if (pd_Cons >= 0 && pd_Cons <= 5)
        PD_Cons[i] = pd_Cons;
    }
  }
  for (int i = 0; i < 2; i++) {
    unsigned int Dis_plot_Gate_Addr = Dis_plot_Gate_addr[i];
    double dis_plot_Gate;
    EEPROM.get(Dis_plot_Gate_Addr, dis_plot_Gate);
    if (EEPROM_readdef_eable)
      if (dis_plot_Gate >= 0 && dis_plot_Gate <= 1000)
        Dis_plot_Gate[i] = dis_plot_Gate;
  }
}

void EEPROM_writeall() {
  for (int i = 0; i < 4; i++) {
    unsigned int HeatingTime_Addr = HeatingTime_addr[i];
    unsigned int ResponseTime_Addr = ResponseTime_addr[i];
    unsigned int PreHeatingTemp_Addr = PreHeatingTemp_addr[i];
    unsigned int HeatingTemp_Max_Addr = HeatingTemp_Max_addr[i];
    unsigned int HeatingTemp_Min_Addr = HeatingTemp_Min_addr[i];
    unsigned int Temp_diff_Addr = Temp_diff_addr[i];
    unsigned int PD_Cons_Addr = PD_Cons_addr[i];

    double heatingTime, responseTime, preHeatingTemp, heatingTemp_Max, heatingTemp_Min, temp_diff, pd_Cons;

    heatingTime = HeatingTime[i];
    responseTime = ResponseTime[i];
    preHeatingTemp = PreHeatingTemp[i];
    heatingTemp_Max = HeatingTemp_Max[i];
    heatingTemp_Min = HeatingTemp_Min[i];
    temp_diff = Temp_diff[i];
    pd_Cons = PD_Cons[i];

    EEPROM.put(HeatingTime_Addr, heatingTime);
    EEPROM.put(ResponseTime_Addr, responseTime);
    EEPROM.put(PreHeatingTemp_Addr, preHeatingTemp);
    EEPROM.put(HeatingTemp_Max_Addr, heatingTemp_Max);
    EEPROM.put(HeatingTemp_Min_Addr, heatingTemp_Min);
    EEPROM.put(Temp_diff_Addr, temp_diff);
    EEPROM.put(PD_Cons_Addr, pd_Cons);
  }
  for (int i = 0; i < 2; i++) {
    unsigned int Dis_plot_Gate_Addr = Dis_plot_Gate_addr[i];
    double dis_plot_Gate;
    dis_plot_Gate = Dis_plot_Gate[i];
    EEPROM.put(Dis_plot_Gate_Addr, dis_plot_Gate);
  }
  EEPROM.put(EEPROM_WriteIn_addr, 0xAA);
}

void EEPROM_resetall() {
  double Temp_diff_t[4] = {TempIC_Diff_0, TempIC_Diff_1, TempIC_Diff_2, TempIC_Diff_3};
  double PD_Cons_t[4]   = {PD_Cons_0, PD_Cons_1, PD_Cons_2, PD_Cons_3};
  double Dis_plot_Gate_t[2]   = {Dis_pA_Gate_Def, Dis_pB_Gate_Def};
  for (int i = 0; i < 4; i++) {
    HeatingTime[i] = HeatingTime_Def;
    ResponseTime[i] = ResponseTime_Def;
    PreHeatingTemp[i] = PreHeatingTemp_Def;
    HeatingTemp_Max[i] = HeatingTemp_Max_Def;
    HeatingTemp_Min[i] = HeatingTemp_Min_Def;
    Temp_diff[i] = Temp_diff_t[i];
    PD_Cons[i] = PD_Cons_t[i];
  }
  for (int i = 0; i < 2; i++) {
    Dis_plot_Gate[i] = Dis_plot_Gate_t[i];
  }
  EEPROM_writeall();
}


