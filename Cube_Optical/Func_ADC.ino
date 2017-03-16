void ADC_setup() {
  wdt_reset();
  pinMode(SSPin, OUTPUT);
  ADC_disable();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.begin();
  ADC_setChRange();
}

void ADC_enable() {
  digitalWrite(SSPin, LOW);
}

void ADC_disable() {
  digitalWrite(SSPin, HIGH);
}

void ADC_setChRange() {
  unsigned int data;
  for (int ch = 0; ch < 8; ch++) {
    data = StartBit + ch * baseBit + RangeSelectBits;
    ADC_enable();
    SPI.transfer(data);
    ADC_disable();
  }
}

unsigned int ADC_Read_channel(int ch) {
  unsigned int data = (StartBit + ch * baseBit + ConvStartBits) * 256;
  unsigned int ADCdata;

  ADC_enable();
  SPI.transfer16(data);
  ADCdata = SPI.transfer16(0);
  ADC_disable();

  ADCdata = ADCdata / 4;
  return ADCdata;
}

unsigned int ADC_Get_Data(int ch) {
  if (!ADC_simulation)
    return ADC_Read_channel(ChannelPin[ch]);
  else
    return ADC_Data_simulation(ch);
}

unsigned int ADC_Data_simulation(int ch) {
  int well = int(ch / 2);
  double  TimerCounter = double(HeatingTime_Counter[well]) / 2;
  unsigned int Data_simulation;

  unsigned int  Data_LED_Off_base, Data_LED_On_base;
  unsigned int  Data_shift, Data_increments;
  double  shift_Time, increments_Time;

  if (DS_LED_Prev[well] == false && LED_onoff[well] == true)
    DS_LED_On_num[well]++;

  DS_LED_Prev[well] = LED_onoff[well];

  if (ch % 2 == 0) {
    Data_LED_Off_base = DS_LED_OFF_base_A;
    Data_LED_On_base = DS_LED_ON_base_A;
    Data_shift = DS_shift_A;
    Data_increments = DS_increments_A;
    shift_Time = DS_shift_Time_A;
    increments_Time = DS_increments_Time_A;
  }
  else {
    Data_LED_Off_base = DS_LED_OFF_base_B;
    Data_LED_On_base = DS_LED_ON_base_B;
    Data_shift = DS_shift_B;
    Data_increments = DS_increments_B;
    shift_Time = DS_shift_Time_B;
    increments_Time = DS_increments_Time_B;
  }

  Data_simulation = Data_LED_Off_base;

  if (LED_onoff[well]) {
    Data_simulation += Data_LED_On_base;
    if (DS_LED_On_num[well] >= shift_Time)
      Data_simulation += Data_shift;
    if (DS_LED_On_num[well] >= increments_Time)
      Data_simulation += (Data_increments * (DS_LED_On_num[well] - increments_Time + 1));
  }

  return Data_simulation;
}

