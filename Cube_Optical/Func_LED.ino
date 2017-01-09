void LED_setup() {
  wdt_reset();
  CL0.setLED(LED_pin[0], HeatingTime[0] * SecCycles, LED_MaskSec_array[0] * SecCycles, LED_CycleSec_array[0] * SecCycles, LED_OnSec_array[0] * SecCycles);
  CL1.setLED(LED_pin[1], HeatingTime[1] * SecCycles, LED_MaskSec_array[1] * SecCycles, LED_CycleSec_array[1] * SecCycles, LED_OnSec_array[1] * SecCycles);
  CL2.setLED(LED_pin[2], HeatingTime[2] * SecCycles, LED_MaskSec_array[2] * SecCycles, LED_CycleSec_array[2] * SecCycles, LED_OnSec_array[2] * SecCycles);
  CL3.setLED(LED_pin[3], HeatingTime[3] * SecCycles, LED_MaskSec_array[3] * SecCycles, LED_CycleSec_array[3] * SecCycles, LED_OnSec_array[3] * SecCycles);
}

void LED_Switch(int num) {
  if (LED_TurnOn[num])
    digitalWrite(LED_pin[num], HIGH);
  else {
    if (num == 0)
      LED_onoff[num] = CL0.updateLED(HeatingTime_Counter[num]);
    else if (num == 1)
      LED_onoff[num] = CL1.updateLED(HeatingTime_Counter[num]);
    else if (num == 2)
      LED_onoff[num] = CL2.updateLED(HeatingTime_Counter[num]);
    else if (num == 3)
      LED_onoff[num] = CL3.updateLED(HeatingTime_Counter[num]);
  }
}


