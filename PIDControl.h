#ifndef PID_CONTROL_H
#define PID_CONTROL_H

#include "NkSuperFerrariTractor.h"
#include "Motor.h"
#include "FetchData.h"

// 控制模式枚举（用于任务切换）
typedef enum {
    MODE_BALANCE,    // 仅平衡
    MODE_FORWARD,    // 前进
    MODE_BACKWARD,   // 后退
    MODE_TURN_LEFT,  // 左转
    MODE_TURN_RIGHT, // 右转
    MODE_STOP        // 停止
} ControlMode;

// PID参数结构体
typedef struct {
    // 直立环参数
    float kp_balance;
    float ki_balance;
    float kd_balance;
    float balance_offset;  // 平衡中点补偿

    // 速度环参数
    float kp_speed;
    float ki_speed;
    float speed_max;       // 最大速度限制

    // 转向环参数
    float kp_turn;
    float kd_turn;
    float turn_max;        // 最大转向限制
} PIDParams;

// PID控制器结构体
typedef struct {
    // 传感器数据
    IMUData imu;
    LTMData ltm;

    // 控制状态
    ControlMode mode;
    float target_speed;    // 目标速度（前进/后退）
    float target_yaw;      // 目标偏航角（转向）

    // PID计算中间变量
    float angle;           // 当前倾角
    float last_angle;      // 上一时刻倾角
    float balance_error;   // 直立环误差
    float balance_integral;// 直立环积分
    float balance_output;  // 直立环输出

    float speed;           // 当前速度（编码器/陀螺仪估算）
    float speed_error;     // 速度环误差
    float speed_integral;  // 速度环积分
    float speed_output;    // 速度环输出

    float yaw_error;       // 转向环误差
    float last_yaw_error;  // 上一时刻转向误差
    float turn_output;     // 转向环输出

    // 电机输出
    int left_pwm;
    int right_pwm;
} PIDController;

// 函数声明
void pidInit(PIDController* pid, PIDParams* params);
void pidUpdate(PIDController* pid);
void pidSetMode(PIDController* pid, ControlMode mode);
void pidSetTargetSpeed(PIDController* pid, float speed);
void pidSetTargetTurn(PIDController* pid, float yaw_offset);

#endif