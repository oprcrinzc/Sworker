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

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

/*
const char* ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n" \
"SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n" \
"GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n" \
"AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n" \
"q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n" \
"SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n" \
"Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n" \
"a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n" \
"/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n" \
"AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n" \
"CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n" \
"bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n" \
"c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n" \
"VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n" \
"ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n" \
"MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n" \
"Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n" \
"AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n" \
"uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n" \
"wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n" \
"X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n" \
"PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n" \
"KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n" \
"-----END CERTIFICATE-----\n";
*/

String worker_name;
String worker_id;
int state = 0;
int is_registered = 0;

String dataserver = "http://192.168.21.185:8888";
// int is_
float waterLevel;
float waterLevelTarget;
float waterLevelToFill;
float h;
float t;
float f;

void see_gatekeeper()
{
  if ((wifiMulti.run() == WL_CONNECTED))
  {

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");
    if (is_registered == 0)
    {
      http.begin(dataserver + "/gatekeeper/" + worker_name);

      USE_SERIAL.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK)
        {
          String payload = http.getString();
          if (payload == "go to register")
            state = 1;
          else
          {
            worker_id = payload;
            state = 2;
          }
          USE_SERIAL.println(payload);
        }
        else
        {
          USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
      }
    }
    http.end();
  }
}
void goto_register()
{
  if ((wifiMulti.run() == WL_CONNECTED))
  {

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");
    if (is_registered == 0)
    {
      http.begin(dataserver + "/register/workers/" + worker_name);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      USE_SERIAL.print("[HTTP] POST...\n");
      // start connection and send HTTP header
      int httpCode = http.POST("");

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == 201)
        {
          String payload = http.getString();
          // if (payload=="go to register") state = 1;
          state = 0;
          USE_SERIAL.println(payload);
        }
        else
        {
          USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
      }
    }
    http.end();
  }
}
void update_data()
{
  if ((wifiMulti.run() == WL_CONNECTED))
  {

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");
    if (is_registered == 0)
    {
      String url_ = dataserver + "/update/workers/" + worker_name;
      http.begin(url_);
      String boundary = "----Eeeboundary";
      http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);

      USE_SERIAL.print("[HTTP] POST...\n");
      // start connection and send HTTP header
      // float h = dht.readHumidity();
      // float t = dht.readTemperature();
      // USE_SERIAL.println("-----");
      // USE_SERIAL.println(h);
      // USE_SERIAL.println(t);
      // String bod = "id="+worker_id+"&humidity="+String(h, 2)+"&temperature="+String(t, 2);
      String body = "";
      body += "--" + boundary + "\r\n";
      body += "Content-Disposition: form-data; name=\"id\"\r\n\r\n";
      body += worker_id.substring(1, 25) + "\r\n";

      body += "--" + boundary + "\r\n";
      body += "Content-Disposition: form-data; name=\"humidity\"\r\n\r\n";
      body += String(h, 2) + "\r\n";

      body += "--" + boundary + "\r\n";
      body += "Content-Disposition: form-data; name=\"temperature\"\r\n\r\n";
      body += String(t, 2) + "\r\n";

      body += "--" + boundary + "\r\n";
      body += "Content-Disposition: form-data; name=\"water_level\"\r\n\r\n";
      body += String(waterLevel, 1) + "\r\n";
      body += "--" + boundary + "\r\n";
      int httpCode = http.PUT(body);

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == 201)
        {
          String payload = http.getString();
          // if (payload=="go to register") state = 1;
          // state = 2;
          USE_SERIAL.println("updated");
        }
        else
        {
          USE_SERIAL.println(url_);
          USE_SERIAL.println(body);
          USE_SERIAL.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
      }
    }
    http.end();
  }
}

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

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);

  for (int pin : btn)
    pinMode(pin, INPUT_PULLUP);
  updateBtnState(0);
  updateBtnState(1);
}

unsigned long previousMillis = 0; // Store last time
const long interval = 5000;       // Interval in milliseconds

void loop()
{
  h = dht.readHumidity();
  t = dht.readTemperature();
  f = dht.readTemperature(true);
  waterLevel = analogRead(35) / 4095.0 * 100;

  unsigned long currentMillis = millis();

  if (state == 0)
    see_gatekeeper();
  if (state == 1)
    goto_register();
  // if () update_data();

  if (state == 2 && currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    update_data();
  }

  updateBtnState(0);
  int btn_num = 0;
  for (int state : btn_current_state)
  {
    if (btn_num == 0 && state == 1 && btn_last_state.at(btn_num) == 0)
    {
      toggleCMMM();
      Serial.println(state);
    }
    btn_num += 1;
  }

  float show;
  std::string units;
  if (munit == 0)
  {
    units = "C";
    show = t;
  }
  else if (munit == 1)
  {
    units = "F";
    show = f;
  }
  else if (munit == 2)
  {
    units = "%";
    show = h;
  }
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(show);
  display.setCursor(100, 15);
  display.setTextSize(2);
  display.print(units.c_str());
  display.display();
  updateBtnState(1);

  // Serial.println("GPIO35 analog");
  // Serial.println(analogRead(35)/4095.0*100);
}
