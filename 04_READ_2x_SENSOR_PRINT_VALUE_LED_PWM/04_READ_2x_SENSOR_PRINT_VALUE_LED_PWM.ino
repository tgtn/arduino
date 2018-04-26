#include "Arduino.h"

// senzor 1 (dreapta)
int trigPinSenzorDreapta = 9; // pinul pe care dam senzorului comanda de masurare
int echoPinSenzorDreapta = 10; // pinul pe care primim raspunsul
double prevReadingCm1 = 0; // previously read value in centimeters; we use it to remove errouneous readings

// senzor 2 (stanga)
int trigPin2 = 13; // pinul pe care dam senzorului comanda de masurare
int echoPin2 = 2; // pinul pe care primim raspunsul
double prevReadingCm2 = 0; // previously read value in centimeters; we use it to remove errouneous readings

#define minDistance 20
int ledPin = 6; // led de semnalizare
void setup() {
  Serial.begin(9600);
  pinMode(trigPinSenzorDreapta, OUTPUT);
  pinMode(echoPinSenzorDreapta, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  long duration1, duration2;
  double cm1, cm2;

  // citirea de la senzor 1
  duration1 = readSensorValue(trigPinSenzorDreapta, echoPinSenzorDreapta);
  cm1 = microsecondsToCentimeters(duration1);

  // print: 2389ms = 41.19cm
  //              in baza 10;------v  sau simplu: String(duration1)
  Serial.println("Senzor dreapta: " + String(duration1, 10) + "us = " + String(cm1) + "cm");
  // all the above prints: 2389ms = 41.19cm

  if (cm1 > 0) {
    if (abs(cm1 - prevReadingCm1) > 100) {
      // is errouneous reading
      cm1 = prevReadingCm1;
    } else {
      prevReadingCm1 = cm1;
    }
  }

  //citirea de la Senzorul 2
  duration2 = readSensorValue(trigPin2, echoPin2);
  cm2 = microsecondsToCentimeters(duration2);

  // print: 2389ms = 41.19cm
  //              in baza 10;------v  sau simplu: String(duration1)
  Serial.println("Senzor stanga: " + String(duration2, 10) + "us = " + String(cm2) + "cm");
  // all the above prints: 2389ms = 41.19cm

  if (cm2 > 0) {
    if (abs(cm2 - prevReadingCm2) > 100) {
      // is errouneous reading
      cm2 = prevReadingCm2;
    } else {
      prevReadingCm2 = cm2;
    }
  }

  if ((cm1 > 0 && cm1 < minDistance) || (cm2 > 0 && cm2 < minDistance)) {
    Serial.println("obstacol la " + String( min(cm1, cm2)) + "cm");
    // aprindem led-ul progresiv
    analogWrite(ledPin,  (255 / minDistance) * (minDistance - min(cm1, cm2)) + 4); // tolerance +4cm min distance
  } else {
    analogWrite(ledPin, 0);
  }

  delay(1000);
}

double microsecondsToCentimeters(long microseconds) {
  return (double) microseconds / (double) 58;
}

long readSensorValue(int triggerPin, int echoPin) {
  trigerMeasurementSignal(triggerPin);

  // get the value returned by the sensor on port echoPin
  return pulseIn(echoPin, HIGH);
}

void trigerMeasurementSignal(int triggerPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
}
