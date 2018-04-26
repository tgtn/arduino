#include "Arduino.h"
// senzor 1 (dreapta)
int trigPinSenzorDreapta = 9; // pinul pe care dam senzorului comanda de masurare
int echoPinSenzorDreapta = 10; // pinul pe care primim raspunsul

// senzor 2 (stanga)
#define trigPinSenzorStanga 13 // pinul pe care dam senzorului comanda de masurare
#define echoPinSenzorStanga 2 // pinul pe care primim raspunsul

double distantaDreapta, distantaStanga;
#define minDistance 20
#define ledPin LED_BUILTIN // led de semnalizare cand un obstacol e detectat

// viteza motoarelor
#define VITEZA_MOTOR_DREAPTA 99
#define VITEZA_MOTOR_STANGA 106

//definiti pinii utilizati pentru controlul motoarelor
//pinii enA si enB sunt folositi pentru a controla viteza motoarelor in functie de semnalul PWM
//pinii in1, in2, in3 si in4 sunt utilizati pentru a controla directia motoarelor(inainte sau inapoi)
//pinii folositi de primul motor
#define pinVitezaMotorDreapta 11 // enA
#define pinInainteMotorDreapta 6 // in1
#define pinInapoiMotorDreapta 7 // in2

//pinii folositi de al doilea motor
#define pinVitezaMotorStanga 3 // enB
#define pinInainteMotorStanga 5 //in3
#define pinInapoiMotorStanga 4 //in4

//
#define inainte HIGH
#define inapoi LOW

void setup() {
	Serial.begin(9600);
	pinMode(trigPinSenzorDreapta, OUTPUT);
	pinMode(echoPinSenzorDreapta, INPUT);
	pinMode(trigPinSenzorStanga, OUTPUT);
	pinMode(echoPinSenzorStanga, INPUT);
	pinMode(ledPin, OUTPUT);

	//toti pinii utilizati de driverul L298N sunt OUTPUT
	// pinii utilizati de primul motor
	pinMode(pinVitezaMotorDreapta, OUTPUT);
	pinMode(pinInainteMotorDreapta, OUTPUT);
	pinMode(pinInapoiMotorDreapta, OUTPUT);
	//pinii utilizati de al doilea motor
	pinMode(pinVitezaMotorStanga, OUTPUT);
	pinMode(pinInainteMotorStanga, OUTPUT);
	pinMode(pinInapoiMotorStanga, OUTPUT);

	directie(inainte, inainte);
	viteza(0, 0);

	delay(5000);
}

void loop() {
	directie(inainte, inainte);
	viteza(VITEZA_MOTOR_DREAPTA, VITEZA_MOTOR_STANGA);

	// citirea de la senzor 1
	distantaDreapta = readSensorValueCm(trigPinSenzorDreapta, echoPinSenzorDreapta);

	// print: 2389ms = 41.19cm
	//              in baza 10;------v  sau simplu: String(duration1)
	// Serial.println("Senzor dreapta: " + String(duration1, 10) + "us = " + String(cm1) + "cm");
	// all the above prints: 2389ms = 41.19cm

	//citirea de la Senzorul 2
	distantaStanga = readSensorValueCm(trigPinSenzorStanga,
	echoPinSenzorStanga);

	// print: 2389ms = 41.19cm
	//              in baza 10;------v  sau simplu: String(duration1)
	// Serial.println("Senzor stanga: " + String(duration2, 10) + "us = " + String(cm2) + "cm");
	// all the above prints: 2389ms = 41.19cm

	if ((distantaDreapta > 0 && distantaDreapta < minDistance)
			|| (distantaStanga > 0 && distantaStanga < minDistance)) {
		String obstacolPePartea = "dreapta";
		if (distantaStanga < distantaDreapta) {
			obstacolPePartea = "stanga";
		}
		Serial.println(
				"obstacol la senzorul " + obstacolPePartea + " la "
						+ String(min(distantaDreapta, distantaStanga)) + "cm");
		// aprindem led-ul builtin
		digitalWrite(ledPin, HIGH);

		if (obstacolPePartea == "dreapta") {
			rotireRobot(inainte, inapoi);
		} else {
			rotireRobot(inapoi, inainte);

		}

	} else {
		digitalWrite(ledPin, LOW);
	}

	delay(100);
}

void directie(int directiaMotorDreapta, int directiaMotorStanga) {
	digitalWrite(pinInainteMotorDreapta, directiaMotorDreapta);
	digitalWrite(pinInapoiMotorDreapta, !directiaMotorDreapta);

	digitalWrite(pinInainteMotorStanga, directiaMotorStanga);
	digitalWrite(pinInapoiMotorStanga, !directiaMotorStanga);
}

void viteza(int vitezaMotorDreapta, int vitezaMotorStanga) {
	analogWrite(pinVitezaMotorDreapta, vitezaMotorDreapta);
	analogWrite(pinVitezaMotorStanga, vitezaMotorStanga);
}

void rotireRobot(int directieMotorDreapta, int directieMotorStanga) {
	directie(inapoi, inapoi);
	viteza(0, 0);

	delay(1000);

	viteza(VITEZA_MOTOR_DREAPTA, VITEZA_MOTOR_STANGA);

	delay(500);

	directie(directieMotorDreapta, directieMotorStanga);

	viteza(VITEZA_MOTOR_DREAPTA, VITEZA_MOTOR_STANGA);
	delay(300);
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
