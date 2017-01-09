void ADC_setup() {
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

