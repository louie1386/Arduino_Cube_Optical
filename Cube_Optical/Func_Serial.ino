void Serial_setup() {
  wdt_reset();
  Serial_Log.begin(Baudrate_Log);
  Serial_Log.println("");
  Serial_Log.print("Start/Reset (Version: ");
  Serial_Log.print(Version);
  Serial_Log.print(subVersion);
  Serial_Log.println(")");
  Serial_Log.print("PID: (");
  Serial_Log.print(double(dKp), 0);
  Serial_Log.print(", ");
  Serial_Log.print(double(dKi), 0);
  Serial_Log.print(", ");
  Serial_Log.print(double(dKd), 0);
  Serial_Log.println(")");
  serial_log_EEPROM_ReadOut();
}

void serial_log_TXD() {
  for (int i = 0; i < 4; i++) {
    Serial_Log.print("Well");
    Serial_Log.print(i + 1);
    Serial_Log.print(": Tr = ");
    Serial_Log.print(Temp[i], 1);
    Serial_Log.print(", Ttr = ");
    Serial_Log.print(Tar[i], 0);
    Serial_Log.print(", Tfix = ");
    Serial_Log.print(Temp_diff[i], 0);
    Serial_Log.print(", Vh(0-255) = ");
    Serial_Log.print(Volt[i], 0);
    Serial_Log.print(", Th(sec) = ");
    if (HeatingTime_Counter[i] == Heating_beg_tag)
      Serial_Log.println("Preheating");
    else if (HeatingTime_Counter[i] == Heating_fin_tag)
      Serial_Log.println("Timeout");
    else {
      Serial_Log.print(int(HeatingTime_Counter[i] / 10), DEC);
      Serial_Log.println(" sec");
    }
  }
  /*
    Serial_Log.print("PD-ADC: (");
    for (int i = 0; i < 8; i++) {
      Serial_Log.print((double(SPI_ADCdata[i]) * 4.096 * 3 / 16384));
      Serial_Log.print("V, ");
    }
    Serial_Log.println(")");
  */
  Serial_Log.print("Lower Temp: ");
  Serial_Log.println(Temp[4]);
  Serial_Log.print("Upper Temp: ");
  Serial_Log.println(Temp[5]);
  Serial_Log.println("");
}

void serial_log_RXD() {
  while (Serial_Log.available() > 0) {
    char RXD = Serial_Log.read();
    if (LogEEPROM_en == false) {
      if (RXD == 'p')
        LogPrint_en = true;
      else if (RXD == 'n')
        LogPrint_en = false;
      else if (RXD == 'e') {
        serial_log_EEPROM_ReadOut();
      }
      else if (RXD == 'r') {
        EEPROM_resetall();
        serial_log_EEPROM_ReadOut();
      }
      else if (RXD == 'w') {
        LogEEPROM_en = true;
        LogPrint_en = false;
        Serial_Log.println("");
        Serial_Log.println("-------------------------Enter EEPROM mode!");
        serial_log_EEPROM_ask();
      }
      else if (RXD == 'c') {
        EEPROM_clear();
        Serial_Log.println("-------------------------Clear EEPROM!");
      }
      else if (RXD == 'l') {
        for (int i = 0; i < 4; i++)
          LED_TurnOn[i] = !LED_TurnOn[i];
      }
    }
    else {
      serial_log_EEPROM_WriteIn(RXD);
    }
  }
}

//EEPROM------------------------------------------
void serial_log_EEPROM_item(double *item, unsigned int *addr, int itemnum, int pointnum, String namestr) {
  Serial_Log.print(namestr);
  double buff;
  for (int i = 0; i < itemnum - 1; i++) {
    Serial_Log.print(item[i], pointnum);
    Serial_Log.print("[");
    EEPROM.get(addr[i], buff);
    Serial_Log.print(buff, pointnum);
    Serial_Log.print("], ");
  }
  Serial_Log.print(item[itemnum - 1], pointnum);
  Serial_Log.print("[");
  EEPROM.get(addr[itemnum - 1], buff);
  Serial_Log.print(buff, pointnum);
  Serial_Log.println("]");
}

void serial_log_EEPROM_ReadOut() {
  EEPROM_readall();
  Serial_Log.println("");
  Serial_Log.println("EEPROM: ");
  String namestr[8] = {
    "---Heating Time(Sec):        ",
    "---Response Time(Sec):       ",
    "---PreHeating Temp(C):       ",
    "---Heating Max Temp(C):      ",
    "---Heating Min Temp(C):      ",
    "---Temp diff(C):             ",
    "---PD Cons:                  ",
    "---Result Gate(ADC counter): "
  };
  serial_log_EEPROM_item(HeatingTime, HeatingTime_addr, 4, 0, namestr[0]);
  serial_log_EEPROM_item(ResponseTime, ResponseTime_addr, 4, 0, namestr[1]);
  serial_log_EEPROM_item(PreHeatingTemp, PreHeatingTemp_addr, 4, 0, namestr[2]);
  serial_log_EEPROM_item(HeatingTemp_Max, HeatingTemp_Max_addr, 4, 0, namestr[3]);
  serial_log_EEPROM_item(HeatingTemp_Min, HeatingTemp_Min_addr, 4, 0, namestr[4]);
  serial_log_EEPROM_item(Temp_diff, Temp_diff_addr, 4, 1, namestr[5]);
  serial_log_EEPROM_item(PD_Cons, PD_Cons_addr, 4, 1, namestr[6]);
  serial_log_EEPROM_item(Dis_plot_Gate, Dis_plot_Gate_addr, 2, 0, namestr[7]);
  Serial_Log.println("");
}

void serial_log_EEPROM_WriteIn(char RXD) {
  if (RXD == 'o') {
    LogEEPROM_en = false;
    Serial_Log.println("-------------------------Exit EEPROM mode!");
  }
  else if (LogEEPROM_addr_WriteIn == false) {
    if (RXD >= 48 && RXD <= 57) {
      LogEEPROM_addrnum = LogEEPROM_addrnum * 10 + (int(RXD) - 48);
      Serial_Log.println(LogEEPROM_addrnum);
      serial_log_EEPROM_input();
    }
    else if (RXD == '(') {
      if (LogEEPROM_addrnum >= 0 && LogEEPROM_addrnum <= (EEPROM_itemnum - 1)) {
        LogEEPROM_addr_WriteIn = true;
        Serial_Log.print(LogEEPROM_addrnum);
        Serial_Log.println("(");
        serial_log_EEPROM_input();
      }
      else {
        Serial_Log.println("EEPROM addr fail!");
        serial_log_EEPROM_ask();
        LogEEPROM_addrnum = 0;
      }
    }
  }
  else if (LogEEPROM_addr_WriteIn == true) {
    if (RXD >= 48 && RXD <= 57) {
      if (LogEEPROM_data_point == false) {
        LogEEPROM_data = LogEEPROM_data * 10 + (int(RXD) - 48);
      }
      else {
        LogEEPROM_data = int(LogEEPROM_data) + double(int(RXD) - 48) / 10;
      }
      Serial_Log.print(LogEEPROM_addrnum);
      Serial_Log.print("(");
      Serial_Log.println(LogEEPROM_data, 1);
      serial_log_EEPROM_input();
    }
    else if (RXD == '.') {
      LogEEPROM_data_point = true;
    }
    else if (RXD == ')') {
      EEPROM.put(EEPROM_addr[LogEEPROM_addrnum], LogEEPROM_data);
      Serial_Log.print(LogEEPROM_addrnum);
      Serial_Log.print("(");
      Serial_Log.print(LogEEPROM_data, 1);
      Serial_Log.println(")");
      serial_log_EEPROM_ReadOut();
      LogEEPROM_addr_WriteIn = false;
      LogEEPROM_data_point = false;
      LogEEPROM_addrnum = 0;
      LogEEPROM_data = 0;
      serial_log_EEPROM_ask();
    }
  }
}

void serial_log_EEPROM_ask() {
  Serial_Log.println("Well Number:            [W1] [W2] [W3] [W4]");
  Serial_Log.println("-------------------------------------------");
  Serial_Log.println("Heating Time addr:      [00] [01] [02] [03]");
  Serial_Log.println("Response Time addr:     [04] [05] [06] [07]");
  Serial_Log.println("PreHeating Temp addr:   [08] [09] [10] [11]");
  Serial_Log.println("Max Heating Temp addr:  [12] [13] [14] [15]");
  Serial_Log.println("Min Heating Temp addr:  [16] [17] [18] [19]");
  Serial_Log.println("Temp diff addr:         [20] [21] [22] [23]");
  Serial_Log.println("PD Cons addr:           [24] [25] [26] [27]");
  Serial_Log.println("");
  Serial_Log.println("Plot A/B:               [pA] [pB]");
  Serial_Log.println("-------------------------------------------");
  Serial_Log.println("Dis plot Gate addr:     [28] [29]");
  Serial_Log.println("");
  Serial_Log.println("[Enter 'o' exit EEPROM mode]");
  Serial_Log.println("EEPROM comand formate:");
  Serial_Log.println(">> addr(data)");
  Serial_Log.print(">> ");
}

void serial_log_EEPROM_input() {
  Serial_Log.print(">> ");
}


