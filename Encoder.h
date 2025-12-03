#ifndef ENCODER_H
#define ENCODER_H

#include "NkSuperFerrariTractor.h"

// 编码器引脚定义（对应Motor.h）
// #define ENCODER_LEFT_A A1
// #define ENCODER_LEFT_B A2
// #define ENCODER_RIGHT_A A3
// #define ENCODER_RIGHT_B A4
#define ENCODER_LEFT_A 34
#define ENCODER_LEFT_B 35
#define ENCODER_RIGHT_A 36
#define ENCODER_RIGHT_B 39

// 编码器数据结构
struct EncoderData {
  long leftCount;   // 左电机脉冲数
  long rightCount;  // 右电机脉冲数
  float leftSpeed;  // 左电机转速 (r/s)
  float rightSpeed; // 右电机转速 (r/s)
};

// 函数声明
void EncoderSetup();
void EncoderCountLeft();
void EncoderCountRight();
void CalculateEncoderSpeed(struct EncoderData *pData, float sampleTime);

extern struct EncoderData encoderData;

#endif


