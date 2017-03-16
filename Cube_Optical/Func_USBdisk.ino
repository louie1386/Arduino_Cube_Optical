void USBdisk_setup() {
  Serial_Sav.begin(Baudrate_Sav);
  pinMode(USB_INT_pin, INPUT);
  pinMode(USB_RST_pin, OUTPUT);
  pinMode(USB_RXD_pin, OUTPUT);
  pinMode(USB_TXD_pin, OUTPUT);
  Serial_Log.println("-----------------------------USB Disk Moudle Check");
  USBdisk_HW_Reset();
  byte ret = USBdisk_CHECK_EXIST(0x0F);
  Serial_Log.print("USB Disk Moudle(Hardware): ");
  if (ret + 0x0F == 0xFF) {
    Serial_Log.println("Pass!");
    USB_Module = true;
    ret = USBdisk_SET_USB_MODE(0x06);
    if (ret == USB_CMD_RET_SUCCESS)
      Serial_Log.println("USB Disk Moudle is Ready!");
  }
  else {
    Serial_Log.println("Fail!");
    USB_Module = false;
  }
}

void USBdisk_HW_Reset() {
  digitalWrite(USB_INT_pin, LOW);
  digitalWrite(USB_RXD_pin, HIGH);
  digitalWrite(USB_TXD_pin, HIGH);
  digitalWrite(USB_INT_pin, LOW);
  delay(50);
  digitalWrite(USB_INT_pin, HIGH);
  delay(50);
  USBdisk_RESET_ALL();
  pinMode(USB_TXD_pin, INPUT);
}

void USBdisk_INT() {
  byte ret;
  if (digitalRead(USB_INT_pin) == USB_INT && USB_INT == false) {
    Serial_Log.println("INT!");
    ret = USBdisk_Command_Return(USB_debug_print, true);
    delay(1);
    if (ret == USB_INT_CONNECT) {
      Serial_Log.println("----------USB Disk In!");
      buzzer_times(2,100);
      String FileName = Save_FileName;
      int ID = 0;
      String IDstr;
      ret = USBdisk_DISK_MOUNT();
      if (ret == USB_INT_SUCCESS)USBdisk_SET_FILE_NAME(Save_DirName);
      if (ret == USB_INT_SUCCESS)ret = USBdisk_DIR_CREATE();
      if (ret == USB_INT_SUCCESS)ret = USBdisk_FILE_CLOSE(0);
      USB_Disk_In = true;
    }
    else if (ret == USB_INT_DISCONNECT) {
      Serial_Log.println("----------USB Disk Out!");
      buzzer_times(2,100);
      USB_Disk_In = false;
    }
  }
  USB_INT = !digitalRead(USB_INT_pin);
}


byte USBdisk_Command_Send(byte * data, int datanum, bool printlog, bool retEnable, bool INT) {
  byte comm[datanum];
  for (int i = 0; i < datanum; i++)
    comm[i] = data[i];
  Serial_Sav.write(comm, datanum);
  if (printlog) {
    Serial_Log.print("<-");
    for (int i = 0; i < datanum; i++) {
      if (comm[i] < 0x10)
        Serial_Log.print(0);
      Serial_Log.print(comm[i], HEX);
      Serial_Log.print(',');
    }
    Serial_Log.println();
  }
  if (retEnable)
    return USBdisk_Command_Return(printlog, INT);
  else
    return 0;
}

byte USBdisk_Command_Return(bool printlog, bool INT) {
  byte intret;
  byte ret[50];
  bool INT_In = false;
  int timer = 10, retnum = 0;
  if (INT) {
    while (!INT_In) {
      delay(1);
      if (!digitalRead(USB_INT_pin)) {
        INT_In = true;
        delay(1);
        if (Serial_Sav.available() > 0) {
          intret = Serial_Sav.read();
          if (printlog) {
            Serial_Log.print("->");
            if (intret < 0x10)
              Serial_Log.print("0");
            Serial_Log.println(intret, HEX);
          }
        }
        USBdisk_GET_STATUS();
      }
    }
  }

  while (timer) {
    delay(1);
    timer--;
    if (printlog == true && Serial_Sav.available() > 0)
      Serial_Log.print("->");
    while (Serial_Sav.available() > 0) {
      ret[retnum] = Serial_Sav.read();
      if (printlog) {
        if (ret[retnum] < 0x10)
          Serial_Log.print("0");
        Serial_Log.print(ret[retnum], HEX);
        Serial_Log.print(",");
      }
      timer = 0;
      retnum++;
    }
    if (printlog)
      Serial_Log.println();
  }
  return ret[0];
}

void USBdisk_GET_STATUS() {
  byte comm[3];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_GET_STATUS;
  USBdisk_Command_Send(comm, 3, USB_debug_print, false, false);
}

void USBdisk_RESET_ALL() {
  byte comm[3];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_RESET_ALL;
  USBdisk_Command_Send(comm, 3, USB_debug_print, false, false);
  delay(200);
  Serial_Log.println("USB Moudle Reset!");
}

byte USBdisk_SET_USB_MODE(int mode) {
  byte comm[4];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_SET_USB_MODE;
  comm[3] = mode;
  return USBdisk_Command_Send(comm, 4, USB_debug_print, true, false);
}

byte USBdisk_CHECK_EXIST(byte val) {
  byte comm[4];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_CHECK_EXIST;
  comm[3] = val;
  return USBdisk_Command_Send(comm, 4, USB_debug_print, true, false);
}

byte USBdisk_DISK_CONNECT() {
  byte comm[3];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_DISK_CONNECT;
  return USBdisk_Command_Send(comm, 3, USB_debug_print, true, true);
}

byte USBdisk_DISK_MOUNT() {
  byte comm[3];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_DISK_MOUNT;
  return USBdisk_Command_Send(comm, 3, USB_debug_print, true, true);
}

void USBdisk_SET_FILE_NAME(String Name) {
  int len = (byte)Name.length() + 1;
  byte temp[len], comm[len + 3];
  Name.getBytes(temp, len);
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_SET_FILE_NAME;
  for (int i = 0; i < len; i++)
    comm[3 + i] = temp[i];
  USBdisk_Command_Send(comm, len + 3, USB_debug_print, false, false);
}

byte USBdisk_DIR_CREATE() {
  byte comm[3];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_DIR_CREATE;
  return USBdisk_Command_Send(comm, 3, USB_debug_print, true, true);
}

byte USBdisk_FILE_OPEN() {
  byte comm[3];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_FILE_OPEN;
  return USBdisk_Command_Send(comm, 3, USB_debug_print, true, true);
}

byte USBdisk_FILE_CREATE() {
  byte comm[3];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_FILE_CREATE;
  return USBdisk_Command_Send(comm, 3, USB_debug_print, true, true);
}

byte USBdisk_BYTE_LOCATE() {
  byte comm[7];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_BYTE_LOCATE;
  comm[3] = 0xFF;
  comm[4] = 0xFF;
  comm[5] = 0xFF;
  comm[6] = 0xFF;
  return USBdisk_Command_Send(comm, 7, USB_debug_print, true, true);
}

byte USBdisk_FILE_CLOSE(int mode) {
  byte comm[4];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_FILE_CLOSE;
  comm[3] = mode;
  return USBdisk_Command_Send(comm, 4, USB_debug_print, true, true);
}

byte USBdisk_BYTE_WRITE(int len) {
  byte comm[5];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_BYTE_WRITE;
  comm[3] = len;
  comm[4] = (len >> 8);
  return USBdisk_Command_Send(comm, 5, USB_debug_print, true, true);
}

byte USBdisk_WR_REQ_DATA(char *str, int len) {
  //  int len = (int)(Save_data_str[well][SaveDataCharSize - 1]);
  byte comm[len + 3];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_WR_REQ_DATA;
  for (int i = 0; i < len; i++)
    comm[3 + i] = (byte)(str[i]);
  return USBdisk_Command_Send(comm, len + 3, USB_debug_print, true, false);
}

byte USBdisk_BYTE_WR_GO() {
  byte comm[3];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_BYTE_WR_GO;
  return USBdisk_Command_Send(comm, 3, USB_debug_print, true, true);
}

byte USBdisk_DIR_INFO_READ() {
  byte comm[4];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_DIR_INFO_READ;
  comm[3] = 0xFF;
  return USBdisk_Command_Send(comm, 4, USB_debug_print, true, true);
}

byte USBdisk_RD_USB_DATA0() {
  byte comm[3];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_RD_USB_DATA0;
  return USBdisk_Command_Send(comm, 3, USB_debug_print, true, false);
}

void USBdisk_WR_OFS_DATA(byte addr, byte len, byte *set) {
  byte comm[len + 5];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_WR_OFS_DATA;
  comm[3] = addr;
  comm[4] = len;
  for (int i = 0; i < len; i++)
    comm[5 + i] = set[i];
  USBdisk_Command_Send(comm, len + 5, USB_debug_print, false, false);
}

byte USBdisk_DIR_INFO_SAVE() {
  byte comm[3];
  comm[0] = USB_CMD_tag0;
  comm[1] = USB_CMD_tag1;
  comm[2] = USB_CMD_DIR_INFO_SAVE;
  return USBdisk_Command_Send(comm, 3, USB_debug_print, true, true);
}
