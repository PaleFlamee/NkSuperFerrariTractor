#include "NkSuperFerrariTractor.h"
#include "BalanceControl.h"
#include "Motor.h"
#include "Encoder.h"

// PID参数初始化（可根据实际调试调整）
struct PIDParams anglePID = {2.5, 0.1, 0.05, 0.0, 0.0, 200.0};
struct PIDParams speedPID = {1.0, 0.02, 0.0, 0.0, 0.0, 100.0};
struct PIDParams steerPID = {0.8, 0.01, 0.02, 0.0, 0.0, 50.0};

// PID初始化
void PIDInit(struct PIDParams *pid, float kp, float ki, float kd, float outputLimit) {
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  pid->integral = 0.0;
  pid->lastError = 0.0;
  pid->outputLimit = outputLimit;
}

// PID计算核心
float PIDCalculate(struct PIDParams *pid, float setpoint, float feedback, float sampleTime) {
  float error = setpoint - feedback;
  
  // 比例项
  float pTerm = pid->kp * error;
  
  // 积分项（抗积分饱和）
  pid->integral += error * sampleTime;
  pid->integral = constrain(pid->integral, -pid->outputLimit/pid->ki, pid->outputLimit/pid->ki);
  float iTerm = pid->ki * pid->integral;
  
  // 微分项
  float dTerm = pid->kd * (error - pid->lastError) / sampleTime;
  pid->lastError = error;
  
  // 总输出限幅
  float output = constrain(pTerm + iTerm + dTerm, -pid->outputLimit, pid->outputLimit);
  return output;
}

// 平衡控制主逻辑（结合IMU、编码器、循迹数据）
void BalanceControl(struct IMUData *imu, struct EncoderData *encoder, struct LTMData *ltm, enum OperationMode mode) {
  static float balanceAngle = 0.0;  // 平衡基准角度
  float angleOutput, speedOutput, steerOutput;
  int leftPwm, rightPwm;

  // 1. 角度环PID（基于IMU的Pitch角）
  angleOutput = PIDCalculate(&anglePID, balanceAngle, imu->Pitch, 0.01); // 10ms采样

  // 2. 速度环PID（基于编码器速度）
  float avgSpeed = (encoder->leftSpeed + encoder->rightSpeed) / 2.0;
  float speedSet = 0.0;
  
  // 根据运行模式设置速度
  switch(mode) {
    case FollowInertia: speedSet = 2.0; break;  // 惯性前进
    case FollowInertiaReverse: speedSet = -2.0; break; // 惯性倒车
    case FollowLine: speedSet = 1.5; break;     // 循迹行驶
    case Stop: speedSet = 0.0; break;           // 停止
    default: speedSet = 0.0;
  }
  speedOutput = PIDCalculate(&speedPID, speedSet, avgSpeed, 0.01);

  // 3. 转向环PID（基于循迹传感器）
  int lineError = 0;
  if(mode == FollowLine) {
    // 八通道循迹传感器误差计算（0-7对应左到右）
    for(int i=0; i<8; i++) {
      if(ltm->ir[i]) lineError += (i - 3.5) * 10; // 中间为3.5，误差量化
    }
  }
  steerOutput = PIDCalculate(&steerPID, 0.0, lineError, 0.01);

  // 4. 计算电机PWM
  leftPwm = angleOutput + speedOutput + steerOutput;
  rightPwm = angleOutput + speedOutput - steerOutput;
  
  // 限幅并设置电机
  leftPwm = constrain(leftPwm, -255, 255);
  rightPwm = constrain(rightPwm, -255, 255);
  SetPWM(MotorLeft, leftPwm);
  SetPWM(MotorRight, rightPwm);
}
