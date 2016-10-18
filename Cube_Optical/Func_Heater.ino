void  HeatingTime_CounterRun(int num) {
  button_check(num);
  LED_Switch(num);
  if (HeatingTime_Counter[num] == ResponseTime[num])
    Tar[num] = HeatingTemp_Min[num];
  else if (HeatingTime_Counter[num] == 0) {
    buzzer_once();
    HeatingTime_Counter[num] = (-1);
  }
  else if (HeatingTime_Counter[num] < ResponseTime[num] && HeatingTime_Counter[num] > 0) {
    if (Temp[num] >= HeatingTemp_Max[num])
      Tar[num] = HeatingTemp_Min[num];
    else if (Temp[num] <= HeatingTemp_Min[num])
      Tar[num] = HeatingTemp_Max[num];
  }

  if (HeatingTime_Counter[num] > 0)
    HeatingTime_Counter[num]--;
}

