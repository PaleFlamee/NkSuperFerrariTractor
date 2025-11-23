#ifndef MOTOR_H
#define MOTOR_H

// Pin definitions
#define PWMA 3
#define AIN1 5
#define AIN2 4
#define PWMB 10
#define BIN1 8
#define BIN2 9
#define STBY 7
#define Voltage A0 // Battery voltage reading pin, analog input

// Motor identifiers
#define MOTOR_LEFT 1  // Motor A
#define MOTOR_RIGHT 2 // Motor B

// Function declarations
void MotorSetup();
void SetPWM(int motor, int pwm);
void MotorTest();

#endif // MOTOR_H