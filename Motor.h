#ifndef MOTOR_H
#define MOTOR_H

// Pin definitions
#define PWMA 3  //3为模拟引脚，用于PWM控制
#define AIN1 5
#define AIN2 4
#define PWMB 10  //10为模拟引脚，用于PWM控制
#define BIN1 8
#define BIN2 9
#define STBY 7  //2、4、8、12、7为数字引脚，用于开关量控制
#define Voltage A0 //使用模拟引脚

// Motor identifiers
#define MOTOR_LEFT 1  // Motor A
#define MOTOR_RIGHT 2 // Motor B

// Function declarations
void MotorSetup();
void SetPWM(int motor, int pwm);
void MotorTest();

#endif // MOTOR_H