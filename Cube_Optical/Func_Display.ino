void Display_setup() {
  wdt_reset();
  Serial_Dis.begin(Baudrate_Dis);
  Serial_Log.println("-----------------------------Display Moudle Check");
  Display_CheckModule();
  if (Display_Module) {
    Display_Reset();
    genie.Begin(Serial_Dis);
    genie.AttachEventHandler(Display_myGenieEventHandler);
    genie.WriteContrast(15);
    for (int i = 0; i < 4; i++)
      genie.WriteObject(Dis_OBJ_USERIMAGES, (i * 2), 0 + Dis_ResultImg_Type * 5);
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
    int precent = 0, output = 0;
    if (HeatingTime_Counter[num] >= 0)
      precent = 100 - (HeatingTime_Counter[num] / HeatingTime[num] * 100);
    else if (HeatingTime_Counter[num] == Heating_fin_tag)
      precent = 100;

    output = precent / 10;
    genie.WriteObject(Dis_OBJ_USERIMAGES, num + Dis_Progressbar_Indexbase, output);
  }
}

void Display_Status(int num) {
  if (Display_Module) {
    int Dis_Status = Dis_Status_PreHeater;
    if (HeatingTime_Counter[num] == Heating_beg_tag) {
      if (Temp_steady[num] == true && Heating_Ready[num] == true)
        Dis_Status = Dis_Status_Standby;
      else
        Dis_Status = Dis_Status_PreHeater;
    }
    else if (HeatingTime_Counter[num] >= 0) {
      if ((HeatingTime_Counter[num] / FuncFreq) % 2 == 0)
        Dis_Status = Dis_Status_Reaction1;
      else
        Dis_Status = Dis_Status_Reaction2;
    }
    else if (HeatingTime_Counter[num] == Heating_fin_tag) {
      Dis_Status = Dis_Status_Finish;
    }
    genie.WriteObject(Dis_OBJ_USERIMAGES, num + Dis_Status_Indexbase, Dis_Status);
  }
}

void Display_ResultImg(int num, bool reset) {
  double DA, DB;
  int RAB = Dis_ResultImg_D_Def;
  if (Display_Module) {
    DA = Dis_Sigma[num * 2];
    DB = Dis_Sigma[num * 2 + 1];

    if (DB == 0)
      DB = 1;
    double Dab = DA / DB;

    if (reset == false) {
      if (Dis_ResultImg_Type == 0) {
        if (DA < Dis_plot_Gate[0] && DB < Dis_plot_Gate[1]) {
          RAB = Dis_ResultImg_D_NN;
        }
        else if (DA < Dis_plot_Gate[0] && DB >= Dis_plot_Gate[1]) {
          RAB = Dis_ResultImg_D_NP;
        }
        else if (DA >= Dis_plot_Gate[0] && DB < Dis_plot_Gate[1]) {
          RAB = Dis_ResultImg_D_PN;
        }
        else if (DA >= Dis_plot_Gate[0] && DB >= Dis_plot_Gate[1]) {
          if (Dab >= Dis_plot_Ratio[0]) {
            RAB = Dis_ResultImg_D_PN;
          }
          else if (Dab < Dis_plot_Ratio[0] && Dab >= Dis_plot_Ratio[1]) {
            RAB = Dis_ResultImg_D_PP;
          }
          else if (Dab < Dis_plot_Ratio[1]) {
            RAB = Dis_ResultImg_D_NP;
          }
        }
      }
      else {
        if (DA < Dis_plot_Gate[0] && DB < Dis_plot_Gate[1]) {
          RAB = Dis_ResultImg_S_NN;
        }
        else if (DA < Dis_plot_Gate[0] && DB >= Dis_plot_Gate[1]) {
          RAB = Dis_ResultImg_S_NP;
        }
        else if (DA >= Dis_plot_Gate[0] && DB < Dis_plot_Gate[1]) {
          RAB = Dis_ResultImg_S_PN;
        }
        else if (DA >= Dis_plot_Gate[0] && DB >= Dis_plot_Gate[1]) {
          if (Dab >= Dis_plot_Ratio[0]) {
            RAB = Dis_ResultImg_S_PN;
          }
          else if (Dab < Dis_plot_Ratio[0] && Dab >= Dis_plot_Ratio[1]) {
            RAB = Dis_ResultImg_S_PP;
          }
          else if (Dab < Dis_plot_Ratio[1]) {
            RAB = Dis_ResultImg_S_NP;
          }
        }
      }
    }
    genie.WriteObject(Dis_OBJ_USERIMAGES, (num * 2), RAB);
  }
  if (reset == false) {
    Save_data_Judgment[num] = RAB;
    Save_data_Judgment_output[num] = true;
  }
}

void Display_RealTempDig(int num) {
  if (Display_Module) {
    genie.WriteObject(Dis_OBJ_LED_DIGITS, num, Temp[num]);
  }
}

void Display_ConstDig(int num) {
  if (Display_Module) {
    genie.WriteObject(Dis_OBJ_CUSTOM_DIGITS, (num * 3), Tar[num]);
    genie.WriteObject(Dis_OBJ_CUSTOM_DIGITS, (num * 3 + 1), Temp_diff[num] * 10);
    genie.WriteObject(Dis_OBJ_CUSTOM_DIGITS, (num * 3 + 2), PD_Cons[num] * 100);
    genie.WriteObject(Dis_OBJ_CUSTOM_DIGITS, (num + 12), int(HeatingTime_Counter[num] / FuncFreq));
    genie.WriteObject(Dis_OBJ_LED_DIGITS, Dis_PreH_Def_Index, PreHeatingTemp[num]);
    genie.WriteObject(Dis_OBJ_LED_DIGITS, Dis_Tr_Def_Index, HeatingTemp_Min[num]);
    for (int i = 0; i < 8; i++)
      genie.WriteObject(Dis_OBJ_LED_DIGITS, Dis_PDADC_Index + i, SPI_ADCdata[i]);
  }
}

void Display_PlotImg(int num, bool reset) {
  if (Display_Module) {
    if (HeatingTime_Counter[num] >= 0 && reset == false) {
      if (LED_onoff[num] == true) {
        if (Dis_data_num[num] == 0) {
          Dis_data_avg[num * 2] = 0;
          Dis_data_avg[num * 2 + 1] = 0;
        }
        else if (Dis_data_num[num] >= 10 && Dis_data_num[num] < 20) {
          Dis_data_avg[num * 2] += double(SPI_ADCdata[num * 2]) / 10;
          Dis_data_avg[num * 2 + 1] += double(SPI_ADCdata[num * 2 + 1]) / 10;
        }
        else if (Dis_data_num[num] == 20) {
          if (Dis_plot_num[num] == 0) {
            Dis_data_avg_prev[num * 2] = Dis_data_avg[num * 2];
            Dis_data_avg_prev[num * 2 + 1] = Dis_data_avg[num * 2 + 1];
            Dis_plot[num * 2] = 0;
            Dis_plot[num * 2 + 1] = 0;
          }

          double deltaA = Dis_data_avg[num * 2] - Dis_data_avg_prev[num * 2];
          double deltaB = Dis_data_avg[num * 2 + 1] - Dis_data_avg_prev[num * 2 + 1];
          Dis_data_avg_prev[num * 2] = Dis_data_avg[num * 2];
          Dis_data_avg_prev[num * 2 + 1] = Dis_data_avg[num * 2 + 1];
          if (deltaA >= 0)
            deltaB = deltaB - (deltaA * PD_Cons[num]);

          double pA = deltaA / double(Dis_ADCcon_Def) * double(Dis_Plot_High_posi);
          double pB = deltaB / double(Dis_ADCcon_Def) * double(Dis_Plot_High_posi);
          Dis_plot[num * 2] += pA;
          Dis_plot[num * 2 + 1] += pB;

          int plotpoints = ((int)HeatingTime[num] - (int)LED_MaskSec_array[num]) / (int)LED_CycleSec_array[num];
          int pointsize = int(double(Dis_Plot_Width) / plotpoints);
          if (pointsize == 0)
            pointsize = 1;
          for (int i = 0; i < pointsize; i++) {
            genie.WriteObject(Dis_OBJ_SCOPE, (num), int(Dis_plot[num * 2]));
            genie.WriteObject(Dis_OBJ_SCOPE, (num), int(Dis_plot[num * 2 + 1]));
          }

          if ((Dis_Sigma[num * 2] >= Dis_sigma_Gate) || (deltaA >= Dis_delta_Gate))
            Dis_Sigma[num * 2] += deltaA;
          else
            Dis_Sigma[num * 2] = 0;

          if ((Dis_Sigma[num * 2 + 1] >= Dis_sigma_Gate) || (deltaB >= Dis_delta_Gate))
            Dis_Sigma[num * 2 + 1] += deltaB;
          else
            Dis_Sigma[num * 2 + 1] = 0;

          int TimeSec = ((Dis_plot_num[num] + 1) * LED_CycleSec + LED_MaskSec) / FuncFreq;
          SaveData_Get_Data_Str(num, TimeSec,
                                Dis_data_avg[num * 2], deltaA, Dis_Sigma[num * 2], Dis_plot[num * 2],
                                Dis_data_avg[num * 2 + 1], deltaB, Dis_Sigma[num * 2 + 1], Dis_plot[num * 2 + 1],
                                LED_onoff[num], digitalRead(LED_pin[num]),
                                Temp[num], Temp[4], Temp[5], (freeRam()));
          Save_data_Ready[num] = true;
          Dis_plot_num[num]++;
        }
        Dis_data_num[num]++;
      }
      else
        Dis_data_num[num] = 0;
    }
    else {
      if (reset == true) {
        for (int i = 0; i < Dis_Plot_Width; i++) {
          genie.WriteObject(Dis_OBJ_SCOPE, (num), 0);
          genie.WriteObject(Dis_OBJ_SCOPE, (num), 0);
        }
      }

      Dis_data_avg[num * 2] = 0;
      Dis_data_avg[num * 2 + 1] = 0;

      Dis_data_avg_prev[num * 2] = 0;
      Dis_data_avg_prev[num * 2 + 1] = 0;

      Dis_Sigma[num * 2] = 0;
      Dis_Sigma[num * 2 + 1] = 0;

      Dis_plot[num * 2] = 0;
      Dis_plot[num * 2 + 1] = 0;

      Dis_data_num[num] = 0;
      Dis_plot_num[num] = 0;
    }
  }
}

void Dis_LEDtrigger(int num) {
  genie.ReadObject(Dis_OBJ_4DBUTTON, num);
  genie.ReadObject(Dis_OBJ_4DBUTTON, num + Dis_LED_Index);
}

void Dis_Settrigger() {
  genie.ReadObject(Dis_OBJ_4DBUTTON, 4);
  genie.ReadObject(Dis_OBJ_4DBUTTON, 5);
}

void Dis_GetTpreH() {
  genie.ReadObject(Dis_OBJ_CUSTOM_DIGITS, Dis_PreH_new_Index);
}

void Dis_GetTtar() {
  genie.ReadObject(Dis_OBJ_CUSTOM_DIGITS, Dis_Tr_new_Index);
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
    if (Event.reportObject.object == Dis_OBJ_4DBUTTON) {
      int index = Event.reportObject.index;
      int ret = genie.GetEventData(&Event);
      if (index < 4)
        LED_TurnOn[index] = ret;
      else if (index >= (0 + Dis_LED_Index) && index < (4 + Dis_LED_Index))
        LED_TurnOn[index - Dis_LED_Index] = ret;
      else if (index == 4 && ret == true) {
        Dis_GetTpreH();
        Dis_GetTtar();
      }
      else if (index == 5) {
        for (int i = 0; i < 4; i++) {
          if (button[i] == false && Dis_ResultImg_Type != ret)
            genie.WriteObject(Dis_OBJ_USERIMAGES, (i * 2), Dis_ResultImg_D_Def + ret * 5);
        }
        Dis_ResultImg_Type = ret;
      }
    }
    else if (Event.reportObject.object == Dis_OBJ_CUSTOM_DIGITS) {
      int index = Event.reportObject.index;
      int ret = genie.GetEventData(&Event);
      if (index == Dis_PreH_new_Index) {
        for (int num = 0; num < 4; num++) {
          if (HeatingTime_Counter[num] < 0) {
            PreHeatingTemp[num] = ret;
          }
        }
      }
      else if (index == Dis_Tr_new_Index) {
        for (int num = 0; num < 4; num++) {
          if (HeatingTime_Counter[num] < 0) {
            HeatingTemp_Max[num] = ret;
            HeatingTemp_Min[num] = ret;
          }
        }
      }
    }
  }
}
