void Led_setup() {
  pinMode(LED_0, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  digitalWrite(LED_0, LOW);
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);
}

void LED_Switch(int num) {
  if (LED_SecCounter[num] > LED_OnSec)
    LEDonoff[num] = false;
  else if (LED_SecCounter[num] <= LED_OnSec) {
    LEDonoff[num] = LEDEanbleType[LED_EnableTimes - LED_CycleCounter[num]];
  }

  if (button[num] == true) {
    LED_SecCounter[num]--;
    if (LED_SecCounter[num] == 0) {
      LED_SecCounter[num] = LED_CycleSec;
      if (LED_CycleCounter[num] > 0)
        LED_CycleCounter[num]--;
    }
  }
  else {
    LED_SecCounter[num] = LED_CycleSec;
    LED_CycleCounter[num] = LED_EnableTimes;
  }

  digitalWrite(LED_pin[num], LEDonoff[num]);
}


