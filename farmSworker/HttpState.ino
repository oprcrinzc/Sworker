#include <ArduinoJson.h>
void see_gatekeeper()
{
  if ((wifiMulti.run() == WL_CONNECTED))
  {

    HTTPClient http;

    USE_SERIAL.print("[Gatekeeper] begin...\n");
    if (is_registered == 0)
    {
      http.begin(dataserver + "/gatekeeper/" + worker_name);

      USE_SERIAL.print("[Gatekeeper] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      USE_SERIAL.printf("[Gatekeeper] GET... code: %d\n", httpCode);

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled

        // file found at server
        if (httpCode == HTTP_CODE_OK)
        {
          String payload = http.getString();
          USE_SERIAL.printf("[Gatekeeper] GET... code: %s\n", payload);
          if (payload == "go to register")
          {state = 1;}
          else
          {
            worker_id = payload;
            state = 2;
            is_registered = 1;
          }
          USE_SERIAL.println(payload);
        }
        else
        {
          USE_SERIAL.printf("[Gatekeeper] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
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

    USE_SERIAL.print("[Register] begin...\n");
    if (is_registered == 0)
    {
      http.begin(dataserver + "/register/workers/" + worker_name);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      USE_SERIAL.print("[Register] POST...\n");
      // start connection and send HTTP header
      int httpCode = http.POST("");

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        USE_SERIAL.printf("[Register] POST... code: %d\n", httpCode);

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
          USE_SERIAL.printf("[Register] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
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

    USE_SERIAL.print("[Update] begin...\n");
    if (is_registered == 1)
    {
      String url_ = dataserver + "/update/workers/" + worker_name;
      http.begin(url_);
      String boundary = "----Eeeboundary";
      http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);

      USE_SERIAL.print("[Update] PUT...\n");
      // start connection and send HTTP header
      // float h = dht.readHumidity();
      // float t = dht.readTemperature();
      // USE_SERIAL.println("-----");
      // USE_SERIAL.println(h);
      // USE_SERIAL.println(t);
      // String bod = "id="+worker_id+"&humidity="+String(h, 2)+"&temperature="+String(t, 2);
      // waterLevel = maxWaterCM - GetDistance();
      // Disperm = waterLevel/maxWaterCM *100;
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
      body += String(Disperm, 2) + "\r\n";
      body += "--" + boundary + "\r\n";
      int httpCode = http.PUT(body);

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        USE_SERIAL.printf("[Update] PUT... code: %d\n", httpCode);

        // file found at server
        if (httpCode == 200)
        {
          String payload = http.getString();
          // if (payload=="go to register") state = 1;
          // state = 2;
          USE_SERIAL.println("updated");
          // USE_SERIAL.println(payload);
          StaticJsonDocument<200> doc;
          DeserializationError error = deserializeJson(doc, payload);
    
          if (error) {
              Serial.print("JSON Error: ");
              Serial.println(error.f_str());
              return;
          }

          float waterLevelTarget_ = doc["water_level_target"];
          float waterLevelToFill_ = doc["water_level_to_fill"];
          waterLevelTarget = waterLevelTarget_;
          waterLevelToFill = waterLevelToFill_;
        }
        else
        {
          // USE_SERIAL.println(url_);
          // USE_SERIAL.println(body);
          USE_SERIAL.printf("[Update] PUT failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
      }
    }
    http.end();
  }
}
