#ifndef MOTOR_H
#define MOTOR_H

// Pin definitions
#define PWMA 3
#define AIN2 4
#define AIN1 5
#define STBY 7
#define BIN1 8
#define BIN2 9
#define PWMB 10
#define Voltage A0 // Battery voltage reading pin, analog input

// Motor identifiers
enum MotorID {
    MotorLeft = 0, // Motor A
    MotorRight = 1 // Motor B 仅供参考别TM再用AB了
};
enum MotorDirection {
    Forward = 0,
    Backward = 1
};

// Function declarations
void MotorSetup();
void SetPWM(enum MotorID motor, enum MotorDirection direction, unsigned int pwm);
void MotorTest();

#endif // MOTOR_H