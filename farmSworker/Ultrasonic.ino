const int trigPin = 16;
const int echoPin = 17;

float soundSpeed;

void BeginUltrasonic(){
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

float GetDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  soundSpeed = 331.3 + 0.606*t;
  soundSpeed = soundSpeed * 100 / 1000000;
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * soundSpeed / 2;
  return distance;
}