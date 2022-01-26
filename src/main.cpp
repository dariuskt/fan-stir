#include <Arduino.h>
#include <PID_v1.h>


#define TACH  D5
#define PWM   D1
#define POT   A0
#define POT_POSITIVE  D6
#define POT_NEGATIVE  D0

#define DBG   D2

volatile unsigned long thisTick,lastTick,iTick = 0;
volatile byte rpm_sample = 0;
double RPM = 0;
double duty = 0;
double setpoint = 1000;

// PID configuration
double Kp=0.3, Ki=0.1, Kd=0;
PID myPID(&RPM, &duty, &setpoint, Kp, Ki, Kd, DIRECT);

// RPM counter interrupt
void ICACHE_RAM_ATTR handleInterrupt() {
    digitalWrite(DBG, !digitalRead(DBG));
    rpm_sample++;
    thisTick = micros();
    if ( (thisTick-iTick) < 2500 ) {
        return;
    } else {
        iTick=thisTick;
    }
    if (rpm_sample > 16) {
        RPM = 60 * 1000 * 1000 / (thisTick - lastTick);
        lastTick = thisTick;
        rpm_sample = 0;

    }
}
// ===============================================

void setup() {
    Serial.begin(76800);
    Serial.println("\nBooting... ");

    pinMode(TACH, INPUT_PULLUP);
    pinMode(PWM, OUTPUT);
    pinMode(POT, INPUT);
    pinMode(DBG, OUTPUT);

    analogWriteFreq(100);

    pinMode(POT_POSITIVE, OUTPUT);
    digitalWrite(POT_POSITIVE, HIGH);
    pinMode(POT_NEGATIVE, OUTPUT);
    digitalWrite(POT_NEGATIVE, LOW);


    attachInterrupt(digitalPinToInterrupt(TACH), handleInterrupt, FALLING);
    myPID.SetMode(AUTOMATIC);
 }

void loop() {
    if ( (micros() - lastTick) > 200000 ) {
        RPM = 0;
        Serial.println("reset");
    }
    delay(100);

    setpoint = analogRead(POT) * 1;

    // RPM jitter is too great, cant use PID for this fan
    //myPID.Compute();
    //duty *= 4;
    //if (duty > 1020) duty = 1020;
    //if (duty < 10) duty = 10;

    //analogWrite(PWM, (int)duty);
    analogWrite(PWM, (int)setpoint);


    Serial.print((int)setpoint);
    Serial.print(" . ");
    Serial.print((int)RPM);
    Serial.print(" . ");
    Serial.println((int)duty);

}
