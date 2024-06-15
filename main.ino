#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo servo1;

const int TEMPERATURE_PIN = A0;
const int BUTTON_PIN = 7;
const int RED_PIN = 10;
const int GREEN_PIN = 9;
const int BLUE_PIN = 6;

// seconds for how long the moving temperature average is
const int AVERAGE_TEMPERATURE_TIME = 120;
const int READING_LENGTH = 1000; // milliseconds
// Prior tries:
// 79.1 / 140.0; //1.0; //79.1 / 164.0; //79.1 / 112; //1.0; //1.0; //0.9039;
const float TEMPERATURE_CALIBRATION = 79.1 / 155.0;

const int TOTAL_READINGS = AVERAGE_TEMPERATURE_TIME / (READING_LENGTH / 1000);

float servoPosition = 0;
float servoChangeEachTime = 0.05; // alternates between -0.05 and 0.05
int fList[TOTAL_READINGS] = {};
float tempF = 0;
int run = 0;
float degreesF;
int time;

float storedTemp = 70.0;
float displayTemp = 70.0;
byte lastButtonState = LOW;
byte ledState = LOW;

void setup()
{
	lcd.begin(16, 2); // initialize the 16x2 LCD
	lcd.clear();
	lcd.print("Happy grad!  108");

    // connect the servo to pin 9 with a min pulse width of 900 and a max pulse
    // width of 2100
    servo1.attach(8, 900, 2100);
  
    for (int i=0; i<TOTAL_READINGS; i++) {
        fList[i] = 70.0;
    }

    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
}

void loop()
{
    time = millis() / 1000;
    
    // turn off or on the LED when the button is pressed
    byte buttonState = digitalRead(BUTTON_PIN);
    if (buttonState != lastButtonState) {
        lastButtonState = buttonState;

        if (buttonState == LOW) {
            ledState = (ledState == HIGH) ? LOW : HIGH;
        }
    }

    if (ledState == HIGH) { // set multicolor LED to blink
        if (time % 7 == 0) {
            digitalWrite(RED_PIN, HIGH);
            digitalWrite(GREEN_PIN, HIGH);
            digitalWrite(BLUE_PIN, HIGH);

        } else if (time % 6 == 0) {
            digitalWrite(RED_PIN, HIGH);
            digitalWrite(GREEN_PIN, LOW);
            digitalWrite(BLUE_PIN, HIGH);

        } else if (time % 5 == 0) {
            digitalWrite(RED_PIN, LOW);
            digitalWrite(GREEN_PIN, HIGH);
            digitalWrite(BLUE_PIN, HIGH);

        } else if (time % 4 == 0) {
            digitalWrite(RED_PIN, HIGH);
            digitalWrite(GREEN_PIN, HIGH);
            digitalWrite(BLUE_PIN, LOW);

        } else if (time % 3 == 0) {
            digitalWrite(RED_PIN, LOW);
            digitalWrite(GREEN_PIN, LOW);
            digitalWrite(BLUE_PIN, HIGH);

        } else if (time % 2 == 0) {
            digitalWrite(RED_PIN, LOW);
            digitalWrite(GREEN_PIN, HIGH);
            digitalWrite(BLUE_PIN, LOW);

        } else {
            digitalWrite(RED_PIN, HIGH);
            digitalWrite(GREEN_PIN, LOW);
            digitalWrite(BLUE_PIN, LOW);
        }
    } else {
        digitalWrite(RED_PIN, LOW);
        digitalWrite(GREEN_PIN, LOW);
        digitalWrite(BLUE_PIN, LOW);
    }

    degreesF = (analogRead(TEMPERATURE_PIN) * TEMPERATURE_CALIBRATION - 50) * (9.0 / 5.0) + 32;

    if (run == TOTAL_READINGS) {
        run = 0; // reset for moving average
    }
    fList[run] = degreesF;
    run += 1;
    
    tempF = 0;
    for (int i=0; i<TOTAL_READINGS; i++) {
        tempF += fList[i];
    }
    tempF /= TOTAL_READINGS; // get average

    if (tempF > 99) {
        tempF = 99;
    } else if (tempF < 60) {
        tempF = 60;
    }

    // fixes irregularities in first 1s
    if (millis() % 5000 < READING_LENGTH) {
        displayTemp = storedTemp;
    } else {
        storedTemp = tempF;
        displayTemp = tempF;
    }

    lcd.setCursor(0, 1); // column 0, row 1
    lcd.print(time); // seconds since the Arduino last reset

    lcd.setCursor(6, 1);
    lcd.print("Temp: ");

    lcd.setCursor(12, 1);
    lcd.print(displayTemp);

    servo1.write(round(servoPosition));

    servoPosition += servoChangeEachTime;
    if (servoPosition >= 160 || servoPosition <= 0) { // so servo oscillates
        servoChangeEachTime = -servoChangeEachTime;
    }
}
