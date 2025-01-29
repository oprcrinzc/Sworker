#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <Wire.h>
#include <DHT.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <vector>
#include <sstream>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;

DHT dht(19, DHT22);

// #define OLED_RESET 4
// Adafruit_SSD1306 display(OLED_RESET);

String worker_name;
String worker_id;
int state = 0;
int is_registered = 0;

String dataserver = "http://192.168.118.185:8888";
// int is_
const float maxWaterCM = 15;
float Disperm;
float waterLevel;
bool waterOverFlow = false;
float waterLevelTarget;
float waterLevelToFill;
float h;
float t;
float f;


std::vector<int> btn({34});
std::vector<int> btn_last_state(btn.size());
std::vector<int> btn_current_state(btn.size());

int playMode = 0;
int lastPlayMode = 0;
int munit = 0;

void updateBtnState(int mode = 0)
{
  if (mode == 0)
  {
    for (int i = 0; i < btn.size(); i++)
    {
      btn_current_state[i] = digitalRead(btn[i]);
    }
  }
  else if (mode == 1)
  {
    btn_last_state = btn_current_state;
  }
}

void toggleCMMM()
{
  munit += 1;
  if (munit > 2)
    munit = 0;
}

void setup()
{

  USE_SERIAL.begin(115200);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--)
  {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  // wifiMulti.addAP("natasa_2.4G", "S141036p");
  wifiMulti.addAP("🌸", "wpaoknao");

  worker_name = "esp32_0";

  pinMode(18, OUTPUT);
  pinMode(35, INPUT);
  digitalWrite(18, HIGH);
  dht.begin();

  // display.begin(SSD1306_SWITCHCAPVCC, 0x3c);

  for (int pin : btn)
    pinMode(pin, INPUT_PULLUP);
  updateBtnState(0);
  updateBtnState(1);

  BeginUltrasonic();
  BeginWaterPump();
}

unsigned long previousMillis = 0; // Store last time
const long interval = 0;       // Interval in milliseconds

void loop()
{
  h = dht.readHumidity();
  t = dht.readTemperature();
  f = dht.readTemperature(true);

  delay(500);

  waterLevel = maxWaterCM - GetDistance() + 5;
  Disperm = waterLevel/maxWaterCM *100;
  if(Disperm > 100) Disperm = 100;
  if(Disperm < 0) Disperm = 0;

  WaterPump();
  // float d = GetDistance();
  USE_SERIAL.println(String(Disperm, 2));
  USE_SERIAL.println(String(waterLevelTarget, 2));
  USE_SERIAL.println(String(waterLevelToFill, 2));

  unsigned long currentMillis = millis();

  if (state == 0)see_gatekeeper();
  if (state == 1)goto_register();

  if (state == 2 && currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    update_data();
  }

  // updateBtnState(0);
  // int btn_num = 0;
  // for (int state : btn_current_state)
  // {
  //   if (btn_num == 0 && state == 1 && btn_last_state.at(btn_num) == 0)
  //   {
  //     toggleCMMM();
  //     Serial.println(state);
  //   }
  //   btn_num += 1;
  // }

  // float show;
  // std::string units;
  // if (munit == 0)
  // {
  //   units = "C";
  //   show = t;
  // }
  // else if (munit == 1)
  // {
  //   units = "F";
  //   show = f;
  // }
  // else if (munit == 2)
  // {
  //   units = "%";
  //   show = h;
  // }
  // display.clearDisplay();
  // display.setTextSize(3);
  // display.setTextColor(WHITE);
  // display.setCursor(0, 0);
  // display.print(show);
  // display.setCursor(100, 15);
  // display.setTextSize(2);
  // display.print(units.c_str());
  // display.display();
  // updateBtnState(1);

  // Serial.println("GPIO35 analog");
  // Serial.println(analogRead(35)/4095.0*100);
}
