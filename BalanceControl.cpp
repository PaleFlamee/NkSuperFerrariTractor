#include "BalanceControl.h"
#include "Motor.h"

// PID变量（静态全局，仅本文件可见）
static float error = 0.0f;         // 角度误差
static float last_error = 0.0f;    // 上一次角度误差
static float integral = 0.0f;      // 积分项
static float derivative = 0.0f;    // 微分项（角速度）

// 获取当前角度（使用俯仰角）
float getCurrentAngle(struct IMUData *imu) {
  return imu->Pitch;  // 基于IMU的俯仰角控制（需确保安装方向正确）
}

// 计算PID输出
static float calculatePID(float currentAngle, float currentGyro) {
  // 1. 计算角度误差
  error = currentAngle - TARGET_ANGLE;

  // 2. 计算积分项（带限幅，防止累积过大）
  integral += error;
  if (integral > INTEGRAL_LIMIT) {
    integral = INTEGRAL_LIMIT;
  } else if (integral < -INTEGRAL_LIMIT) {
    integral = -INTEGRAL_LIMIT;
  }

  // 3. 计算微分项（使用角速度，抑制震荡）
  derivative = currentGyro;  // 直接使用Y轴角速度（与俯仰角对应）

  // 4. 保存当前误差用于下次计算
  last_error = error;

  // 5. 计算PID总输出（角度环P+I + 角速度环D）
  return KP * error + KI * integral + KD * derivative;
}

// 直立控制主函数
void balanceControl(struct IMUData *imu) {
  // 获取当前角度和角速度
  float currentAngle = getCurrentAngle(imu);
  float currentGyro = imu->gyroY;  // Y轴角速度（与俯仰角对应）

  // 计算PID控制量
  float pidOutput = calculatePID(currentAngle, currentGyro);

  // 计算左右轮PWM（直立控制量 + 基础速度）
  int leftPWM = BASE_SPEED - pidOutput;
  int rightPWM = BASE_SPEED - pidOutput;

  // 限制PWM范围（防止超过电机驱动能力）
  leftPWM = constrain(leftPWM, MIN_PWM, MAX_PWM);
  rightPWM = constrain(rightPWM, MIN_PWM, MAX_PWM);

  // 输出到电机
  SetPWM(MotorLeft, leftPWM);
  SetPWM(MotorRight, rightPWM);

  // 调试信息输出（可选）
  #ifdef DEBUG_IMU
    LogSerial.print("Angle: ");
    LogSerial.print(currentAngle);
    LogSerial.print(" | PWM: ");
    LogSerial.print(leftPWM);
    LogSerial.print(", ");
    LogSerial.println(rightPWM);
  #endif
}

// 停止所有电机
void stopMotors() {
  SetPWM(MotorLeft, 0);
  SetPWM(MotorRight, 0);
}