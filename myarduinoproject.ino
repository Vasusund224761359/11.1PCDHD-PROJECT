
const int RED1 = 2;
const int YELLOW1 = 3;
const int GREEN1 = 4;

const int RED2 = 5;
const int YELLOW2 = 6;
const int GREEN2 = 7;

const int RED3 = 8;
const int YELLOW3 = 9;
const int GREEN3 = 10;

// Sensor and switch pins
const int SWITCH_PIN = 13;
const int SOUND_SENSOR = A1;
const int TRIG_PIN = 11;
const int ECHO_PIN = 12;

// Thresholds
const int emergencySoundThreshold = 25;
const float distanceThreshold = 20.0;

// Lane control
int currentLane = 1;
bool systemActive = false;

void setup() {
  Serial.begin(9600);

  int ledPins[] = {RED1, YELLOW1, GREEN1, RED2, YELLOW2, GREEN2, RED3, YELLOW3, GREEN3};
  for (int i = 0; i < 9; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT);
}

void turnAllLedsOff() {
  int ledPins[] = {RED1, YELLOW1, GREEN1, RED2, YELLOW2, GREEN2, RED3, YELLOW3, GREEN3};
  for (int i = 0; i < 9; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

bool isSwitchOn() {
  return digitalRead(SWITCH_PIN) == HIGH;
}

void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    if (!isSwitchOn()) {
      turnAllLedsOff();
      systemActive = false;  
      return;
    }
    delay(10);
  }
}

void handleEmergency() {
  Serial.println("HOLDING GREEN FOR EMERGENCY VEHICLE");
  turnAllLedsOff();

  if (currentLane != 1) digitalWrite(RED1, HIGH);
  if (currentLane != 2) digitalWrite(RED2, HIGH);
  if (currentLane != 3) digitalWrite(RED3, HIGH);

  if (currentLane == 1) digitalWrite(GREEN1, HIGH);
  else if (currentLane == 2) digitalWrite(GREEN2, HIGH);
  else if (currentLane == 3) digitalWrite(GREEN3, HIGH);

  smartDelay(6000);
  turnAllLedsOff();
  smartDelay(1000);
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  return (duration == 0) ? -1 : duration * 0.034 / 2.0;
}

void normalTrafficCycle() {
  currentLane = 1;
  digitalWrite(RED2, HIGH);
  digitalWrite(RED3, HIGH);
  digitalWrite(GREEN1, HIGH);
  smartDelay(3000);
  digitalWrite(GREEN1, LOW);
  digitalWrite(YELLOW1, HIGH);
  smartDelay(1000);
  digitalWrite(YELLOW1, LOW);
  digitalWrite(RED1, HIGH);
  smartDelay(500);
  if (!systemActive) return;

  currentLane = 2;
  digitalWrite(RED2, LOW);
  digitalWrite(GREEN2, HIGH);
  smartDelay(3000);
  digitalWrite(GREEN2, LOW);
  digitalWrite(YELLOW2, HIGH);
  smartDelay(1000);
  digitalWrite(YELLOW2, LOW);
  digitalWrite(RED2, HIGH);
  smartDelay(500);
  if (!systemActive) return;

  currentLane = 3;
  digitalWrite(RED3, LOW);
  digitalWrite(GREEN3, HIGH);
  smartDelay(3000);
  digitalWrite(GREEN3, LOW);
  digitalWrite(YELLOW3, HIGH);
  smartDelay(1000);
  digitalWrite(YELLOW3, LOW);
  digitalWrite(RED3, HIGH);
  smartDelay(500);
}

void loop() {
  if (isSwitchOn()) {
    if (!systemActive) {
      Serial.println("SYSTEM STARTED");
      systemActive = true;     
      currentLane = 1;         
    }

    int soundValue = analogRead(SOUND_SENSOR);
    float distance = getDistance();

    Serial.print("SOUND: ");
    Serial.print(soundValue);
    Serial.print(", DIST: ");
    Serial.println(distance);

    if (soundValue > emergencySoundThreshold && distance > 0 && distance < distanceThreshold) {
      handleEmergency();
    } else {
      normalTrafficCycle();
    }
  } else {
    if (systemActive) {
      Serial.println("SYSTEM TURNED OFF");
      turnAllLedsOff();
      systemActive = false;
    }
    delay(100);
  }
}