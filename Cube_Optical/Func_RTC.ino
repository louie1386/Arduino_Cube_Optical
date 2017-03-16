void RTC_setup() {
  setSyncProvider(RTC.get);
  if (year() == 1970) {
    RTCtm.Hour = RTC_Hour;
    RTCtm.Minute = RTC_Min;
    RTCtm.Second = RTC_Sec;

    RTCtm.Day = RTC_Day;
    RTCtm.Month = RTC_Month;
    RTCtm.Year = CalendarYrToTm(RTC_Year);

    RTC.write(RTCtm);
  }
}

word RTC_timetoFAT32() {
  setSyncProvider(RTC.get);
  word RTCtime = hour() << 11;
  RTCtime += (minute() << 5);
  RTCtime += (second() / 2);
  return RTCtime;
}

word RTC_datetoFAT32() {
  setSyncProvider(RTC.get);
  word RTCdate = (year() - 1980) << 9;
  RTCdate += (month() << 5);
  RTCdate += day();
  return RTCdate;
}

void RTC_digitalClockDisplay() {
  if (RTC_print) {
    setSyncProvider(RTC.get);
    Serial_Log.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial_Log.print(" ");
    Serial_Log.print(day());
    Serial_Log.print(" ");
    Serial_Log.print(month());
    Serial_Log.print(" ");
    Serial_Log.print(year());
    Serial_Log.println();
  }
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial_Log.print(":");
  if (digits < 10)
    Serial_Log.print('0');
  Serial_Log.print(digits);
}
