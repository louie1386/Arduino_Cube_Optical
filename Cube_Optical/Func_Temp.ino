void  TempIC_setup() {
  wdt_reset();
  analogReference(DEFAULT);
  ADC3V3 = analogRead(AREF_3V3);
  for (int i = 0; i < 4; i++)
    Tar[i] = PreHeatingTemp[i] + BoostTemp_Diff[i];
}

double TempIC_get(int pin) {
  double temp = 0;
  ADC3V3 = analogRead(AREF_3V3);
  temp = (double(analogRead(pin)) * 3300 / ADC3V3 - TempIC_base) / TempIC_reso;
  return temp;
}

double Temp_avg(int num) {
  double Max = 0, Min = 1000, buff, temp = 0;
  for (int i = 0; i < TempSampleTimes; i++) {
    buff = TempIC_get(TIC_pin[num]);
    Max = max(Max, buff);
    Min = min(Min, buff);
    temp += buff;
  }
  if (TempSampleTimes > 2)
    temp = (temp - Max - Min) / (TempSampleTimes - 2);
  else
    temp = temp / TempSampleTimes;
  return (temp + Temp_diff[num]);
}

bool  Temp_check(int ch) {
  if (Temp[ch] >= (Tar[ch] - TempTarRange))
    return true;
  else
    return false;
}

double Temp_Get_Data(int ch) {
  if (!Temp_simulation)
    return Temp_avg(ch);
  else
    return Temp_Data_simulation(ch);
}

double Temp_Data_simulation(int ch) {
  double DS_temp;
  if (Temp[ch] == 0)
    Temp[ch] = Temp[4];
  if (Temp[ch] < Tar[ch])
    DS_temp = Temp[ch] + ((Tar[ch] - Temp[ch]) / 10);
  else
    DS_temp = Temp[ch] - ((Temp[ch] - Tar[ch]) / 10);
  return DS_temp;
}

