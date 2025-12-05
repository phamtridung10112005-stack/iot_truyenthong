#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

const int PIN_BTN_MODE = 6;
const int PIN_BTN_UP   = 5;
const int PIN_BTN_DOWN = 4;
const int PIN_BTN_SET  = 3;
const int PIN_BUZZER   = 2;

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

enum ModeState {
  MODE_VIEW_TIME,
  MODE_SET_ALARM_H,
  MODE_SET_ALARM_M,
  MODE_SET_TIME_H,
  MODE_SET_TIME_M
};

ModeState currentMode = MODE_VIEW_TIME;

int alarmHour = 6;
int alarmMinute = 0;
bool isAlarmOn = true;
bool isRinging = false;

int tempHour, tempMinute;

unsigned long modePressTime = 0;
bool modeButtonHeld = false;

void handleButtons();
void changeMode();
void handleUp();
void handleDown();
void handleSet();
void printDigits(int digits);
void displayRealTime();
void displaySetAlarm();
void displaySetTime();
void checkAlarmTrigger();
void ringAlarm();
void stopAlarm();

void setup() {
  pinMode(PIN_BTN_MODE, INPUT_PULLUP);
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_SET, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  
  digitalWrite(PIN_BUZZER, LOW);

  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    lcd.print("Loi RTC!");
    while (1);
  }

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  handleButtons();
  
  if (isRinging) {
    ringAlarm();
  } else {
    switch (currentMode) {
      case MODE_VIEW_TIME:
        displayRealTime();
        checkAlarmTrigger();
        break;
      case MODE_SET_ALARM_H:
      case MODE_SET_ALARM_M:
        displaySetAlarm();
        break;
      case MODE_SET_TIME_H:
      case MODE_SET_TIME_M:
        displaySetTime();
        break;
    }
  }
  delay(50);
}

void handleButtons() {
  int modeState = digitalRead(PIN_BTN_MODE);
  
  if (modeState == LOW) {
    if (modePressTime == 0) modePressTime = millis();
    if (millis() - modePressTime > 2000 && !modeButtonHeld && currentMode == MODE_VIEW_TIME) {
      isAlarmOn = !isAlarmOn; 
      modeButtonHeld = true; 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(isAlarmOn ? "BAO THUC: ON " : "BAO THUC: OFF");
      delay(1000);
      lcd.clear();
    }
  } else {
    if (modePressTime > 0) {
      if (!modeButtonHeld && millis() - modePressTime < 1000) {
        if (isRinging) {
          stopAlarm(); 
        } else {
          changeMode();
        }
      }
      modePressTime = 0;
      modeButtonHeld = false;
    }
  }

  if (digitalRead(PIN_BTN_UP) == LOW) {
    handleUp();
    delay(200);
  }
  if (digitalRead(PIN_BTN_DOWN) == LOW) {
    handleDown();
    delay(200);
  }
  if (digitalRead(PIN_BTN_SET) == LOW) {
    handleSet();
    delay(300);
  }
}

void changeMode() {
  lcd.clear();
  switch (currentMode) {
    case MODE_VIEW_TIME:
      currentMode = MODE_SET_ALARM_H;
      tempHour = alarmHour;
      tempMinute = alarmMinute;
      break;
    case MODE_SET_ALARM_H: 
    case MODE_SET_ALARM_M:
      currentMode = MODE_SET_TIME_H;
      {
        DateTime now = rtc.now();
        tempHour = now.hour();
        tempMinute = now.minute();
      }
      break;
    default:
      currentMode = MODE_VIEW_TIME;
      break;
  }
}

void handleUp() {
  if (currentMode == MODE_SET_ALARM_H || currentMode == MODE_SET_TIME_H) {
    tempHour++;
    if (tempHour > 23) tempHour = 0;
  } else if (currentMode == MODE_SET_ALARM_M || currentMode == MODE_SET_TIME_M) {
    tempMinute++;
    if (tempMinute > 59) tempMinute = 0;
  }
}

void handleDown() {
  if (currentMode == MODE_SET_ALARM_H || currentMode == MODE_SET_TIME_H) {
    tempHour--;
    if (tempHour < 0) tempHour = 23;
  } else if (currentMode == MODE_SET_ALARM_M || currentMode == MODE_SET_TIME_M) {
    tempMinute--;
    if (tempMinute < 0) tempMinute = 59;
  }
}

void handleSet() {
  DateTime now = rtc.now();
  switch (currentMode) {
    case MODE_SET_ALARM_H:
      currentMode = MODE_SET_ALARM_M; 
      break;
    case MODE_SET_ALARM_M:
      alarmHour = tempHour; 
      alarmMinute = tempMinute;
      isAlarmOn = true; 
      currentMode = MODE_VIEW_TIME;
      lcd.clear();
      lcd.print("Da Luu Bao Thuc!");
      delay(1000);
      lcd.clear();
      break;
    case MODE_SET_TIME_H:
      currentMode = MODE_SET_TIME_M; 
      break;
    case MODE_SET_TIME_M:
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), tempHour, tempMinute, 0));
      currentMode = MODE_VIEW_TIME;
      lcd.clear();
      lcd.print("Da Luu Gio!");
      delay(1000);
      lcd.clear();
      break;
  }
}

void printDigits(int digits) {
  if (digits < 10) lcd.print('0');
  lcd.print(digits);
}

void displayRealTime() {
  DateTime now = rtc.now();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  printDigits(now.hour()); lcd.print(":");
  printDigits(now.minute()); lcd.print(":");
  printDigits(now.second());

  lcd.setCursor(0, 1);
  if (isAlarmOn) lcd.print("ON ");
  else lcd.print("OFF");
  
  lcd.print(" Hen Gio:");
  printDigits(alarmHour); lcd.print(":");
  printDigits(alarmMinute);
}

void displaySetAlarm() {
  lcd.setCursor(0, 0);
  lcd.print("CAI BAO THUC");
  lcd.setCursor(0, 1);
  
  if (currentMode == MODE_SET_ALARM_H) lcd.print(">"); 
  else lcd.print(" ");
  printDigits(tempHour);
  lcd.print(":");
  
  if (currentMode == MODE_SET_ALARM_M) lcd.print(">"); 
  else lcd.print(" ");
  printDigits(tempMinute);
}

void displaySetTime() {
  lcd.setCursor(0, 0);
  lcd.print("CAI THOI GIAN");
  lcd.setCursor(0, 1);
  
  if (currentMode == MODE_SET_TIME_H) lcd.print(">");
  else lcd.print(" ");
  printDigits(tempHour);
  lcd.print(":");
  
  if (currentMode == MODE_SET_TIME_M) lcd.print(">");
  else lcd.print(" ");
  printDigits(tempMinute);
}

void checkAlarmTrigger() {
  if (!isAlarmOn) return;
  
  DateTime now = rtc.now();
  if (now.hour() == alarmHour && now.minute() == alarmMinute && now.second() == 0) {
    isRinging = true;
  }
} 

void ringAlarm() {
  lcd.setCursor(0, 0);
  lcd.print("DAY DI BAN OI!!! ");
  lcd.setCursor(0, 1);
  lcd.print("Bam MODE de tat ");
  
  digitalWrite(PIN_BUZZER, HIGH);
  delay(100);
  digitalWrite(PIN_BUZZER, LOW);
  delay(100);
}

void stopAlarm() {
  isRinging = false;
  digitalWrite(PIN_BUZZER, LOW);
  lcd.clear();
}