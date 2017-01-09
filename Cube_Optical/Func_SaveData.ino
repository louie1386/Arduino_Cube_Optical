void SavaData_setup() {
  SavaData_CheckModule();
  SavaData_Reset();
  SavaData_CheckCard();
}

void SavaData_Reset() {
  pinMode(SavReset, OUTPUT);
  digitalWrite(SavReset, LOW);
  delay(10);
  digitalWrite(SavReset, HIGH);
  delay(10);

  Serial_Sav.begin(Baudrate_Sav_Def);
  fprot_init(SavaData_TXD, SavaData_RXD, SavaData_Delay);
  SavaData_SetBaudRate();
}

void SavaData_CheckModule() {
  pinMode(SD_Module, INPUT);
  Save_Module = digitalRead(SD_Module);
  if (Save_Module)
    Serial_Log.println("SD Moudle: True");
  else
    Serial_Log.println("SD Moudle: False");
}

void SavaData_SetBaudRate() {
  if (Save_Module) {
    if (fprot_set_baudrate(Baudrate_Sav_New) == FPROT_NO_ERROR) {
      Serial_Sav.end();
      Serial_Sav.begin(Baudrate_Sav_New);
      delay(10);
    }
  }
}

void SavaData_TXD(unsigned char* data, unsigned long data_len) {
  Serial_Sav.write(data, data_len);
}

unsigned long SavaData_RXD(unsigned char* data, unsigned long data_len) {
  unsigned long read = 0;

  while (read < data_len) {
    if (Serial_Sav.available() > 0)
      data[read++] = Serial_Sav.read();
  }
  return read;
}

void SavaData_Delay(unsigned long ms) {
  delay(ms);
}

//------------------------------------------------------
void SavaData_NewPath(int Num) {
  int dirnum[3];
  if (Save_file_num[Num] > 999)
    Save_file_num[Num] = 0;
  dirnum[0] = int(Save_file_num[Num] / 100);
  dirnum[1] = int(Save_file_num[Num] % 100 / 10);
  dirnum[2] = int(Save_file_num[Num] % 10);
  for (int i = 0; i < 3; i++)
    Save_file_path[9 + i] = char(dirnum[i] + 48);
  Save_file_path[17] = char(Num + 1 + 48);
}

void SavaData_OpenFile(int num) {
  if (Save_cardin) {
    if (Save_file_ID[num])
      fprot_close(Save_file_ID[num]);
    Save_file_num[num] = 0;
    SavaData_NewPath(num);
    unsigned char File_ID = 0;
    while (fprot_open(Save_file_path, FPROT_MODE_RW | FPROT_MODE_CREATE_NEW, &File_ID) != FPROT_NO_ERROR) {
      Save_file_num[num]++;
      SavaData_NewPath(num);
    }
    Serial_Log.print(Save_file_path);
    Save_file_ID[num] = File_ID;
    if (Save_file_ID[num] == FPROT_INVALID_FILE) {
      Serial_Log.print("-----------Open well");
      Serial_Log.print(num + 1);
      Serial_Log.print(" file fail!---");
      Serial_Log.println(Save_file_ID[num]);
      Save_file_ID[num] = 0;
    }
    else {
      SaveData_WriteIn_Title(num);
      fprot_flush(Save_file_ID[num]);
      Serial_Log.print("-----------Open well");
      Serial_Log.print(num + 1);
      Serial_Log.print(" file successful!---");
      Serial_Log.println(Save_file_ID[num]);
    }
  }
}

void SavaData_CloseFile(int num) {
  if (Save_cardin) {
    if (Save_file_ID[num]) {
      fprot_flush(Save_file_ID[num]);
      fprot_close(Save_file_ID[num]);
      Serial_Log.print("-----------Close well");
      Serial_Log.print(num + 1);
      Serial_Log.print(" file---");
      Serial_Log.println(Save_file_ID[num]);
      Save_file_ID[num] = 0;
    }
  }
}

void SavaData_CheckCard() {
  if (Save_Module) {
    unsigned char ret = fprot_check_card();
    if (ret == FPROT_NO_ERROR) {
      fprot_mkdir(Save_file_dir);
      if (!Save_cardin) {
        Serial_Log.println("-----------SD Card In!");
      }
      Save_cardin = true;
    }
    else if (ret = FPROT_NO_CARD) {
      if (Save_cardin) {
        fprot_close_all();
        Serial_Log.println("-----------SD Card Out!");
      }
      Save_cardin = false;
    }
  }
}

void SaveData_WriteIn(int num, String str) {
  unsigned long written;
  int strlength = str.length() + 1;
  char strarray[strlength];
  str.toCharArray(strarray, strlength);
  if (Save_cardin) {
    if (Save_file_ID[num]) {
      fprot_write(Save_file_ID[num], strarray, strlen(strarray), &written);
      //fprot_flush(Save_file_ID[num]);
    }
  }
}

void SaveData_WriteIn_Title(int num) {
  String title = "Time,Plot A,Plot B, LED, Temperature, Well Check, Draw A, Draw B\r\n";
  SaveData_WriteIn(num, title);
}

void SaveData_WriteIn_Data(int num) {
  if (HeatingTime_Counter[num] >= 0) {
    String Time = String(float(HeatingTime[num] * SecCycles - HeatingTime_Counter[num]) / 10, 1);
    String pA = String(SPI_ADCdata[num * 2]);
    String pB = String(SPI_ADCdata[num * 2 + 1]);
    String LED = String(digitalRead(LED_pin[num]));
    String tr = String(Temp[num]);
    String wn = String(num + 1);
    String PlotA = String(Dis_plot_draw[num * 2]);
    String PlotB = String(Dis_plot_draw[num * 2 + 1]);

    String str =
      Time + String(", ") +
      pA + String(", ") +
      pB + String(", ") +
      LED + String(", ") +
      tr + String(", ") +
      wn + String(", ") +
      PlotA + String(", ") +
      PlotB + String("\r\n");
    SaveData_WriteIn(num, str);
  }
}



