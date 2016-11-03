#include <avr/wdt.h>
#include <fprot.h>
#include <genieArduino.h>
#include <EEPROM.h>
#include <PID_v1.h>
#include <Event.h>
#include <Timer.h>
#include <SPI.h>

/*  Version   修改內容
    1.00      初版(含目標溫度震盪功能)
    1.01      新增 EEPROM 功能
    1.02      新增 SD Card 功能
    1.03      新增 Watchdog 功能
*/
#define Version         1.03

//Pin define-----------------
//Analog Pin
#define TIC0            A0  //PF0, P97
#define TIC1            A1  //PF1, P96
#define TIC2            A2  //PF2, P95
#define TIC3            A3  //PF3, P94
#define TIC4            A4  //PF4, P93
#define TIC5            A5  //PF5, P92
#define AREF_3V3        A15 //PK7, P82

//Digital Pin
#define Heater_0        3 //PE5, P7
#define Heater_1        2 //PE4, P6
#define Heater_2        5 //PE3, P5
#define Heater_3        4 //PG5, P1

#define LED_0           6 //PH3, P15
#define LED_1           7 //PH4, P16
#define LED_2           8 //PH5, P17
#define LED_3           9 //PH6, P18

#define Fan             10  //PB4, P23
#define Buzzer          11  //PB5, P24

#define Button_0        25  //PA3, P75
#define Button_1        24  //PA2, P76
#define Button_2        23  //PA1, P77
#define Button_3        22  //PA0, P78

#define DisReset        41  //PG0, P51
#define SavReset        38  //PD7, P50

//SPI Pin:
//MISO  50
//MOSI  51
//SCK   52
#define  SSPin          53  //PB0, P19

#define  SD_Module      17  //RX2

int   TIC_pin[6] = {TIC0, TIC1, TIC2, TIC3, TIC4, TIC5};
int   Heater_pin[4] = {Heater_0, Heater_1, Heater_2, Heater_3};
int   LED_pin[4] = {LED_0, LED_1, LED_2, LED_3};
int   button_pin[4] = {Button_0, Button_1, Button_2, Button_3};

//Timer----------------------
#define PIDTime         2
#define CycTime         100
#define SecTime         1000
#define SecCycles       (SecTime / CycTime)

Timer timer;
int Cycles = 0;

//Temp-----------------------
#define TempIC_Diff_0   0
#define TempIC_Diff_1   0
#define TempIC_Diff_2   0
#define TempIC_Diff_3   0

#define TempIC_base     400
#define TempIC_reso     19.5

#define TempTarRange    5
#define TempSampleTimes 5

int     ADC3V3 =          666;
double  Temp[6] =         {0, 0, 0, 0, 0, 0};
double  Tar[4] =          {0, 0, 0, 0};
double  Temp_diff[6] =    {TempIC_Diff_0, TempIC_Diff_1, TempIC_Diff_2, TempIC_Diff_3, 0, 0};
bool    Temp_steady[4] =  {false, false, false, false};

//Serial-----------------------
#define Serial_Log Serial
#define Serial_Dis Serial1
#define Serial_Sav Serial2
#define Baudrate_Log      250000
#define Baudrate_Dis      115200
#define Baudrate_Sav_Def  9600
#define Baudrate_Sav_New  115200

bool  LogPrint_en = false;
bool  LogEEPROM_en = false;
bool  LogEEPROM_addr_WriteIn = false;
bool  LogEEPROM_data_point = false;
int   LogEEPROM_addrnum = 0;
double   LogEEPROM_data = 0;

//Buzzer--------------------
#define buzzer_Hz 4978
#define buzzer_ms 500

//PID-----------------------
#define PIDSampleTime   100
#define PIDOutputLimit  255

#define dKp   200
#define dKi   20
#define dKd   0

double  Volt[4] = {0, 0, 0, 0};
double  Kp[4] =   {dKp, dKp, dKp, dKp};
double  Ki[4] =   {dKi, dKi, dKi, dKi};
double  Kd[4] =   {dKd, dKd, dKd, dKd};
int     PIDnum = 0;

PID PID0(&Temp[0], &Volt[0], &Tar[0], Kp[0], Ki[0], Kd[0], DIRECT);
PID PID1(&Temp[1], &Volt[1], &Tar[1], Kp[1], Ki[1], Kd[1], DIRECT);
PID PID2(&Temp[2], &Volt[2], &Tar[2], Kp[2], Ki[2], Kd[2], DIRECT);
PID PID3(&Temp[3], &Volt[3], &Tar[3], Kp[3], Ki[3], Kd[3], DIRECT);

//Heater--------------------
#define HeatingTime_Def       900   //PCR反應時間(含預熱時間)
#define PreHeatingTime_Def    60    //預熱時間
#define ResponseTime_Def      HeatingTime_Def - PreHeatingTime_Def

#define PreHeatingTemp_Def    120   //預熱溫度
#define HeatingTemp_Max_Def   100   //PCR反應溫度
#define HeatingTemp_Min_Def   90    //PCR反應溫度

double  HeatingTime[4]        = {HeatingTime_Def, HeatingTime_Def, HeatingTime_Def, HeatingTime_Def};
double  ResponseTime[4]       = {ResponseTime_Def, ResponseTime_Def, ResponseTime_Def, ResponseTime_Def};
double  PreHeatingTemp[4]     = {PreHeatingTemp_Def, PreHeatingTemp_Def, PreHeatingTemp_Def, PreHeatingTemp_Def};
double  HeatingTemp_Max[4]    = {HeatingTemp_Max_Def, HeatingTemp_Max_Def, HeatingTemp_Max_Def, HeatingTemp_Max_Def};
double  HeatingTemp_Min[4]    = {HeatingTemp_Min_Def, HeatingTemp_Min_Def, HeatingTemp_Min_Def, HeatingTemp_Min_Def};

#define MaxTemp               135   //過熱關閉加熱器溫度
#define Heating_beg_tag       (-1)
#define Heating_fin_tag       (-2)

bool  Heating_Begin[4] = {false, false, false, false};
int   HeatingTime_Counter[4] = {Heating_beg_tag, Heating_beg_tag, Heating_beg_tag, Heating_beg_tag};

//Button---------------------
#define button_disable_delay  2

bool  button[4] = {false, false, false, false};
int   button_disable_counter[4] = {0, 0, 0, 0};

//LED-----------------------
#define LED_EnableTimes     15
#define LED_CycleSec        (60 * SecCycles)
#define LED_OnSec           (5 * SecCycles)

int   LED_SecCounter[4] = {LED_CycleSec, LED_CycleSec, LED_CycleSec, LED_CycleSec};
int   LED_CycleCounter[4] = {LED_EnableTimes, LED_EnableTimes, LED_EnableTimes, LED_EnableTimes};
bool  LEDonoff[4] = {false, false, false, false};
bool  LEDEanbleType[LED_EnableTimes + 1] = {false, false, true, true, true,
                                            true, true, true, true, true,
                                            true, true, true, true, true, false
                                           };

//SPI_ADC--------------------
#define Well_0_A            7
#define Well_0_B            4
#define Well_1_A            1
#define Well_1_B            6
#define Well_2_A            3
#define Well_2_B            0
#define Well_3_A            5
#define Well_3_B            2

#define  StartBit           B10000000
#define  baseBit            B10000
#define  RangeSelectBits    B0110
#define  ConvStartBits      B0000

#define  PD_Cons_0          0
#define  PD_Cons_1          0
#define  PD_Cons_2          0
#define  PD_Cons_3          0

int ChannelPin[8] = {Well_0_A, Well_0_B, Well_1_A, Well_1_B, Well_2_A, Well_2_B, Well_3_A, Well_3_B};
unsigned int SPI_ADCdata[8];
double PD_Cons[4] = {PD_Cons_0, PD_Cons_1, PD_Cons_2, PD_Cons_3};

//Display---------------------
#define Dis_ProgressBar_Name  GENIE_OBJ_GAUGE
#define Dis_ReadyLed_Name     GENIE_OBJ_LED
#define Dis_ResultImg_Name    GENIE_OBJ_USERIMAGES
#define Dis_RealTempDig_Name  GENIE_OBJ_LED_DIGITS
#define Dis_ConstDig_Name     GENIE_OBJ_CUSTOM_DIGITS
#define Dis_PlotImg_Name      GENIE_OBJ_SCOPE

#define Dis_pA_Gate_Def       300
#define Dis_pB_Gate_Def       300
Genie genie;

double  Dis_plot_Gate[2] = {Dis_pA_Gate_Def, Dis_pB_Gate_Def};

double  Dis_data_avg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int     Dis_data_num[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int     Dis_plot_num[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int     Dis_plot_zero[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int     Dis_plot_end[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int     Dis_data_base[8][10] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
int     Dis_data_base_avg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int     Dis_plot_min[8] = {0, 0, 0, 0, 0, 0, 0, 0};
bool    Dis_plot_base_enable = true;

//EEPROM---------------------
#define Size_Double             sizeof(double)

#define EEPROM_WriteInByte      0xFF
#define EEPROM_WriteIn_addr     0x0000

#define HeatingTime_0_addr      0x0100
#define HeatingTime_1_addr      HeatingTime_0_addr + Size_Double
#define HeatingTime_2_addr      HeatingTime_1_addr + Size_Double
#define HeatingTime_3_addr      HeatingTime_2_addr + Size_Double

#define ResponseTime_0_addr     0x0200
#define ResponseTime_1_addr     ResponseTime_0_addr + Size_Double
#define ResponseTime_2_addr     ResponseTime_1_addr + Size_Double
#define ResponseTime_3_addr     ResponseTime_2_addr + Size_Double

#define PreHeatingTemp_0_addr   0x0300
#define PreHeatingTemp_1_addr   PreHeatingTemp_0_addr + Size_Double
#define PreHeatingTemp_2_addr   PreHeatingTemp_1_addr + Size_Double
#define PreHeatingTemp_3_addr   PreHeatingTemp_2_addr + Size_Double

#define HeatingTemp_Max_0_addr  0x0400
#define HeatingTemp_Max_1_addr  HeatingTemp_Max_0_addr + Size_Double
#define HeatingTemp_Max_2_addr  HeatingTemp_Max_1_addr + Size_Double
#define HeatingTemp_Max_3_addr  HeatingTemp_Max_2_addr + Size_Double

#define HeatingTemp_Min_0_addr  0x0500
#define HeatingTemp_Min_1_addr  HeatingTemp_Min_0_addr + Size_Double
#define HeatingTemp_Min_2_addr  HeatingTemp_Min_1_addr + Size_Double
#define HeatingTemp_Min_3_addr  HeatingTemp_Min_2_addr + Size_Double

#define Temp_diff_0_addr        0x0600
#define Temp_diff_1_addr        Temp_diff_0_addr + Size_Double
#define Temp_diff_2_addr        Temp_diff_1_addr + Size_Double
#define Temp_diff_3_addr        Temp_diff_2_addr + Size_Double

#define PD_Cons_0_addr          0x0700
#define PD_Cons_1_addr          PD_Cons_0_addr + Size_Double
#define PD_Cons_2_addr          PD_Cons_1_addr + Size_Double
#define PD_Cons_3_addr          PD_Cons_2_addr + Size_Double

#define Dis_plot_Gate_A_addr    0x0800
#define Dis_plot_Gate_B_addr    Dis_plot_Gate_A_addr + Size_Double

int HeatingTime_addr[4]     = {HeatingTime_0_addr, HeatingTime_1_addr, HeatingTime_2_addr, HeatingTime_3_addr};
int ResponseTime_addr[4]    = {ResponseTime_0_addr, ResponseTime_1_addr, ResponseTime_2_addr, ResponseTime_3_addr};
int PreHeatingTemp_addr[4]  = {PreHeatingTemp_0_addr, PreHeatingTemp_1_addr, PreHeatingTemp_2_addr, PreHeatingTemp_3_addr};
int HeatingTemp_Max_addr[4] = {HeatingTemp_Max_0_addr, HeatingTemp_Max_1_addr, HeatingTemp_Max_2_addr, HeatingTemp_Max_3_addr};
int HeatingTemp_Min_addr[4] = {HeatingTemp_Min_0_addr, HeatingTemp_Min_1_addr, HeatingTemp_Min_2_addr, HeatingTemp_Min_3_addr};
int Temp_diff_addr[4]       = {Temp_diff_0_addr, Temp_diff_1_addr, Temp_diff_2_addr, Temp_diff_3_addr};
int PD_Cons_addr[4]         = {PD_Cons_0_addr, PD_Cons_1_addr, PD_Cons_2_addr, PD_Cons_3_addr};
int Dis_plot_Gate_addr[2]   = {Dis_plot_Gate_A_addr, Dis_plot_Gate_B_addr};

#define EEPROM_itemnum      30
int EEPROM_addr[EEPROM_itemnum] = {
  HeatingTime_addr[0],       HeatingTime_addr[1],      HeatingTime_addr[2],      HeatingTime_addr[3],
  ResponseTime_addr[0],      ResponseTime_addr[1],     ResponseTime_addr[2],     ResponseTime_addr[3],
  PreHeatingTemp_addr[0],    PreHeatingTemp_addr[1],   PreHeatingTemp_addr[2],   PreHeatingTemp_addr[3],
  HeatingTemp_Max_addr[0],   HeatingTemp_Max_addr[1],  HeatingTemp_Max_addr[2],  HeatingTemp_Max_addr[3],
  HeatingTemp_Min_addr[0],   HeatingTemp_Min_addr[1],  HeatingTemp_Min_addr[2],  HeatingTemp_Min_addr[3],
  Temp_diff_addr[0],         Temp_diff_addr[1],        Temp_diff_addr[2],        Temp_diff_addr[3],
  PD_Cons_addr[0],           PD_Cons_addr[1],          PD_Cons_addr[2],          PD_Cons_addr[3],
  Dis_plot_Gate_addr[0],      Dis_plot_Gate_addr[1]
};

//Save data--------------------
char Save_file_dir[] = "\\log";
char Save_file_path[] = "\\log\\log_???_Well?.csv";

unsigned char Save_file_ID[4] = {0, 0, 0, 0};
int Save_file_num[4] = {0, 0, 0, 0};
bool Save_Module = false;
bool Save_cardin = false;

void setup() {
  // put your setup code here, to run once:
  EEPROM_setup();
  Serial_setup();
  Fan_setup();
  TempIC_setup();
  Buzzer_setup();
  Button_setup();
  Led_setup();
  PID_setup();
  SPI_setup();
  SavaData_setup();
  Display_setup();
  Timer_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.update();
}
