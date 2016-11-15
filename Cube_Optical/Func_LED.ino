void Led_setup() {
  //  pinMode(LED_0, OUTPUT);
  //  pinMode(LED_1, OUTPUT);
  //  pinMode(LED_2, OUTPUT);
  //  pinMode(LED_3, OUTPUT);
  //  digitalWrite(LED_0, LOW);
  //  digitalWrite(LED_1, LOW);
  //  digitalWrite(LED_2, LOW);
  //  digitalWrite(LED_3, LOW);

  CL0.setLED(LED_0, HeatingTime[0] * SecCycles, LED_MaskSec, LED_CycleSec, LED_OnSec);
  CL1.setLED(LED_1, HeatingTime[1] * SecCycles, LED_MaskSec, LED_CycleSec, LED_OnSec);
  CL2.setLED(LED_2, HeatingTime[2] * SecCycles, LED_MaskSec, LED_CycleSec, LED_OnSec);
  CL3.setLED(LED_3, HeatingTime[3] * SecCycles, LED_MaskSec, LED_CycleSec, LED_OnSec);
}

void LED_Switch(int num) {
//  if (LED_SecCounter[num] > LED_OnSec)
//    LEDonoff[num] = false;
//  else if (LED_SecCounter[num] <= LED_OnSec) {
//    LEDonoff[num] = LEDEanbleType[LED_EnableTimes - LED_CycleCounter[num]];
//  }
//
//  if (button[num] == true) {
//    LED_SecCounter[num]--;
//    if (LED_SecCounter[num] == 0) {
//      LED_SecCounter[num] = LED_CycleSec;
//      if (LED_CycleCounter[num] > 0)
//        LED_CycleCounter[num]--;
//    }
//  }
//  else {
//    LED_SecCounter[num] = LED_CycleSec;
//    LED_CycleCounter[num] = LED_EnableTimes;
//  }
//
//  digitalWrite(LED_pin[num], LEDonoff[num]);

  if (num == 0)
    LEDonoff[num] = CL0.updateLED(HeatingTime_Counter[num]);
  else if (num == 1)
    LEDonoff[num] = CL1.updateLED(HeatingTime_Counter[num]);
  else if (num == 2)
    LEDonoff[num] = CL2.updateLED(HeatingTime_Counter[num]);
  else if (num == 3)
    LEDonoff[num] = CL3.updateLED(HeatingTime_Counter[num]);
}


