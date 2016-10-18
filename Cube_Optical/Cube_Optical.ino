#include <PID_v1.h>
#include <Event.h>
#include <Timer.h>
#include <SPI.h>

/*  Version   修改內容
    1.00      初版
*/
#define Version         1.00

//Pin define-----------------
//Analog Pin
#define TIC0            A0
#define TIC1            A1
#define TIC2            A2
#define TIC3            A3
#define TIC4            A4
#define TIC5            A5
#define AREF_3V3        A15

//Digital Pin
#define Heater_0        3
#define Heater_1        2
#define Heater_2        5
#define Heater_3        4

#define LED_0           6
#define LED_1           7
#define LED_2           8
#define LED_3           9

#define Fan             10
#define Buzzer          11

#define Button_0        22
#define Button_1        23
#define Button_2        24
#define Button_3        25

//SPI Pin: 50 (MISO), 51 (MOSI), 52 (SCK), 53 (SS)
#define  SSPin          53

int   TIC_pin[6] = {TIC0, TIC1, TIC2, TIC3, TIC4, TIC5};
int   Heater_pin[4] = {Heater_0, Heater_1, Heater_2, Heater_3};
int   LED_pin[4] = {LED_0, LED_1, LED_2, LED_3};
int   button_pin[4] = {Button_0, Button_1, Button_2, Button_3};

//Temp-----------------------
#define TempIC_Diff_0   0
#define TempIC_Diff_1   0
#define TempIC_Diff_2   0
#define TempIC_Diff_3   0
#define TempIC_Diff_4   0
#define TempIC_Diff_5   0

#define TempIC_base     400
#define TempIC_reso     19.5

#define TempTarRange    5
#define TempSampleTimes 5

int     ADC3V3 =          666;
double  Temp[6] =         {0, 0, 0, 0, 0, 0};
double  Tar[4] =          {0, 0, 0, 0};
double  Temp_diff[6] =    {TempIC_Diff_0, TempIC_Diff_1, TempIC_Diff_2, TempIC_Diff_3, TempIC_Diff_4, TempIC_Diff_5};
bool    Temp_steady[4] =  {false, false, false, false};

//Serial-----------------------
#define Serial_Log Serial
#define Serial_Dis Serial1
#define Baudrate  250000
bool    LogPrint_en = true;

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
//Default Setting
#define HeatingTime_Def       900   //PCR反應時間(含預熱時間)
#define PreHeatingTime_Def    60    //預熱時間
#define ResponseTime_Def      HeatingTime_Def - PreHeatingTime_Def
#define PreHeatingTemp_Def    120   //預熱溫度
#define HeatingTemp_Max_Def   100   //PCR反應溫度
#define HeatingTemp_Min_Def   95    //PCR反應溫度

double  HeatingTime[4]        = {HeatingTime_Def, HeatingTime_Def, HeatingTime_Def, HeatingTime_Def};
double  ResponseTime[4]       = {ResponseTime_Def, ResponseTime_Def, ResponseTime_Def, ResponseTime_Def};
double  PreHeatingTemp[4]     = {PreHeatingTemp_Def, PreHeatingTemp_Def, PreHeatingTemp_Def, PreHeatingTemp_Def};
double  HeatingTemp_Max[4]    = {HeatingTemp_Max_Def, HeatingTemp_Max_Def, HeatingTemp_Max_Def, HeatingTemp_Max_Def};
double  HeatingTemp_Min[4]    = {HeatingTemp_Min_Def, HeatingTemp_Min_Def, HeatingTemp_Min_Def, HeatingTemp_Min_Def};

#define MaxTemp               135   //過熱關閉加熱器溫度

bool  Heating_Begin[4] = {false, false, false, false};
int   HeatingTime_Counter[4] = {(-1), (-1), (-1), (-1)};

//Button---------------------
#define button_disable_delay  2

bool  button[4] = {false, false, false, false};
int   button_disable_counter[4] = {0, 0, 0, 0};

//LED-----------------------
#define LED_EnableTimes     15
#define LED_OffTimes        2
#define LED_OnTimes         LED_EnableTimes - LED_OffTimes
#define LED_CycleSec        60
#define LED_OnSec           5

int   LED_SecCounter[4];
int   LED_CycleCounter[4];
bool  LEDonoff[4];

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

int ChannelPin[8] = {Well_0_A, Well_0_B, Well_1_A, Well_1_B, Well_2_A, Well_2_B, Well_3_A, Well_3_B};
unsigned int SPI_ADCdata[8];

//Timer----------------------
#define PIDTime         2
#define secTime         1000

Timer timer;

void setup() {
  // put your setup code here, to run once:
  Fan_setup();
  Serial_setup();
  TempIC_setup();
  Buzzer_setup();
  Button_setup();
  Led_setup();
  PID_setup();
  SPI_setup();
  Timer_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.update();
}
