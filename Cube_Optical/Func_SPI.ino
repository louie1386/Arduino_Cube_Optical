void SPI_setup() {
  pinMode(SSPin, OUTPUT);
  SPI_disable();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.begin();
  SPI_setChRange();
}

void SPI_enable() {
  digitalWrite(SSPin, LOW);
}

void SPI_disable() {
  digitalWrite(SSPin, HIGH);
}

void SPI_setChRange() {
  unsigned int data;
  for (int ch = 0; ch < 8; ch++) {
    data = StartBit + ch * baseBit + RangeSelectBits;
    SPI_enable();
    SPI.transfer(data);
    SPI_disable();
  }
}

unsigned int SPI_Read_channel(int ch) {
  unsigned int data = (StartBit + ch * baseBit + ConvStartBits) * 256;
  unsigned int ADCdata;

  SPI_enable();
  SPI.transfer16(data);
  ADCdata = SPI.transfer16(0);
  SPI_disable();

  ADCdata = ADCdata / 4;
  return ADCdata;
}
