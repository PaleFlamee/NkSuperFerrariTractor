#include "NkSuperFerrariTractor.h"
#include "Encoder.h"
#include "Motor.h"

struct EncoderData encoderData = {0, 0, 0.0, 0.0};
static unsigned long lastLeftTime = 0;
static unsigned long lastRightTime = 0;

// 编码器初始化（外部中断配置）
void EncoderSetup() {
  pinMode(ENCODER_LEFT_A, INPUT);
  pinMode(ENCODER_LEFT_B, INPUT);
  pinMode(ENCODER_RIGHT_A, INPUT);
  pinMode(ENCODER_RIGHT_B, INPUT);
  
  // 开启外部中断（根据Arduino引脚映射，A1/A2/A3/A4对应中断需匹配硬件）
  attachInterrupt(digitalPinToInterrupt(ENCODER_LEFT_A), EncoderCountLeft, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_RIGHT_A), EncoderCountRight, RISING);
}

// 左编码器计数中断
void EncoderCountLeft() {
  encoderData.leftCount++;
  lastLeftTime = millis();
}

// 右编码器计数中断
void EncoderCountRight() {
  encoderData.rightCount++;
  lastRightTime = millis();
}

// 计算电机转速（sampleTime为采样时间，单位：秒）
void CalculateEncoderSpeed(struct EncoderData *pData, float sampleTime) {
  // 编码器每圈13脉冲，减速比为30
  float pulsesPerRev = 13.0 * 30.0;
  pData->leftSpeed = (pData->leftCount / pulsesPerRev) / sampleTime;
  pData->rightSpeed = (pData->rightCount / pulsesPerRev) / sampleTime;
  
  // 重置计数
  pData->leftCount = 0;
  pData->rightCount = 0;
}