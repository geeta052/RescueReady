#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <NewPing.h>

#define FIREBASE_HOST "disastermanagement-92d4f-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "PSzJiukxeUUvPXXm7V4R3ab9zfY70FOBHSHpb3am"
#define WIFI_SSID "Deathadder"
#define WIFI_PASSWORD "deathadder33"

const int trigPin = D6; // HC-SR04 trigger pin
const int echoPin = D5; // HC-SR04 echo pin
const int vibrationPin = D4; // SW-420 vibration sensor pin
const int flamePin = D1; // Flame sensor pin
const int buzzerPin = D3; // Buzzer pin

// Define sound velocity in cm/uS for HC-SR04 sensor
#define SOUND_VELOCITY 0.034

long duration;
float distanceCm;
int vibrationValue; // Raw value from the vibration sensor
int flameValue; // Flame sensor value

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(vibrationPin, INPUT_PULLUP);
  pinMode(flamePin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi!");

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  // Read distance from HC-SR04 sensor
  unsigned int distanceCm = getDistance();

  // Read raw value from the vibration sensor
  vibrationValue = analogRead(vibrationPin);

  // Read flame sensor value
  flameValue = digitalRead(flamePin);

  // Print sensor values
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Vibration Value: ");
  Serial.println(vibrationValue);
  Serial.print("Flame Sensor Value: ");
  Serial.println(flameValue);

  // Send data to Firebase
  Firebase.setFloat("/distance", distanceCm);
  Firebase.setInt("/vibrationValue", vibrationValue);
  Firebase.setInt("/flameValue", flameValue);

  // Check if flame is detected and activate buzzer accordingly
  if (flameValue == HIGH) {
    digitalWrite(buzzerPin, LOW); // Turn on the buzzer
  } else {
    digitalWrite(buzzerPin, HIGH); // Turn off the buzzer
  }

  delay(1000);
}

unsigned int getDistance() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance in centimeters
  distanceCm = duration * SOUND_VELOCITY / 2;
  
  return distanceCm;
}
