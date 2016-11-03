void SavaData_setup() {
  SavaData_Reset();
  SavaData_CheckModule();
  Serial_Sav.begin(Baudrate_Sav_Def);
  fprot_init(SavaData_TXD, SavaData_RXD, SavaData_Delay);
  SavaData_SetBaudRate();
  SavaData_CheckCard();
}

void SavaData_Reset() {
  pinMode(SavReset, OUTPUT);
  digitalWrite(SavReset, LOW);
  delay(10);
  digitalWrite(SavReset, HIGH);
  delay(10);
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
void SavaData_NewPath(int num) {
  int dirnum[3];
  if (Save_file_num[num] > 999)
    Save_file_num[num] = 0;
  dirnum[0] = int(Save_file_num[num] / 100);
  dirnum[1] = int(Save_file_num[num] % 100 / 10);
  dirnum[2] = int(Save_file_num[num] % 10);
  for (int i = 0; i < 3; i++)
    Save_file_path[9 + i] = char(dirnum[i] + 48);
  Save_file_path[17] = char(num + 1 + 48);
}

void SavaData_OpenFile(int num) {
  if (Save_cardin) {
    if (Save_file_ID[num])
      fprot_close(Save_file_ID[num]);
    while (fprot_open(Save_file_path, FPROT_MODE_RW | FPROT_MODE_CREATE_NEW, &Save_file_ID[num]) != FPROT_NO_ERROR) {
      Save_file_num[num]++;
      SavaData_NewPath(num);
    }

    if (Save_file_ID[num] == FPROT_INVALID_FILE) {
      Save_file_ID[num] = 0;
      Serial_Log.print("-----------Open well");
      Serial_Log.print(num + 1);
      Serial_Log.println(" file fail!");
    }
    else {
      SaveData_WriteIn_Title(num);
      fprot_flush(Save_file_ID[num]);
      Serial_Log.print("-----------Open well");
      Serial_Log.print(num + 1);
      Serial_Log.println(" file successful!");
    }
  }
}

void SavaData_CloseFile(int num) {
  if (Save_cardin) {
    if (Save_file_ID[num]) {
      fprot_close(Save_file_ID[num]);
      Serial_Log.print("-----------Close well");
      Serial_Log.print(num + 1);
      Serial_Log.println(" file");
    }
  }
}

void SavaData_CheckCard() {
  if (Save_Module) {
    if (fprot_check_card() == FPROT_NO_ERROR) {
      fprot_mkdir(Save_file_dir);
      if (!Save_cardin)
        Serial_Log.println("-----------SD Card In!");
      Save_cardin = true;
    }
    else {
      if (Save_cardin) {
        fprot_close_all();
        Serial_Log.println("-----------SD Card Out!");
      }
      Save_cardin = false;
    }
  }
}

void SaveData_WriteIn(int num, char str[]) {
  unsigned long written;
  if (Save_cardin) {
    if (Save_file_ID[num]) {
      if (fprot_write(Save_file_ID[num], str, strlen(str), &written) == FPROT_NO_ERROR)
        fprot_flush(Save_file_ID[num]);
    }
  }
}

void SaveData_WriteIn_Title(int num) {
  char title[] = "Time,Plot A,Plot B, LED, Temperature\r\n";
  SaveData_WriteIn(num, title);
}

void SaveData_WriteIn_Data(int num) {
  if (HeatingTime_Counter[num] >= 0) {
    String Time = String(float(HeatingTime[num] * SecCycles - HeatingTime_Counter[num]) / 10, 1);
    String pA = String(SPI_ADCdata[num * 2]);
    String pB = String(SPI_ADCdata[num * 2 + 1]);
    String LED = String(LEDonoff[num]);
    String tr = String(Temp[num]);

    String str =
      Time + String(", ") +
      pA + String(", ") +
      pB + String(", ") +
      LED + String(", ") +
      tr + String("\r\n");

    int strlength = str.length() + 1;
    char strarray[strlength];
    str.toCharArray(strarray, strlength);
    SaveData_WriteIn(num, strarray);
  }
}



