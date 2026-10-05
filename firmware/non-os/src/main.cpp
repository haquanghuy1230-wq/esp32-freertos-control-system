// Recovered from section 4.2.2 of RTOS_report.docx.
#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//PIN
#define LED_1     15
#define LED_2     4
#define LED_3     23

#define BUTTON_1  5
#define BUTTON_2  18

//OLED
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BH1750 lightMeter;

//time
#define LED1_PERIOD     200
#define BH1750_PERIOD   500
#define OLED_PERIOD     500
#define DEBOUNCE_TIME   10

//state
bool led1State = false; //led blink 500ms
bool led2State = false; //led toggle
bool led3State = false; //led semaphore

float luxValue = 0;

//timer
unsigned long tLed1 = 0;
unsigned long tBH   = 0;
unsigned long tOLED = 0;
unsigned long tLed3 = 0; 

//led3
bool led3Running = false;
int  led3Countdown = 0;

//button
bool lastBtn1 = HIGH;
bool stableBtn1 = HIGH;
unsigned long tDebounce1 = 0;

bool lastBtn2 = HIGH;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  Wire.setClock(400000);

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);

  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);

  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);

  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED ERROR");
    while (1);
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
}

void loop() {
  unsigned long now = millis();

  //task1
  if (now - tLed1 >= LED1_PERIOD / 2) {
    tLed1 = now;
    led1State = !led1State;
    digitalWrite(LED_1, led1State);
  }

  //task3
  bool btn1 = digitalRead(BUTTON_1);
  if (btn1 != lastBtn1) {
    tDebounce1 = now;
  }

  if (now - tDebounce1 > DEBOUNCE_TIME) {
    if (btn1 != stableBtn1) {
      stableBtn1 = btn1;
      if (stableBtn1 == LOW) {
        led2State = !led2State;
        digitalWrite(LED_2, led2State);
      }
    }
  }
  lastBtn1 = btn1;

  //task4
  bool btn2 = digitalRead(BUTTON_2);
  if (lastBtn2 == HIGH && btn2 == LOW) {
    if (!led3Running) {
      led3Running = true;
      led3Countdown = 3;
      tLed3 = now;
      digitalWrite(LED_3, HIGH);
    }
  }
  lastBtn2 = btn2;

  //task5
  if (led3Running && now - tLed3 >= 1000) {
    tLed3 = now;
    led3Countdown--;
    if (led3Countdown <= 0) {
      led3Running = false;
      digitalWrite(LED_3, LOW);
    }
  }

  //task2
  if (now - tBH >= BH1750_PERIOD) {
    tBH = now;
    luxValue = lightMeter.readLightLevel();
  }

  //task6
  if (now - tOLED >= OLED_PERIOD) {
    tOLED = now;

    display.clearDisplay();

    display.setCursor(0, 0);
    display.print("Light:");
    display.print(luxValue, 1);
    display.println(" lx");

    display.setCursor(0, 20);
    display.print("LED2: ");
    display.println(led2State ? "ON" : "OFF");

    display.setCursor(0, 40);
    display.print("LED3: ");
    if (led3Running) {
      display.print(led3Countdown);
      display.print(" s");
    } else {
      display.print("OFF");
    }

    display.display();
  }
}
