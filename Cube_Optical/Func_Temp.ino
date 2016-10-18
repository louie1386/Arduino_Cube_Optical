void  TempIC_setup() {
  ADC3V3 = analogRead(AREF_3V3);
}

double TempIC_get(int pin) {
  double temp = 0;
  ADC3V3 = analogRead(AREF_3V3);
  temp = (double(analogRead(pin)) / ADC3V3 * 3250 - TempIC_base) / TempIC_reso;
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
  temp = (temp - Max - Min) / (TempSampleTimes - 2);
  return (temp + Temp_diff[num]);
}

bool  Temp_check(int ch) {
  if (Temp[ch] >= (Tar[ch] - TempTarRange) && Temp[ch] <= (Tar[ch] + TempTarRange))
    return true;
  else
    return false;
}

