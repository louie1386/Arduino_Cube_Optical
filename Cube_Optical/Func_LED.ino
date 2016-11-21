void Led_setup() {
  CL0.setLED(LED_0, HeatingTime[0] * SecCycles, LED_MaskSec_array[0] * SecCycles, LED_CycleSec_array[0] * SecCycles, LED_OnSec_array[0] * SecCycles);
  CL1.setLED(LED_1, HeatingTime[1] * SecCycles, LED_MaskSec_array[1] * SecCycles, LED_CycleSec_array[1] * SecCycles, LED_OnSec_array[1] * SecCycles);
  CL2.setLED(LED_2, HeatingTime[2] * SecCycles, LED_MaskSec_array[2] * SecCycles, LED_CycleSec_array[2] * SecCycles, LED_OnSec_array[2] * SecCycles);
  CL3.setLED(LED_3, HeatingTime[3] * SecCycles, LED_MaskSec_array[3] * SecCycles, LED_CycleSec_array[3] * SecCycles, LED_OnSec_array[3] * SecCycles);
}

void LED_Switch(int num) {
  if (num == 0)
    LEDonoff[num] = CL0.updateLED(HeatingTime_Counter[num]);
  else if (num == 1)
    LEDonoff[num] = CL1.updateLED(HeatingTime_Counter[num]);
  else if (num == 2)
    LEDonoff[num] = CL2.updateLED(HeatingTime_Counter[num]);
  else if (num == 3)
    LEDonoff[num] = CL3.updateLED(HeatingTime_Counter[num]);
}


