#include "Arduino.h"

int pingPin = 3; // pinul pe care dam senzorului comanda de masurare
int inPin = 2; // pinul pe care primim raspunsul de la senzor
int ledPin = 9; // led de semnalizare obstacol detectat
double prevReadingCm = 0; // previously read value in centimeters; we use it to remove errouneous readings

void setup() {
  Serial.begin(9600);
  pinMode(pingPin, OUTPUT);
  pinMode(inPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  long duration;
  double cm;

  duration = readSensorValue();

  cm = microsecondsToCentimeters(duration);

  // print: 2389ms = 41.19cm
  //              in baza 10;------v  sau simplu: String(duration)
  Serial.println(String(duration, 10) + "ms = " + String(cm) + "cm");
  // all the above prints: 2389ms = 41.19cm

  if (cm > 0) {
    if (abs(cm - prevReadingCm) > 100) {
      // is errouneous reading
      cm = prevReadingCm;
    } else {
      prevReadingCm = cm;
    }
  }

  if (cm > 0 && cm < 50) {
    // aprindem led-ul progresiv
    analogWrite(ledPin,  5 * (50 - cm) + 9); // tolerance +4cm min distance + 5 for max 255 output
  } else {
    analogWrite(ledPin, 0);
  }

  delay(30);
}

double microsecondsToCentimeters(long microseconds) {
  return (double) microseconds / (double) 58;
}

long readSensorValue() {
  trigerMeasurementSignal();

  // get the value returned by the sensor on port inPin
  return pulseIn(inPin, HIGH);

}

void trigerMeasurementSignal() {
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
}
