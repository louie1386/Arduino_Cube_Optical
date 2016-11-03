void EEPROM_setup() {
  if (EEPROM[EEPROM_WriteIn_addr] == EEPROM_WriteInByte) {
    EEPROM_writeall();
    EEPROM.put(EEPROM_WriteIn_addr, EEPROM_WriteInByte);
  }
  else {
    EEPROM_readall();
  }
}

void EEPROM_clear() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.update(i, 0xFF);
  }
}

void EEPROM_readall() {
  for (int i = 0; i < 4; i++) {
    EEPROM.get(HeatingTime_addr[i], HeatingTime[i]);
    EEPROM.get(ResponseTime_addr[i], ResponseTime[i]);
    EEPROM.get(PreHeatingTemp_addr[i], PreHeatingTemp[i]);
    EEPROM.get(HeatingTemp_Max_addr[i], HeatingTemp_Max[i]);
    EEPROM.get(HeatingTemp_Min_addr[i], HeatingTemp_Min[i]);
    EEPROM.get(Temp_diff_addr[i], Temp_diff[i]);
    EEPROM.get(PD_Cons_addr[i], PD_Cons[i]);
  }
  for (int i = 0; i < 2; i++) {
    EEPROM.get(Dis_plot_Gate_addr[i], Dis_plot_Gate[i]);
  }
}

void EEPROM_writeall() {
  for (int i = 0; i < 4; i++) {
    EEPROM.put(HeatingTime_addr[i], HeatingTime[i]);
    EEPROM.put(ResponseTime_addr[i], ResponseTime[i]);
    EEPROM.put(PreHeatingTemp_addr[i], PreHeatingTemp[i]);
    EEPROM.put(HeatingTemp_Max_addr[i], HeatingTemp_Max[i]);
    EEPROM.put(HeatingTemp_Min_addr[i], HeatingTemp_Min[i]);
    EEPROM.put(Temp_diff_addr[i], Temp_diff[i]);
    EEPROM.put(PD_Cons_addr[i], PD_Cons[i]);
  }
  for (int i = 0; i < 2; i++) {
    EEPROM.put(Dis_plot_Gate_addr[i], Dis_plot_Gate[i]);
  }
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


