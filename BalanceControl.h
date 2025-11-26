#ifndef BALANCE_CONTROL_H
#define BALANCE_CONTROL_H

#include "NkSuperFerrariTractor.h"
#include "Motor.h"

// 配置参数
#define TARGET_ANGLE 0.5f      // 目标平衡角度（需校准，单位：°）
#define ANGLE_LIMIT 30.0f      // 最大安全角度（超过则停止，单位：°）
#define BASE_SPEED 0           // 基础前进速度（0为静止平衡，可正负调节）
#define MAX_PWM 200            // 电机最大PWM值
#define MIN_PWM -200           // 电机最小PWM值

// PID参数（需根据实际调试优化）
#define KP 4.2f                // 比例系数（角度环）
#define KI 0.0f                // 积分系数（角度环）
#define KD 0.0f                // 微分系数（角速度环）
#define INTEGRAL_LIMIT 50.0f   // 积分限幅（防止积分饱和）

// 函数声明
void balanceControl(struct IMUData *imu);  // 直立控制主函数
float getCurrentAngle(struct IMUData *imu); // 获取当前角度
void stopMotors();                         // 停止所有电机

#endif