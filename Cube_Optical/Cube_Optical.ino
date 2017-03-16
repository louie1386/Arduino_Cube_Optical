#include <cubeled.h>
#include <avr/wdt.h>
#include <fprot.h>
#include <genieArduino.h>
#include <PID_v1.h>
#include <Event.h>
#include <Timer.h>
#include <SPI.h>
#include <Custom_Setting.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

/*  Version   修改內容
    1.00      初版(含目標溫度震盪功能)
    1.01      新增 EEPROM 功能
    1.02      新增 SD Card 功能
    1.03      新增 Watchdog 功能
    1.04      修正溫度讀取, SD card與EEPROM bug
    1.05      修改LED控制為動態設定
    1.06      新增待機溫度與強制開燈變數
    1.07      新增4Dsystem 倒數計時顯示
    1.08      新增4Dsystem 溫度設定顯示
    1.09      修改4Dsystem GUI與SD卡存檔修正
    1.10      新增上風扇控制與修改結果判斷公式
    2.00      修改儲存裝置為USB disk，修改演算法，修改LED控制
*/
#define Version         "2.00."
#define subVersion      "03.15.01"

//Function Simulation-----------------
bool ADC_simulation = true;
bool Temp_simulation = true;

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

#define USB_INT_pin     38
#define USB_RST_pin     40
#define USB_RXD_pin     16
#define USB_TXD_pin     17

//SPI Pin:
//MISO  50
//MOSI  51
//SCK   52
#define  SSPin          53  //PB0, P19

#define  Dis_Module     19  //RX1
//#define  SD_Module      17  //RX2

int   TIC_pin[6] = {TIC0, TIC1, TIC2, TIC3, TIC4, TIC5};
int   Heater_pin[4] = {Heater_0, Heater_1, Heater_2, Heater_3};
int   LED_pin[4] = {LED_0, LED_1, LED_2, LED_3};
int   button_pin[4] = {Button_0, Button_1, Button_2, Button_3};

//Timer----------------------
#define CycTime         100
#define SecTime         1000
#define FuncFreq        (SecTime / CycTime)

Timer timer;
int FuncFreq_num = 0;
unsigned long TimeStart[4], TimeEnd[4];
//Temp-----------------------
#define TempIC_Diff_0   TempIC_Diff_0_Custom
#define TempIC_Diff_1   TempIC_Diff_1_Custom
#define TempIC_Diff_2   TempIC_Diff_2_Custom
#define TempIC_Diff_3   TempIC_Diff_3_Custom

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
//#define Baudrate_Sav_Def  9600
//#define Baudrate_Sav_New  115200
#define Baudrate_Sav  115200

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

#define dKp   dKp_Custom
#define dKi   dKi_Custom
#define dKd   dKd_Custom

double  Volt[4] = {0, 0, 0, 0};
int     PIDnum = 0;

PID PID0(&Temp[0], &Volt[0], &Tar[0], dKp, dKi, dKd, DIRECT);
PID PID1(&Temp[1], &Volt[1], &Tar[1], dKp, dKi, dKd, DIRECT);
PID PID2(&Temp[2], &Volt[2], &Tar[2], dKp, dKi, dKd, DIRECT);
PID PID3(&Temp[3], &Volt[3], &Tar[3], dKp, dKi, dKd, DIRECT);

//Heater--------------------
#define WarmUpTime_Def        WarmUpTime_Def_Custom * FuncFreq
#define HeatingTime_Def       HeatingTime_Custom * FuncFreq
#define PreHeatingTime_Def    PreHeatingTime_Custom * FuncFreq
#define ResponseTime_Def      HeatingTime_Def - PreHeatingTime_Def

#define PreHeatingTemp_Def    PreHeatingTemp_Custom
#define StandbyTemp_Def       StandbyTemp_Custom
#define HeatingTemp_Max_Def   HeatingTemp_Max_Custom
#define HeatingTemp_Min_Def   HeatingTemp_Min_Custom

#define Boost1_Diff           Boost1_Diff_Custom
#define Boost2_Diff           Boost2_Diff_Custom
#define Boost3_Diff           Boost3_Diff_Custom
#define Boost4_Diff           Boost4_Diff_Custom

double  HeatingTime[4]        = {HeatingTime_Def, HeatingTime_Def, HeatingTime_Def, HeatingTime_Def};
double  ResponseTime[4]       = {ResponseTime_Def, ResponseTime_Def, ResponseTime_Def, ResponseTime_Def};
double  BoostTemp_Diff[4]     = {Boost1_Diff, Boost2_Diff, Boost3_Diff, Boost4_Diff};
double  PreHeatingTemp[4]     = {PreHeatingTemp_Def, PreHeatingTemp_Def, PreHeatingTemp_Def, PreHeatingTemp_Def};
double  HeatingTemp_Max[4]    = {HeatingTemp_Max_Def, HeatingTemp_Max_Def, HeatingTemp_Max_Def, HeatingTemp_Max_Def};
double  HeatingTemp_Min[4]    = {HeatingTemp_Min_Def, HeatingTemp_Min_Def, HeatingTemp_Min_Def, HeatingTemp_Min_Def};

#define MaxTemp               135
#define Heating_beg_tag       (-1)
#define Heating_fin_tag       (-2)

bool  Heating_Begin[4] = {false, false, false, false};
bool  Heating_Ready[4] = {false, false, false, false};
int   HeatingTime_Counter[4] = {Heating_beg_tag, Heating_beg_tag, Heating_beg_tag, Heating_beg_tag};
int   WarmUpTime_Counter[4] = {WarmUpTime_Def, WarmUpTime_Def, WarmUpTime_Def, WarmUpTime_Def};

//Button---------------------
#define button_disable_delay  2 * FuncFreq

bool  button[4] = {false, false, false, false};
int   button_disable_counter[4] = {0, 0, 0, 0};

//LED-----------------------
#define LED_MaskSec         120 * FuncFreq
#define LED_CycleSec        10 * FuncFreq
#define LED_OnSec           2.1 * FuncFreq

int   LED_MaskSec_array[4] = {LED_MaskSec, LED_MaskSec, LED_MaskSec, LED_MaskSec};
int   LED_CycleSec_array[4] = {LED_CycleSec, LED_CycleSec, LED_CycleSec, LED_CycleSec};
int   LED_OnSec_array[4] = {LED_OnSec, LED_OnSec, LED_OnSec, LED_OnSec};
bool  LED_onoff[4] = {false, false, false, false};
bool  LED_TurnOn[4] = {false, false, false, false};

CubeLed CL0, CL1, CL2, CL3;

//SPI_ADC--------------------
#define Well_0_A            7
#define Well_0_B            0
#define Well_1_A            5
#define Well_1_B            6
#define Well_2_A            3
#define Well_2_B            4
#define Well_3_A            1
#define Well_3_B            2

#define  StartBit           B10000000
#define  baseBit            B10000
#define  RangeSelectBits    B0110
#define  ConvStartBits      B0000

#define  PD_Cons_0          PD_Cons_0_Custom
#define  PD_Cons_1          PD_Cons_1_Custom
#define  PD_Cons_2          PD_Cons_2_Custom
#define  PD_Cons_3          PD_Cons_3_Custom

int     ChannelPin[8] = {Well_0_A, Well_0_B, Well_1_A, Well_1_B, Well_2_A, Well_2_B, Well_3_A, Well_3_B};
unsigned int SPI_ADCdata[8];
double  PD_Cons[4] = {PD_Cons_0, PD_Cons_1, PD_Cons_2, PD_Cons_3};

//Data Simulation---------------------
#define DS_LED_OFF_base_A   300
#define DS_LED_OFF_base_B   300
#define DS_LED_ON_base_A    1500
#define DS_LED_ON_base_B    1000
#define DS_shift_A          0
#define DS_shift_B          0
#define DS_increments_A     15
#define DS_increments_B     20

#define DS_shift_Time_A           0
#define DS_shift_Time_B           0
#define DS_increments_Time_A      41
#define DS_increments_Time_B      42

bool    DS_LED_Prev[4] = {false, false, false, false};
int     DS_LED_On_num[4] = {0, 0, 0, 0};

//Display---------------------
#define Dis_OBJ_GAUGE           GENIE_OBJ_GAUGE
#define Dis_OBJ_LED             GENIE_OBJ_LED
#define Dis_OBJ_USERIMAGES      GENIE_OBJ_USERIMAGES
#define Dis_OBJ_LED_DIGITS      GENIE_OBJ_LED_DIGITS
#define Dis_OBJ_CUSTOM_DIGITS   GENIE_OBJ_CUSTOM_DIGITS
#define Dis_OBJ_SCOPE           GENIE_OBJ_SCOPE
#define Dis_OBJ_4DBUTTON        GENIE_OBJ_4DBUTTON

#define Dis_Progressbar_Indexbase   8
#define Dis_Status_Indexbase        12

#define Dis_PreH_Def_Index          4
#define Dis_Tr_Def_Index            5
#define Dis_PreH_new_Index          16
#define Dis_Tr_new_Index            17

#define Dis_PDADC_Index             6
#define Dis_LED_Index               6

#define Dis_ADCcon_Def        2000

#define Dis_Status_PreHeater  0
#define Dis_Status_Standby    1
#define Dis_Status_Reaction1  2
#define Dis_Status_Reaction2  3
#define Dis_Status_Finish     4

#define Dis_ResultImg_D_Def     0
#define Dis_ResultImg_D_PP      1
#define Dis_ResultImg_D_PN      2
#define Dis_ResultImg_D_NP      3
#define Dis_ResultImg_D_NN      4
#define Dis_ResultImg_S_Def     5
#define Dis_ResultImg_S_PP      6
#define Dis_ResultImg_S_PN      7
#define Dis_ResultImg_S_NP      8
#define Dis_ResultImg_S_NN      9

#define Dis_delta_Gate        Dis_delta_Gate_Custom
#define Dis_sigma_Gate        Dis_sigma_Gate_Custom
#define Dis_pA_Gate_Def       Dis_pA_Gate_Def_Custom
#define Dis_pB_Gate_Def       Dis_pB_Gate_Def_Custom
#define Dis_Ratio_Max         Dis_Ratio_Max_Custom
#define Dis_Ratio_Min         Dis_Ratio_Min_Custom

#define Dis_Plot_Width        260
#define Dis_Plot_High_posi    150

Genie genie;

double  Dis_plot_Gate[2] = {Dis_pA_Gate_Def, Dis_pB_Gate_Def};
double  Dis_plot_Ratio[2] = {Dis_Ratio_Max, Dis_Ratio_Min};

double  Dis_data_avg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
double  Dis_data_avg_prev[8] = {0, 0, 0, 0, 0, 0, 0, 0};
double  Dis_Sigma[8] = {0, 0, 0, 0, 0, 0, 0, 0};
double  Dis_plot[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int     Dis_data_num[4] = {0, 0, 0, 0};
int     Dis_plot_num[4] = {0, 0, 0, 0};
int     Dis_ResultImg_Type = Dis_ResultImg_Type_Custom;
bool    Display_Module = false;

//Save data--------------------
#define SaveSettingCharSize 150
#define SaveSettingItems    8
#define SaveDataCharSize    150
#define SaveDataItems       15
String  Save_DirName = "/LOG";
String  Save_FileName = "WNLOG000.CSV";
String  Save_RealFileName[4] = {Save_FileName, Save_FileName, Save_FileName, Save_FileName};
char    Save_Setting[] = "Well, Light Constant, Delta Gate, Sigma Gate, Result Gate, Max Ratio, Min Ratio, Total Time\r\n";
char    Save_Setting_str[4][SaveSettingCharSize];
char    Save_Title[] = "Time, Avg_A, Delta_A, Sigma_A, Plot_A, Avg_B, Delta_B, Sigma_B, Plot_B, LED(SW), LED(HW), WellTemp, LowerTemp, UpperTemp, FreeRam\r\n";
char    Save_data_str[4][SaveDataCharSize];
bool    Save_data_Ready[4] = {false, false, false, false};
bool    Save_data_print = false;
bool    Save_data_output_finish[4] = {false, false, false, false};
bool    Save_data_Judgment_output[4] = {false, false, false, false};
int     Save_data_Judgment[4];

//USB Disk --------------------
#define USB_CMD_tag0            0x57
#define USB_CMD_tag1            0xAB

#define USB_CMD_RESET_ALL       0x05
#define USB_CMD_CHECK_EXIST     0x06
#define USB_CMD_SET_USB_MODE    0x15
#define USB_CMD_GET_STATUS      0x22
#define USB_CMD_RD_USB_DATA0    0x27
#define USB_CMD_WR_REQ_DATA     0x2D
#define USB_CMD_WR_OFS_DATA     0x2E
#define USB_CMD_SET_FILE_NAME   0x2F
#define USB_CMD_DISK_CONNECT    0x30
#define USB_CMD_DISK_MOUNT      0x31
#define USB_CMD_FILE_OPEN       0x32
#define USB_CMD_FILE_CREATE     0x34
#define USB_CMD_FILE_CLOSE      0x36
#define USB_CMD_DIR_INFO_READ   0x37
#define USB_CMD_DIR_INFO_SAVE   0x38
#define USB_CMD_BYTE_LOCATE     0x39
#define USB_CMD_BYTE_WRITE      0x3C
#define USB_CMD_BYTE_WR_GO      0x3D
#define USB_CMD_DIR_CREATE      0x40

#define USB_CMD_RET_SUCCESS     0x51
#define USB_CMD_RET_ABORT       0x5F

#define USB_INT_SUCCESS         0x14
#define USB_INT_CONNECT         0x15
#define USB_INT_DISCONNECT      0x16
#define USB_INT_DISK_WRITE      0x1E

#define USB_ERR_OPEN_DIR        0x41
#define USB_ERR_MISS_FILE       0x42
#define USB_ERR_DISK_DISCON     0x82

#define USB_FAT32_Setuptime_addr      0x0E
#define USB_FAT32_Setupdate_addr      0x10
#define USB_FAT32_Accessdate_addr     0x12
#define USB_FAT32_Modifytime_addr     0x16
#define USB_FAT32_Modifydate_addr     0x18

byte USB_RET[50];
bool USB_INT = false;
bool USB_Module = false;
bool USB_Disk_In = false;
bool USB_debug_print = false;

//RTC--------------------------
tmElements_t tm;
int RTC_Hour = 00;
int RTC_Min = 00;
int RTC_Sec = 00;
int RTC_Day = 13;
int RTC_Month = 3;
int RTC_Year = 2017;
bool RTC_print = false;

tmElements_t RTCtm;

//Fan--------------------------
#define Fan_Gate0_Temp      Fan_Gate0_Temp_Custom
#define Fan_Gate1_Temp      Fan_Gate1_Temp_Custom
#define Fan_Gate2_Temp      Fan_Gate2_Temp_Custom
#define Fan_Gate3_Temp      Fan_Gate3_Temp_Custom

#define Fan_PWM_LowPower0   Fan_PWM_LowPower0_Custom
#define Fan_PWM_LowPower1   Fan_PWM_LowPower1_Custom
#define Fan_PWM_FullPower   Fan_PWM_FullPower_Custom

int Fan_PWM_now = Fan_PWM_LowPower0;

//FreeRam--------------------------
int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setup() {
  // put your setup code here, to run once:
  wdt_enable(WDTO_4S);
  Serial_setup();
  TempIC_setup();
  Button_setup();
  LED_setup();
  PID_setup();
  ADC_setup();
  SavaData_setup();
  Display_setup();
  Fan_setup();
  Timer_setup();
  Buzzer_setup();
  RTC_setup();
  Serial_Log.println("-----------------------------Setup end!");
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.update();
  PID_Loop();
  USBdisk_INT();
  genie.DoEvents();
  wdt_reset();
}
