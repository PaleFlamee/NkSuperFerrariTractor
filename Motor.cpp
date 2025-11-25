#include "NkSuperFerrariTractor.h"

#include "Motor.h"

// MotorCtrlBoard   Arduino
// PWMA             3
// AIN2             4
// AIN1             5
// STBY             7
// BIN1             8
// BIN2             9
// PWMB             10
// 5V
// GND
// ADC              A0
// E1A
// E1B
// E2A
// E2B

// IN1   IN2
//  0     0   Stop
//  1     0   Forward
//  0     1   Reverse
//  1     1   Stop

void MotorSetup() {
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(STBY, OUTPUT);
    digitalWrite(AIN1, 0);// Stop motor Left
    digitalWrite(AIN2, 0);
    digitalWrite(BIN1, 0);// Stop motor Right
    digitalWrite(BIN2, 0);
    digitalWrite(STBY, 1);// Enable motor
    analogWrite(PWMA, 0);
    analogWrite(PWMB, 0);
  
    pinMode(Voltage,INPUT);
}

// God damn comment
void SetPWM(enum MotorID motor, enum MotorDirection direction, unsigned int pwm){
    if(motor == MotorLeft && direction == Forward)  {
        digitalWrite(AIN1, 1);
        digitalWrite(AIN2, 0);
        analogWrite(PWMA, pwm);
    }
    else if(motor == MotorLeft && direction == Backward)  {
        digitalWrite(AIN1, 0);
        digitalWrite(AIN2, 1);
        analogWrite(PWMA, pwm);
    }
    else if(motor == MotorRight && direction == Forward)  {
        digitalWrite(BIN1, 1);
        digitalWrite(BIN2, 0);
        analogWrite(PWMB, pwm);
    }
    else if(motor == MotorRight && direction == Backward)  {
        digitalWrite(BIN1, 0);
        digitalWrite(BIN2, 1);
        analogWrite(PWMB, pwm);
    }
}

void MotorTest() {
    double V = analogRead(Voltage);
    LogSerial.print("Battery Voltage: ");
    LogSerial.print(V * 0.05371);
    LogSerial.println("V");
    delay(1000);

    LogSerial.println("Left Motor Forward, 1/4 speed, 1s");
    SetPWM(MotorLeft, Forward, 64);
    delay(1000);
    SetPWM(MotorLeft, Forward, 0);

    LogSerial.println("Left Motor Backward, 1/4 speed, 1s");
    SetPWM(MotorLeft, Backward, 64);
    delay(1000);
    SetPWM(MotorLeft, Backward, 0);

    LogSerial.println("Right Motor Forward, 1/4 speed, 1s");
    SetPWM(MotorRight, Forward, 64);
    delay(1000);
    SetPWM(MotorRight, Forward, 0);

    LogSerial.println("Left Motor Backward, 1/4 speed, 1s");
    SetPWM(MotorRight, Backward, 64);
    delay(1000);
    SetPWM(MotorRight, Backward, 0);
}
