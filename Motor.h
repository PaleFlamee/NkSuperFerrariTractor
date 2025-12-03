#ifndef MOTOR_H
#define MOTOR_H

//Pin definitions
// #define PWMA 3
// #define AIN2 4
// #define AIN1 5
// #define STBY 7
// #define BIN1 8
// #define BIN2 9
// #define PWMB 11

#define PWMA 32
#define AIN2 25
#define AIN1 26
#define STBY 13
#define BIN1 27
#define BIN2 14
#define PWMB 33

// #define E1A A1
// #define E1B A2
// #define E2A A3
// #define E2B A4

#define E1A 34
#define E1B 35
#define E2A 36
#define E2B 39
// #define Voltage A0 // Battery voltage reading pin, analog input

// Motor identifiers
enum MotorID {
    MotorLeft = 0, // Motor A
    MotorRight = 1 // Motor B 仅供参考别TM再用AB了
};

// Function declarations
void MotorSetup();
void SetPWM(enum MotorID motor, signed int pwm);
void MotorTest();

#endif // MOTOR_H