//#include "DigiKeyboard.h"

double enableThreshold = 4.0;
double disableThreshold = 4.5;
bool mainConnected = false;
bool boardEnabled = false;
int signalPin = 0;
int relayPin = 2;
int ledPin = 1;

double lastReading = 0;
long voltage = 0;
double decimalVoltage = 0;

void setup()
{
	pinMode(ledPin, OUTPUT);
	pinMode(signalPin, OUTPUT);
	pinMode(relayPin, OUTPUT);
	digitalWrite(relayPin, HIGH);
	analogWrite(signalPin, 0); // High 3.3 v
}

void loop()
{

	//read and convert the voltage to a double
	long voltage = readVcc();
	double decimalVoltage = doubleMap(double(voltage), 0, 6000, 0, 6);
	if (decimalVoltage > lastReading || decimalVoltage > 4.9)
	{
		mainConnected = true;
		blinkThreeTimes();
	}
	else
	{
		mainConnected = false;
		blinkOnes();
	}
	lastReading = decimalVoltage;


	if (mainConnected && !boardEnabled && lastReading > enableThreshold)
	{
		analogWrite(signalPin, 160); // High 3.3 v
		digitalWrite(relayPin, LOW); // Enable board
		boardEnabled = true;
	}

	if (boardEnabled && !mainConnected && lastReading < disableThreshold)
	{
		analogWrite(signalPin, 0);
		digitalWrite(ledPin, HIGH);
		delay(30000);
		digitalWrite(relayPin, HIGH); // Enable board
		boardEnabled = false;
		digitalWrite(ledPin, LOW);
	}

	delay(2000);


	//// this is generally not necessary but with some older systems it seems to
	//// prevent missing the first character after a delay:
	//DigiKeyboard.sendKeyStroke(0);

	//// Type out this string letter by letter on the computer (assumes US-style
	//// keyboard)
	//DigiKeyboard.print("Voltage:");
	//DigiKeyboard.println(decimalVoltage);

	//// It's better to use DigiKeyboard.delay() over the regular Arduino delay()
	//// if doing keyboard stuff because it keeps talking to the computer to make
	//// sure the computer knows the keyboard is alive and connected
	//DigiKeyboard.delay(5000);
}

void blinkThreeTimes() {
	digitalWrite(ledPin, HIGH);
	delay(200);
	digitalWrite(ledPin, LOW);
	delay(200);
	digitalWrite(ledPin, HIGH);
	delay(200);
	digitalWrite(ledPin, LOW);
	delay(200);
	digitalWrite(ledPin, HIGH);
	delay(200);
	digitalWrite(ledPin, LOW);
}

void blinkTwice() {
	digitalWrite(ledPin, HIGH);
	delay(200);
	digitalWrite(ledPin, LOW);
	delay(200);
	digitalWrite(ledPin, HIGH);
	delay(200);
	digitalWrite(ledPin, LOW);
}

void blinkOnes() {
	digitalWrite(ledPin, HIGH);
	delay(200);
	digitalWrite(ledPin, LOW);
}

double doubleMap(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

long readVcc() {
	// Read 1.1V reference against AVcc
	// set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
	ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
	ADMUX = _BV(MUX3) | _BV(MUX2);
#else
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif  

	delay(2); // Wait for Vref to settle
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA, ADSC)); // measuring

	uint8_t low = ADCL; // must read ADCL first - it then locks ADCH  
	uint8_t high = ADCH; // unlocks both

	long result = (high << 8) | low;

	result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
	return result; // Vcc in millivolts
}