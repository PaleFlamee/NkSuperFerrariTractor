#ifndef BALANCE_CONTROL_H
#define BALANCE_CONTROL_H

#include "NkSuperFerrariTractor.h"
#include "FetchData"
#include "Encoder.h"

// PID参数结构体
struct PIDParams {
  float kp;
  float ki;
  float kd;
  float integral;
  float lastError;
  float outputLimit;
};

// 函数声明
void PIDInit(struct PIDParams *pid, float kp, float ki, float kd, float outputLimit);
float PIDCalculate(struct PIDParams *pid, float setpoint, float feedback, float sampleTime);
void BalanceControl(struct IMUData *imu, struct EncoderData *encoder, struct LTMData *ltm, enum OperationMode mode);

// 全局PID实例（角度、速度、转向）
extern struct PIDParams anglePID;
extern struct PIDParams speedPID;
extern struct PIDParams steerPID;

#endif

