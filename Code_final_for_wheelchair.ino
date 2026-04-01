#include <SoftwareSerial.h>

// Motor driver pins (ESP8266 valid GPIOs)
#define pin1 5  // GPIO5  = D1
#define pin2 4  // GPIO4  = D2
#define pin3 0  // GPIO0  = D3
#define pin4 2  // GPIO2  = D4

// VC-02 voice module communication pins
#define VC_RX 12 // GPIO12 = D6
#define VC_TX 14 // GPIO14 = D5
SoftwareSerial vcSerial(VC_RX, VC_TX); // RX, TX

// Ultrasonic Sensor 1 (Front)
#define trig1 13 // GPIO13 = D7
#define echo1 15 // GPIO15 = D8

// Ultrasonic Sensor 2 (Downward or Side)
#define trig2 16 // GPIO16 = D0
#define echo2 10 // GPIO10 = SD3

unsigned int receivedValue = 0;
int speedValue = 0;
const int eco = 100;
const int normal = 150;
const int sports = 255;
const int commandDelay = 1000; // <<< Delay after each command in milliseconds

void setup() {
  Serial.begin(9600);
  vcSerial.begin(9600);

  // Motor pins
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);

  // Ultrasonic sensors
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);

  stopMotors();
  Serial.println("VC-02 Voice + Ultrasonic Control Ready");
}

void loop() {
  long dist1 = getDistance(trig1, echo1); // front
  long dist2 = getDistance(trig2, echo2); // downward

  Serial.print("Distance1: ");
  Serial.print(dist1);
  Serial.print(" cm, Distance2: ");
  Serial.print(dist2);
  Serial.println(" cm");

  // Safety check: Stop if obstacle too close
  if (dist1 < 10 || dist2 < 10) {
    Serial.println("Obstacle detected - Stopping!");
    stopMotors();
    delay(200);
    return;
  }

  // Voice command check
  if (vcSerial.available() >= 2) {
    byte highByte = vcSerial.read();
    byte lowByte = vcSerial.read();
    receivedValue = (highByte << 8) | lowByte;

    Serial.print("Received HEX: 0x");
    Serial.println(receivedValue, HEX);

    if (receivedValue == 0xA000) {
      Serial.println("FORWARD - SLOW");
      speedValue = eco;
      moveForward();
      delay(commandDelay);
      // stopMotors();
    } else if (receivedValue == 0xA100) {
      Serial.println("FORWARD - NORMAL");
      speedValue = normal;
      moveForward();
      delay(commandDelay);
      // stopMotors();
    } else if (receivedValue == 0xA200) {
      Serial.println("FORWARD - FAST");
      speedValue = sports;
      moveForward();
      delay(commandDelay);
      // stopMotors();
    } else if (receivedValue == 0xB000) {
      Serial.println("BACKWARD - SLOW");
      speedValue = eco;
      moveBackward();
      delay(commandDelay);
      // stopMotors();
    } else if (receivedValue == 0xB100) {
      Serial.println("BACKWARD - NORMAL");
      speedValue = normal;
      moveBackward();
      delay(commandDelay);
      // stopMotors();
    } else if (receivedValue == 0xB200) {
      Serial.println("BACKWARD - FAST");
      speedValue = sports;
      moveBackward();
      delay(commandDelay);
      // stopMotors();
    } else if (receivedValue == 0xC100) {
      Serial.println("TURNING LEFT");
      turnLeft();
      delay(commandDelay);
      // stopMotors();
    } else if (receivedValue == 0xD100) {
      Serial.println("TURNING RIGHT");
      turnRight();
      delay(commandDelay);
      // stopMotors();
    } else {
      Serial.println("STOP / UNKNOWN COMMAND");
      stopMotors();
    }
  }
}

// -------- Ultrasonic Function --------
long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000); // Timeout at 20ms
  long distance = duration * 0.034 / 2;
  return distance;
}

// -------- Motor Logic --------
void moveForward() {
  analogWrite(pin1, speedValue);
  digitalWrite(pin2, LOW);
  analogWrite(pin3, speedValue);
  digitalWrite(pin4, LOW);
}

void moveBackward() {
  digitalWrite(pin1, LOW);
  analogWrite(pin2, speedValue);
  digitalWrite(pin3, LOW);
  analogWrite(pin4, speedValue);
}

void turnRight() {
  digitalWrite(pin1, LOW);
  analogWrite(pin2, speedValue);
  analogWrite(pin3, speedValue);
  digitalWrite(pin4, LOW);
}

void turnLeft() {
  analogWrite(pin1, speedValue);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  analogWrite(pin4, speedValue);
}

void stopMotors() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
}
