void Led_setup() {
  pinMode(LED_0, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  digitalWrite(LED_0, LOW);
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);
  for (int i = 0; i < 4; i++) {
    LED_SecCounter[i] = LED_CycleSec;
    LED_CycleCounter[i] = LED_EnableTimes;
  }
}

void LED_Switch(int num) {
  if (button[num] == true) {
    if (LED_SecCounter[num] > 0)
      LED_SecCounter[num]--;
    else {
      LED_SecCounter[num] = LED_CycleSec;
      if (LED_CycleCounter[num] > 0)
        LED_CycleCounter[num]--;
    }
  }
  else {
    LED_SecCounter[num] = LED_CycleSec;
    LED_CycleCounter[num] = LED_EnableTimes;
  }

  if (LED_SecCounter[num] > LED_OnSec)
    LEDonoff[num] = false;
  else if (LED_SecCounter[num] <= LED_OnSec) {
    if (LED_CycleCounter[num] > LED_OnTimes || LED_CycleCounter[num] == 0)
      LEDonoff[num] = false;
    else
      LEDonoff[num] = true;
  }
  digitalWrite(LED_pin[num], LEDonoff[num]);
}

