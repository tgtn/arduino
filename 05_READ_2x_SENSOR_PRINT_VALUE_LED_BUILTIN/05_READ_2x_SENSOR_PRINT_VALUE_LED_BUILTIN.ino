#include "Arduino.h"

// senzor 1 (dreapta)
int trigPinSenzorDreapta = 9; // pinul pe care dam senzorului comanda de masurare
int echoPinSenzorDreapta = 10; // pinul pe care primim raspunsul

// senzor 2 (stanga)
#define trigPinSenzorStanga 13 // pinul pe care dam senzorului comanda de masurare
#define echoPinSenzorStanga 2 // pinul pe care primim raspunsul

#define minDistance 20
#define ledPin LED_BUILTIN // led de semnalizare cand un obstacol e detectat
void setup() {
  Serial.begin(9600);
  pinMode(trigPinSenzorDreapta, OUTPUT);
  pinMode(echoPinSenzorDreapta, INPUT);
  pinMode(trigPinSenzorStanga, OUTPUT);
  pinMode(echoPinSenzorStanga, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  double cm1, cm2;

  // citirea de la senzor 1
  cm1 = readSensorValueCm(trigPinSenzorDreapta, echoPinSenzorDreapta);

  // print: 2389ms = 41.19cm
  //              in baza 10;------v  sau simplu: String(duration1)
  // Serial.println("Senzor dreapta: " + String(duration1, 10) + "us = " + String(cm1) + "cm");
  // all the above prints: 2389ms = 41.19cm

  //citirea de la Senzorul 2
  cm2 = readSensorValueCm(trigPinSenzorStanga, echoPinSenzorStanga);

  // print: 2389ms = 41.19cm
  //              in baza 10;------v  sau simplu: String(duration1)
  // Serial.println("Senzor stanga: " + String(duration2, 10) + "us = " + String(cm2) + "cm");
  // all the above prints: 2389ms = 41.19cm

  if ((cm1 > 0 && cm1 < minDistance) || (cm2 > 0 && cm2 < minDistance)) {
    String obstacolPePartea = "dreapta";
    if (cm1 > cm2) {
      obstacolPePartea = "stanga";
    }
     Serial.println("obstacol la senzorul " + obstacolPePartea + " la " + String( min(cm1, cm2)) + "cm");
    // aprindem led-ul builtin
    digitalWrite(ledPin,  HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(100);
}

double microsecondsToCentimeters(long microseconds) {
  return (double) microseconds / (double) 58;
}

double readSensorValueCm(int triggerPin, int echoPin) {
  trigerMeasurementSignal(triggerPin);

  // get the value returned by the sensor on port echoPin
  long duration1 = pulseIn(echoPin, HIGH);

  // convert value read from sensor in microseconds to centimeters
  return microsecondsToCentimeters(duration1);
}

void trigerMeasurementSignal(int triggerPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
}
