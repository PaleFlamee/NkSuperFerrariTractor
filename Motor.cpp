#include "NkSuperFerrariTractor.h"

#include "Motor.h"

// MotorCtrlBoard   Arduino
// PWMA             3
// AIN2             4
// AIN1             5
// STBY             7
// BIN1             8
// BIN2             9
// PWMB             11
// 5V
// GND              
// ADC              
// E1A              A1
// E1B              A2
// E2A              A3
// E2B              A4

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
    pinMode(E1A, OUTPUT);
    pinMode(E1B, OUTPUT);
    pinMode(E2A, OUTPUT);
    pinMode(E2B, OUTPUT);
    digitalWrite(STBY, 1);// Enable motor
    SetPWM(MotorLeft, 0);
    SetPWM(MotorRight, 0);
  
    // pinMode(Voltage,INPUT);
}

// God damn comment
void SetPWM(enum MotorID motor, signed int pwm){
    if (motor == MotorLeft && pwm == 0) {
        digitalWrite(AIN1, 0);
        digitalWrite(AIN2, 0);
        analogWrite(PWMA, pwm);
    }
    else if (motor == MotorLeft && pwm > 0)  {
        digitalWrite(AIN1, 1);
        digitalWrite(AIN2, 0);
        analogWrite(PWMA, pwm);
    }
    else if (motor == MotorLeft && pwm < 0)  {
        digitalWrite(AIN1, 0);
        digitalWrite(AIN2, 1);
        analogWrite(PWMA, -pwm);
    }
    else if (motor == MotorRight && pwm == 0) {
        digitalWrite(BIN1, 0);
        digitalWrite(BIN2, 0);
        analogWrite(PWMB, pwm);
    }
    else if (motor == MotorRight && pwm > 0)  {
        digitalWrite(BIN1, 1);
        digitalWrite(BIN2, 0);
        analogWrite(PWMB, pwm);
    }
    else if (motor == MotorRight && pwm < 0)  {
        digitalWrite(BIN1, 0);
        digitalWrite(BIN2, 1);
        analogWrite(PWMB, -pwm);
    }
}

void MotorTest() {
    // double V = analogRead(Voltage);
    // LogSerial.print("Battery Voltage: ");
    // LogSerial.print(V * 0.05371);
    // LogSerial.println("V");
    // delay(1000);

    LogSerial.println("Left Motor Forward, 1/4 speed, 1s");
    SetPWM(MotorLeft, 64);
    delay(1000);
    SetPWM(MotorLeft, 0);

    LogSerial.println("Left Motor Backward, 1/4 speed, 1s");
    SetPWM(MotorLeft, -64);
    delay(1000);
    SetPWM(MotorLeft, 0);

    LogSerial.println("Right Motor Forward, 1/4 speed, 1s");
    SetPWM(MotorRight, 64);
    delay(1000);
    SetPWM(MotorRight, 0);

    LogSerial.println("Right Motor Backward, 1/4 speed, 1s");
    SetPWM(MotorRight, -64);
    delay(1000);
    SetPWM(MotorRight, 0);
}
