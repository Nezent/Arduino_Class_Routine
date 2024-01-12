#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change the address if needed
int buzzerPin = 9; // Change the pin number to the one connected to the buzzer

void setup() {
  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT);
  lcd.begin(16, 2);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(_DATE), F(__TIME_)));
  }
  lcd.backlight(); 
}

void loop() {
  DateTime now = rtc.now();
  int currentDay = now.dayOfTheWeek();

  // Set class periods and breaks for each day
  switch (currentDay) {
    case 1:  // Monday
      setRoutine(8, 0, 10, 30, "ETE 2200"); // 8:00 AM - 9:00 AM: Class 1
      setRoutine(10, 30, 10, 50, "Tiffin Break"); // 9:15 AM - 10:15 AM: Class 2
      setRoutine(10, 50, 11, 40, "CSE 2253"); // 10:30 AM - 11:30 AM: Class 3
      setRoutine(11, 40, 12, 30, "ETE 2211"); // 11:45 AM - 12:45 PM: Class 4
      setRoutine(12, 30, 13, 20, "HUM 2215"); // 1:30 PM - 2:00
      setRoutine(13, 20, 14, 30, "Class Break"); // 1:30 PM - 2:00 PM: Tiffin Break
      setRoutine(14, 30, 17, 0, "ETE 2212"); // 2:15 PM - 3:15 PM: Class 5
      break;

    case 2:  // Tuesday
      setRoutine(8, 0, 10, 30, "CSE 2254"); // 8:00 AM - 9:00 AM: Class 1
      setRoutine(10, 30, 10, 50, "Tiffin Break"); // 9:15 AM - 10:15 AM: Class 2
      setRoutine(10, 50, 11, 40, "ETE 2211"); // 10:30 AM - 11:30 AM: Class 3
      setRoutine(11, 40, 12, 30, "HUM 2215"); // 11:45 AM - 12:45 PM: Class 4
      setRoutine(12, 30, 13, 20, "CSE 2253"); // 1:30 PM - 2:00
      break;
     case 3:  // Wednesday
      setRoutine(8, 0, 10, 30, "CSE 2254"); // 8:00 AM - 9:00 AM: Class 1
      setRoutine(10, 30, 10, 50, "Tiffin Break"); // 9:15 AM - 10:15 AM: Class 2
      setRoutine(10, 50, 11, 40, "ETE 2211"); // 10:30 AM - 11:30 AM: Class 3
      setRoutine(11, 40, 12, 30, "HUM 2215"); // 11:45 AM - 12:45 PM: Class 4
      setRoutine(12, 30, 13, 20, "CSE 2253"); // 1:30 PM - 2:00
      break;
    case 6:  // Saturday
      setRoutine(8, 0, 8, 50, "MATH 2215"); // 8:00 AM - 9:00 AM: Class 1
      setRoutine(8, 50, 9, 40, "ETE 2215"); // 9:15 AM - 10:15 AM: Class 2
      setRoutine(9, 40, 10, 30, "HUM 2215"); // 10:30 AM - 11:30 AM: Class 3
      setRoutine(10, 30, 10, 50, "Tiffin Break"); // 1:30 PM - 2:00 PM: Tiffin Break
      setRoutine(10, 50, 13, 20, "ETE 2200"); // 11:45 AM - 12:45 PM: Class 4
      break;
     case 7:  // Sunday
      setRoutine(10, 50, 11, 40, "CSE 2253"); // 8:00 AM - 9:00 AM: Class 1
      setRoutine(11, 40, 12, 30, "MATH 2215"); // 9:15 AM - 10:15 AM: Class 2
      setRoutine(12, 30, 13, 20, "ETE 2215"); // 10:30 AM - 11:30 AM: Class 3
      break;
  }

  delay(1000); // Wait for a second
}

void setRoutine(int startHour, int startMinute, int endHour, int endMinute, const char *className) {
  DateTime now = rtc.now();
  DateTime classStart = DateTime(now.year(), now.month(), now.day(), startHour, startMinute);
  DateTime classEnd = DateTime(now.year(), now.month(), now.day(), endHour, endMinute);

  if (now >= classStart && now < classEnd) {
    // Class is ongoing, display the class name on the LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(now.toString("YYYY-MM-DD")); // Display date in the first line
    lcd.setCursor(0, 1);
    lcd.print(className);
    return;
  }

  // If the current time is not within the class period, check if it's time for an alarm
  if (now >= classEnd && now < classEnd + TimeSpan(5)) {
    // Class period just ended, activate buzzer for 30 seconds
    activateBuzzer(30, className, now); // 30 seconds in seconds
  } else if (now >= classStart - TimeSpan(5) && now < classStart) {
    // Class period about to start, activate buzzer for 30 seconds
    activateBuzzer(30, className, now); // 30 seconds in seconds
  }
}

void activateBuzzer(int seconds, const char *message, DateTime now) {
  for (int i = 0; i < seconds; i++) {
    tone(buzzerPin, 1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(now.toString("YYYY-MM-DD")); // Display date in the first line
    lcd.setCursor(0, 1);
    lcd.print(message);
    delay(500); // Buzz for 500 milliseconds
    noTone(buzzerPin);
    delay(500); // Pause for 500 milliseconds
  }

  delay(1000); // Additional delay after turning off the buzzer
  lcd.clear();
}
