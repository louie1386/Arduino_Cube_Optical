void  HeatingTime_CounterRun(int num) {
  wdt_reset();
  Timer_Test(num);
  if (HeatingTime_Counter[num] == ResponseTime[num]) {
    Tar[num] = HeatingTemp_Min[num];
    Temp_steady[num] = Temp_check(num);
    if (Temp_steady[num] == false) {
      Serial_Log.print("Well ");
      Serial_Log.print(num);
      Serial_Log.println(" heatering fail!");
    }
  }
  else if (HeatingTime_Counter[num] == 0) {
    buzzer_once();
    Display_ResultImg(num, false);
    HeatingTime_Counter[num] = Heating_fin_tag;
    Tar[num] = 0;
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


