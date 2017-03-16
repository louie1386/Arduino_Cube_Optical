void SavaData_setup() {
  wdt_reset();
  USBdisk_setup();
}

void SavaData_CheckConnect() {
  if (USB_Module == true && USB_Disk_In == true) {
    if (USBdisk_DISK_CONNECT() == USB_INT_SUCCESS) {
      if (!USB_Disk_In) {
        Serial_Log.println("----------USB Disk In!");
        buzzer_times(2, 100);
      }
      USB_Disk_In = true;
    }
    else if (USBdisk_DISK_CONNECT() == USB_ERR_DISK_DISCON) {
      if (USB_Disk_In) {
        Serial_Log.println("----------USB Disk Out!");
        buzzer_times(2, 100);
      }
      USB_Disk_In = false;
    }
  }
}
////------------------------------------------------------
byte SavaData_WriteIn(int len, char* data) {
  char datastr[len];
  byte ret, datawait = len, datawrited = 0;
  ret = USBdisk_BYTE_WRITE(len);
  while (datawait > 0) {
    for (int i = 0; i < datawait; i++)
      datastr[i] = data[i + datawrited];
    if (ret == USB_INT_DISK_WRITE)datawrited += USBdisk_WR_REQ_DATA(datastr, datawait);
    ret = USBdisk_BYTE_WR_GO();
    datawait = len - datawrited;
  }
  return ret;
}

void SavaData_CreateFile(int num) {
  byte ret;
  String FileName = Save_FileName;
  int ID = 0;
  String IDstr;
  char str[64];
  int charnum = 0;
  FileName.replace("N", String(num + 1));
  USBdisk_SET_FILE_NAME(Save_DirName);
  ret = USBdisk_DIR_CREATE();
  while (ret == USB_INT_SUCCESS) {
    if (ret == USB_INT_SUCCESS)USBdisk_SET_FILE_NAME(FileName);
    if (ret == USB_INT_SUCCESS)ret = USBdisk_FILE_OPEN();
    if (ret == USB_INT_SUCCESS) {
      ret = USBdisk_FILE_CLOSE(1);
      if (ret == USB_INT_SUCCESS)USBdisk_SET_FILE_NAME(Save_DirName);
      if (ret == USB_INT_SUCCESS)ret = USBdisk_DIR_CREATE();
      ID++;
      if (ID < 10)
        IDstr = "00" + String(ID);
      else if (ID < 100)
        IDstr = "0" + String(ID);
      else
        IDstr = String(ID);
      FileName = Save_FileName;
      FileName.replace("N", String(num + 1));
      FileName.replace("000", IDstr);
    }
  }
  if (ret == USB_ERR_MISS_FILE)ret = USBdisk_FILE_CREATE();
  if (ret == USB_INT_SUCCESS)ret = USBdisk_BYTE_LOCATE();
  if (ret == USB_INT_SUCCESS)ret = SavaData_WriteIn(sizeof(Save_Setting), Save_Setting);
  SaveData_Get_Setting_Str(num, PD_Cons[num], int(Dis_delta_Gate), int(Dis_sigma_Gate), int(Dis_pA_Gate_Def), double(Dis_Ratio_Max), double(Dis_Ratio_Min), int(HeatingTime_Def / FuncFreq));
  char buff[int(Save_Setting_str[num][SaveSettingCharSize - 1])];
  for (int i = 0; i < sizeof(buff); i++)
    buff[i] = Save_Setting_str[num][i];
  if (ret == USB_INT_SUCCESS)ret = SavaData_WriteIn(sizeof(buff), buff);
  if (ret == USB_INT_SUCCESS)ret = SavaData_WriteIn(sizeof(Save_Title), Save_Title);
  if (ret == USB_INT_SUCCESS) {
    ret = USBdisk_DIR_INFO_READ();

    word rtctime = RTC_timetoFAT32();
    byte rtctimeArray[2] = {(lowByte(rtctime)), (highByte(rtctime))};
    word rtcdate = RTC_datetoFAT32();
    byte rtcdateArray[2] = {(lowByte(rtcdate)), (highByte(rtcdate))};

    USBdisk_WR_OFS_DATA(USB_FAT32_Setuptime_addr, 2, rtctimeArray);
    USBdisk_WR_OFS_DATA(USB_FAT32_Setupdate_addr, 2, rtcdateArray);
    USBdisk_WR_OFS_DATA(USB_FAT32_Accessdate_addr, 2, rtcdateArray);
    USBdisk_WR_OFS_DATA(USB_FAT32_Modifytime_addr, 2, rtctimeArray);
    USBdisk_WR_OFS_DATA(USB_FAT32_Modifydate_addr, 2, rtcdateArray);
    //  USBdisk_RD_USB_DATA0();
  }
  if (ret == USB_INT_SUCCESS)ret = USBdisk_DIR_INFO_SAVE();
  ret = USBdisk_FILE_CLOSE(1);
  if (ret == USB_INT_SUCCESS)Save_RealFileName[num] = FileName;
}

void SaveData_WriteInData(int num) {
  USBdisk_SET_FILE_NAME(Save_DirName);
  byte ret = USBdisk_DIR_CREATE();
  if (ret == USB_INT_SUCCESS)USBdisk_SET_FILE_NAME(Save_RealFileName[num]);
  if (ret == USB_INT_SUCCESS)ret = USBdisk_FILE_OPEN();
  if (ret == USB_INT_SUCCESS)ret = USBdisk_BYTE_LOCATE();

  char buff[int(Save_data_str[num][SaveDataCharSize - 1])];
  for (int i = 0; i < sizeof(buff); i++)
    buff[i] = Save_data_str[num][i];
  if (ret == USB_INT_SUCCESS)ret = SavaData_WriteIn(sizeof(buff), buff);
  ret = USBdisk_FILE_CLOSE(1);
}

void SaveData_Get_Setting_Str(int Well, double Constant, int Delta_Gate, int Sigma_Gate, int Result_Gate, double Max_Ratio, double Min_Ratio, int Total_Time) {
  int itemnum = SaveSettingItems;
  String data[itemnum];
  char comma = ',';
  char nextline[3] = {'\r', '\n', 0x00};
  int datasize = 1, itemno = 0;
  data[itemno] = String(Well + 1);

  data[++itemno] = String(Constant, 2);
  data[++itemno] = String(Delta_Gate);
  data[++itemno] = String(Sigma_Gate);
  data[++itemno] = String(Result_Gate);

  data[++itemno] = String(Max_Ratio, 1);
  data[++itemno] = String(Min_Ratio, 1);

  data[++itemno] = String(Total_Time);

  for (int i = 0; i < itemnum; i++) {
    datasize += data[i].length() + 1;
  }
  datasize += (1 * itemnum + 2);

  byte str_charArray[datasize];
  int bytes_num = 0;

  for (int i = 0; i < itemnum; i++) {
    int len = (byte) data[i].length() + 1;
    char temp[len];
    data[i].toCharArray(temp, len);
    for (int j = 0; j < len; j++) {
      str_charArray[bytes_num] = temp[j];
      bytes_num++;
    }
    str_charArray[bytes_num] = comma;
    bytes_num++;
  }
  for (int n = 0; n < 3; n++) {
    str_charArray[bytes_num] = nextline[n];
    bytes_num++;
  }
  for (int p = 0; p < SaveDataCharSize; p++)
    Save_Setting_str[Well][p] = 0x00;
  for (int p = 0; p < datasize; p++)
    Save_Setting_str[Well][p] = str_charArray[p];

  Save_Setting_str[Well][SaveSettingCharSize - 1] = (byte)datasize;
  //  for (int i = 0; i < (Save_Setting_str[Well][SaveSettingCharSize - 1]); i++)
  //    Serial_Log.print(char(Save_Setting_str[Well][i]));
}

void SaveData_Get_Data_Str(int Well, int Time, double avgA, double deltaA, double sigmaA, int plotA, double avgB, double deltaB, double sigmaB, int plotB, bool LED_SW, bool LED_HW, double tempW, double tempL, double tempU, int freeram) {
  int itemnum = SaveDataItems;
  String data[itemnum];
  char comma = ',';
  char nextline[3] = {'\r', '\n', 0x00};
  int datasize = 1, itemno = 0;
  data[itemno] = String(Time);

  data[++itemno] = String(avgA, 2);
  data[++itemno] = String(deltaA, 2);
  data[++itemno] = String(sigmaA, 2);
  data[++itemno] = String(plotA);

  data[++itemno] = String(avgB, 2);
  data[++itemno] = String(deltaB, 2);
  data[++itemno] = String(sigmaB, 2);
  data[++itemno] = String(plotB);

  data[++itemno] = String(LED_SW);
  data[++itemno] = String(LED_HW);
  data[++itemno] = String(tempW, 2);
  data[++itemno] = String(tempL, 2);
  data[++itemno] = String(tempU, 2);
  data[++itemno] = String(freeram);

  for (int i = 0; i < itemnum; i++) {
    datasize += data[i].length() + 1;
  }
  datasize += (1 * itemnum + 2);

  byte str_charArray[datasize];
  int bytes_num = 0;

  for (int i = 0; i < itemnum; i++) {
    int len = (byte) data[i].length() + 1;
    char temp[len];
    data[i].toCharArray(temp, len);
    for (int j = 0; j < len; j++) {
      str_charArray[bytes_num] = temp[j];
      bytes_num++;
    }
    str_charArray[bytes_num] = comma;
    bytes_num++;
  }
  for (int n = 0; n < 3; n++) {
    str_charArray[bytes_num] = nextline[n];
    bytes_num++;
  }
  for (int p = 0; p < SaveDataCharSize; p++)
    Save_data_str[Well][p] = 0x00;
  for (int p = 0; p < datasize; p++)
    Save_data_str[Well][p] = str_charArray[p];

  Save_data_str[Well][SaveDataCharSize - 1] = (byte)datasize;
}

void SaveData_WriteInJudgment(int num, int result) {
  char Judgment[] = "Judgment:,";
  char PPD[] = "+ +\r\n";
  char PND[] = "+ -\r\n";
  char NPD[] = "- +\r\n";
  char NND[] = "- -\r\n";
  USBdisk_SET_FILE_NAME(Save_DirName);
  byte ret = USBdisk_DIR_CREATE();
  if (ret == USB_INT_SUCCESS)USBdisk_SET_FILE_NAME(Save_RealFileName[num]);
  if (ret == USB_INT_SUCCESS)ret = USBdisk_FILE_OPEN();
  if (ret == USB_INT_SUCCESS)ret = USBdisk_BYTE_LOCATE();
  if (ret == USB_INT_SUCCESS)ret = SavaData_WriteIn(sizeof(Judgment), Judgment);

  if (result == Dis_ResultImg_D_PP || result == Dis_ResultImg_S_PP) {
    if (ret == USB_INT_SUCCESS)ret = SavaData_WriteIn(sizeof(PPD), PPD);
  }
  else if (result == Dis_ResultImg_D_PN || result == Dis_ResultImg_S_PN) {
    if (ret == USB_INT_SUCCESS)ret = SavaData_WriteIn(sizeof(PND), PND);
  }
  else if (result == Dis_ResultImg_D_NP || result == Dis_ResultImg_S_NP) {
    if (ret == USB_INT_SUCCESS)ret = SavaData_WriteIn(sizeof(NPD), NPD);
  }
  else if (result == Dis_ResultImg_D_NN || result == Dis_ResultImg_S_NN) {
    if (ret == USB_INT_SUCCESS)ret = SavaData_WriteIn(sizeof(NND), NND);
  }

  ret = USBdisk_FILE_CLOSE(1);
}

