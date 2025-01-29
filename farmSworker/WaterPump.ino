const int waterPumpPin = 22;
bool isPumping = false;

void BeginWaterPump(){
  pinMode(waterPumpPin, OUTPUT);
}

void WaterPump(){
  
  // float Disperm = waterLevel/maxWaterCM *100;
  
  if (Disperm >= waterLevelToFill && Disperm < waterLevelTarget){
    isPumping = true;
    digitalWrite(waterPumpPin, HIGH);
  } else {
    isPumping = false;
    digitalWrite(waterPumpPin, LOW);
  }
}