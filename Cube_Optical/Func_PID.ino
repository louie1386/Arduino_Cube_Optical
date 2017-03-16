void PID_setup() {
  wdt_reset();
  PID0.SetOutputLimits(0, PIDOutputLimit);
  PID0.SetSampleTime(PIDSampleTime);
  PID0.SetMode(AUTOMATIC);
  PID1.SetOutputLimits(0, PIDOutputLimit);
  PID1.SetSampleTime(PIDSampleTime);
  PID1.SetMode(AUTOMATIC);
  PID2.SetOutputLimits(0, PIDOutputLimit);
  PID2.SetSampleTime(PIDSampleTime);
  PID2.SetMode(AUTOMATIC);
  PID3.SetOutputLimits(0, PIDOutputLimit);
  PID3.SetSampleTime(PIDSampleTime);
  PID3.SetMode(AUTOMATIC);
}

void PID_Control(int num) {
  Temp[num] = Temp_Get_Data(num);

  switch (num) {
    case  0:
      PID0.Compute();
      break;
    case  1:
      PID1.Compute();
      break;
    case  2:
      PID2.Compute();
      break;
    case  3:
      PID3.Compute();
      break;
  }

  if (Volt[num] < 0 || Volt[num] > PIDOutputLimit)
    Volt[num] = 0;

  if (Temp[num] >= MaxTemp || Temp[num] <= 0)
    Volt[num] = 0;
  analogWrite(Heater_pin[num], Volt[num]);
}

void  PID_Loop() {
  PID_Control(PIDnum);
  PIDnum = (PIDnum + 1) % 4;
}

