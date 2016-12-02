void Display_setup() {
  Serial_Dis.begin(Baudrate_Dis);
  Display_CheckModule();
  if (Display_Module) {
    Display_Reset();
    genie.Begin(Serial_Dis);
    genie.AttachEventHandler(Display_myGenieEventHandler);
    genie.WriteContrast(15);
  }
}

void Display_Reset() {
  pinMode(DisReset, OUTPUT);
  digitalWrite(DisReset, LOW);
  delay(100);
  digitalWrite(DisReset, HIGH);
  delay(3500);
}

void Display_CheckModule() {
  Display_Module = digitalRead(Dis_Module);
  if (Display_Module)
    Serial_Log.println("Display Moudle: True");
  else
    Serial_Log.println("Display Moudle: False");
}

void Display_Progressbar(int num) {
  if (Display_Module) {
    int precent = 0;
    if (HeatingTime_Counter[num] >= 0)
      precent = 100 - (HeatingTime_Counter[num] / SecCycles / HeatingTime[num] * 100);
    else if (button[num] == true && HeatingTime_Counter[num] == Heating_fin_tag)
      precent = 100;
    genie.WriteObject(Dis_ProgressBar_Name, num, precent);
  }
}

void Display_ReadyLED(int num) {
  if (Display_Module) {
    int Dis_ReadyLED = Dis_ReadyLed_Off;
    if (button[num] == false) {
      if (Temp_steady[num] == true)
        Dis_ReadyLED = Dis_ReadyLed_On;
    }
    else {
      if (HeatingTime_Counter[num] > 0)
        Dis_ReadyLED = Dis_ReadyLed_On;
    }
    genie.WriteObject(Dis_ReadyLed_Name, num, Dis_ReadyLED);
  }
}

void Display_ResultImg(int num, bool reset) {
  if (Display_Module) {
    int DA, DB;
    int RA = 0, RB = 0;
    DA = Dis_plot_end[num * 2];
    DB = Dis_plot_end[num * 2 + 1];
    
    if (DA == 0)
      DA = 1;
    double Dab = double(DB) / double(DA);

    if (reset == false) {
      if (DA < Dis_plot_Gate[0] && DB < Dis_plot_Gate[1]) {
        RA = Dis_ResultImg_Nega;
        RB = Dis_ResultImg_Nega;
      }
      else if (DA < Dis_plot_Gate[0] && DB >= Dis_plot_Gate[1]) {
        RA = Dis_ResultImg_Nega;
        RB = Dis_ResultImg_Posi;
      }
      else if (DA >= Dis_plot_Gate[0] && DB < Dis_plot_Gate[1]) {
        RA = Dis_ResultImg_Posi;
        RB = Dis_ResultImg_Nega;
      }
      else if (DA >= Dis_plot_Gate[0] && DB >= Dis_plot_Gate[1]) {
        if (Dab >= Dis_Ratio_Max) {
          RA = Dis_ResultImg_Nega;
          RB = Dis_ResultImg_Posi;
        }
        else if (Dab < Dis_Ratio_Max && Dab >= Dis_Ratio_Min) {
          RA = Dis_ResultImg_Posi;
          RB = Dis_ResultImg_Posi;
        }
        else if (Dab < Dis_Ratio_Min) {
          RA = Dis_ResultImg_Posi;
          RB = Dis_ResultImg_Nega;
        }
      }
    }

    genie.WriteObject(Dis_ResultImg_Name, (num * 2), RA);
    genie.WriteObject(Dis_ResultImg_Name, (num * 2 + 1), RB);
  }
}

void Display_RealTempDig(int num) {
  if (Display_Module) {
    genie.WriteObject(Dis_RealTempDig_Name, num, Temp[num]);
  }
}

void Display_ConstDig(int num) {
  if (Display_Module) {
    genie.WriteObject(Dis_ConstDig_Name, (num * 3), Tar[num]);
    genie.WriteObject(Dis_ConstDig_Name, (num * 3 + 1), Temp_diff[num] * 10);
    genie.WriteObject(Dis_ConstDig_Name, (num * 3 + 2), PD_Cons[num] * 10);
    genie.WriteObject(Dis_ConstDig_Name, (num + 12), int(HeatingTime_Counter[num] / 10));
  }
}

void Display_PlotImg(int num, bool reset) {
  if (Display_Module) {
    if (button[num] == true && reset == false) {
      if (LED_onoff[num] == true) {
        if (Dis_data_num[num] == 0) {
          Dis_data_base_avg[num * 2] = 0;
          Dis_data_base_avg[num * 2 + 1] = 0;
          if (Dis_plot_base_enable) {
            for (int i = 0; i < 10; i++) {
              Dis_data_base_avg[num * 2] += Dis_data_base[num * 2][i] / 10;
              Dis_data_base_avg[num * 2 + 1] += Dis_data_base[num * 2 + 1][i] / 10;
            }
          }
        }
        else if (Dis_data_num[num] >= 20 && Dis_data_num[num] < 40) {
          Dis_data_avg[num * 2] += double(SPI_ADCdata[num * 2]) / 20;
          Dis_data_avg[num * 2 + 1] += double(SPI_ADCdata[num * 2 + 1]) / 20;
        }
        else if (Dis_data_num[num] == 40) {
          if (Dis_plot_num[num] == 0) {
            Dis_plot_zero[num * 2] = Dis_data_avg[num * 2] - Dis_data_base_avg[num * 2];
            Dis_plot_zero[num * 2 + 1] = Dis_data_avg[num * 2 + 1] - Dis_data_base_avg[num * 2 + 1];
          }

          int dA = Dis_data_avg[num * 2] - Dis_data_base_avg[num * 2] - Dis_plot_zero[num * 2];
          int dB = Dis_data_avg[num * 2 + 1] - Dis_data_base_avg[num * 2 + 1] - Dis_plot_zero[num * 2 + 1];
          dB = int(double(dB) - double(dA) * PD_Cons[num]);
          int pA = double(dA) * 150 / Dis_ADCcon_Def;
          int pB = double(dB) * 150 / Dis_ADCcon_Def;

          for (int i = 0; i < 20; i++) {
            genie.WriteObject(Dis_PlotImg_Name, (num), pA);
            genie.WriteObject(Dis_PlotImg_Name, (num), pB);
          }
          //                    Serial_Log.println(Dis_data_avg[num * 2]);
          //                    Serial_Log.println(Dis_data_avg[num * 2 + 1]);
          //                    Serial_Log.println(Dis_data_base_avg[num * 2]);
          //                    Serial_Log.println(Dis_data_base_avg[num * 2 + 1]);
          //                    Serial_Log.println(Dis_plot_zero[num * 2]);
          //                    Serial_Log.println(Dis_plot_zero[num * 2 + 1]);
          //                    Serial_Log.println(dA);
          //                    Serial_Log.println(dB);
          //                    Serial_Log.println(pA);
          //                    Serial_Log.println(pB);
          //                    Serial_Log.println(Dis_plot_num[num]);
          //                    Serial_Log.println("----------------------");

          Dis_plot_end[num * 2] = dA;
          Dis_plot_end[num * 2 + 1] = dB;

          Dis_plot_draw[num * 2] = pA;
          Dis_plot_draw[num * 2 + 1] = pB;

          Dis_data_avg[num * 2] = 0;
          Dis_data_avg[num * 2 + 1] = 0;

          Dis_plot_num[num]++;
        }
        Dis_data_num[num]++;
      }
      else {
        Dis_data_num[num] = 0;

        for (int i = 8; i >= 0; i--) {
          Dis_data_base[num * 2][i + 1] = Dis_data_base[num * 2][i];
          Dis_data_base[num * 2 + 1][i + 1] = Dis_data_base[num * 2 + 1][i];
        }
        Dis_data_base[num * 2][0] = SPI_ADCdata[num * 2];
        Dis_data_base[num * 2 + 1][0] = SPI_ADCdata[num * 2 + 1];
      }
    }
    else {
      if (reset == true) {
        for (int i = 0; i < 260; i++) {
          genie.WriteObject(Dis_PlotImg_Name, (num), 0);
          genie.WriteObject(Dis_PlotImg_Name, (num), 0);
        }
      }
      Dis_data_avg[num * 2] = 0;
      Dis_data_avg[num * 2 + 1] = 0;

      Dis_plot_zero[num * 2] = 0;
      Dis_plot_zero[num * 2 + 1] = 0;

      Dis_plot_end[num * 2] = 0;
      Dis_plot_end[num * 2 + 1] = 0;

      Dis_data_num[num] = 0;
      Dis_plot_num[num] = 0;
    }
  }
}

void Dis_LEDtrigger(int num) {
  genie.ReadObject(Dis_LEDtrigger_Name, num);
}

void Display_myGenieEventHandler(void)
{
  genieFrame Event;
  genie.DequeueEvent(&Event); // Remove the next queued event from the buffer, and process it below

  //If the cmd received is from a Reported Event (Events triggered from the Events tab of Workshop4 objects)
  if (Event.reportObject.cmd == GENIE_REPORT_EVENT) {
  }

  //If the cmd received is from a Reported Object, which occurs if a Read Object (genie.ReadOject) is requested in the main code, reply processed here.
  if (Event.reportObject.cmd == GENIE_REPORT_OBJ) {
    if (Event.reportObject.object == Dis_LEDtrigger_Name) {
      int num = Event.reportObject.index;
      LED_TurnOn[num] = genie.GetEventData(&Event);
    }
  }
}
