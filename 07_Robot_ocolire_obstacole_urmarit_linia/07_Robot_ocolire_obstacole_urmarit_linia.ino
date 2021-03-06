#include "Arduino.h"
#include <NewPing.h>

boolean proximityDetection = true;
// senzor 1 (dreapta)
int trigPinSenzorDreapta = 9; // pinul pe care dam senzorului comanda de masurare
int echoPinSenzorDreapta = 10; // pinul pe care primim raspunsul

// senzor 2 (stanga)
#define trigPinSenzorStanga 13 // pinul pe care dam senzorului comanda de masurare
#define echoPinSenzorStanga 2 // pinul pe care primim raspunsul

double distantaDreapta, distantaStanga;
#define minDistance 16
#define ledPin LED_BUILTIN // led de semnalizare obstacol detectat

// viteza motoarelor
/*#define VITEZA_MOTOR_DREAPTA 87
 #define VITEZA_MOTOR_STANGA 96*/
#define VITEZA_MOTOR_DREAPTA 56
#define VITEZA_MOTOR_STANGA 56

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

#define pinLineSensorR A0
#define pinLineSensorC A1
#define pinLineSensorL A2

int lineSensorValueR = 0;
int lineSensorValueC = 0;
int lineSensorValueL = 0;

int lastLineSensorState = 0;

NewPing distSensorRight(trigPinSenzorDreapta, echoPinSenzorDreapta, minDistance);
NewPing distSensorLeft(trigPinSenzorStanga, echoPinSenzorStanga, minDistance);

void setup() {
	Serial.begin(9600);
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

	pinMode(pinLineSensorR, INPUT);
	pinMode(pinLineSensorC, INPUT);
	pinMode(pinLineSensorL, INPUT);

	directie(inainte, inainte);
	viteza(0, 0);

	delay(1000);
	Serial.println(1);
	delay(1000);
	Serial.println(2);
	delay(1000);
	Serial.println(3);
	delay(1000);
	Serial.println(4);
	delay(1000);
	Serial.println(5);
}

void loop() {
	if (proximityDetection) {
		// citirea de la senzor dreapta
		distantaStanga = distSensorLeft.ping_cm();
		// Serial.println("Senzor stanga: " + String(distantaStanga) + "cm");

		//citirea de la Senzorul stanga
		distantaDreapta = distSensorRight.ping_cm();
		// Serial.println("Senzor dreapta: " + String(distantaDreapta) + "cm");
	}

	lineSensorValueL = digitalRead(pinLineSensorL);
	lineSensorValueC = digitalRead(pinLineSensorC);
	lineSensorValueR = digitalRead(pinLineSensorR);
	// Serial.println(lineSensorValueL);
	// Serial.println(lineSensorValueC);
	// Serial.println(lineSensorValueR);

	int lineSensorVals[3] = { lineSensorValueL, lineSensorValueC,
			lineSensorValueR };
	int lineSensorState = binaryArrayToInt(lineSensorVals, 3);
	// Serial.print("===== line sensor state = ");
	// Serial.println(lineSensorState);

	if ((distantaDreapta > 0 && distantaDreapta < minDistance)
			|| (distantaStanga > 0 && distantaStanga < minDistance)) {
		String obstacolPePartea = "dreapta";
		if (distantaDreapta == 0
				|| (distantaStanga > 0 && distantaStanga < distantaDreapta)) {
			obstacolPePartea = "stanga";
		}
		Serial.println("Detectat obstacol la senzorul " + obstacolPePartea + "(" + String(distantaStanga) + ", " + String(distantaDreapta) + ")");

		// aprindem led-ul builtin
		digitalWrite(ledPin, HIGH);

		if (obstacolPePartea == "dreapta") {
			rotireRobot(inainte, inapoi);
		} else {
			rotireRobot(inapoi, inainte);
		}
		lastLineSensorState = 0;
	} else {
		if (lineSensorState == 0) {
			// stop
			viteza(0, 0);
		} else if (lineSensorState == 2 || lineSensorState == 3
				|| lineSensorState == 4) {
			// viraj stanga in cautarea liniei
			viteza(VITEZA_MOTOR_DREAPTA + 30, 0);
			// Serial.println("Cauta stanga");
		} else if (lineSensorState == 1 || lineSensorState == 6) {
			// viraj dreapta in cautarea liniei
			viteza(0, VITEZA_MOTOR_STANGA + 30);
			// Serial.println("Cauta dreapta");
		} else if (lineSensorState == 7 && lastLineSensorState != 0) {
			if (lastLineSensorState == 6) {
				// curba usoara dreapta
				viteza(VITEZA_MOTOR_DREAPTA - 10, VITEZA_MOTOR_STANGA + 20);
			} else if (lastLineSensorState == 3) {
				// curba usoara stanga
				viteza(VITEZA_MOTOR_DREAPTA + 20, VITEZA_MOTOR_STANGA - 10);
			}
		} else {
			directie(inainte, inainte);
			viteza(VITEZA_MOTOR_DREAPTA, VITEZA_MOTOR_STANGA);
		}

		// daca OK salvam ultima pozition
		if (lineSensorState != 7) {
			if (lineSensorState == 5) {
				lastLineSensorState = 0;
			} else  {
				lastLineSensorState = lineSensorState;
			}
		}
	}

	//	delay(100);
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

	digitalWrite(ledPin, LOW);
}

int binaryArrayToInt(int s[], int size) {
	int n = 0;

	for (int i = 0; i < size; ++i) {
		n <<= 1;
		n += s[i];
	}

	return n;
}
