#include "PIDControl.h"

// 默认PID参数（根据实际机械结构调整）
static const PIDParams DEFAULT_PARAMS = {
    // 直立环（核心参数，需精细调试）
    .kp_balance = 6.5f,    // 比例系数
    .ki_balance = 0.02f,   // 积分系数（抑制静差）
    .kd_balance = 0.8f,    // 微分系数（抑制震荡）
    .balance_offset = 0.5f,// 平衡中点补偿（机械重心偏差）

    // 速度环（控制前进后退）
    .kp_speed = 2.0f,
    .ki_speed = 0.1f,
    .speed_max = 40.0f,    // 最大速度（对应PWM限幅）

    // 转向环（控制左右转向）
    .kp_turn = 1.2f,
    .kd_turn = 0.1f,
    .turn_max = 30.0f      // 最大转向偏差（度）
};

// 初始化PID控制器
void pidInit(PIDController* pid, PIDParams* params) {
    // 初始化传感器数据
    memset(&pid->imu, 0, sizeof(IMUData));
    memset(&pid->ltm, 0, sizeof(LTMData));

    // 初始化控制模式
    pid->mode = MODE_STOP;
    pid->target_speed = 0.0f;
    pid->target_yaw = 0.0f;

    // 初始化PID中间变量
    pid->angle = 0.0f;
    pid->last_angle = 0.0f;
    pid->balance_error = 0.0f;
    pid->balance_integral = 0.0f;
    pid->balance_output = 0.0f;

    pid->speed = 0.0f;
    pid->speed_error = 0.0f;
    pid->speed_integral = 0.0f;
    pid->speed_output = 0.0f;

    pid->yaw_error = 0.0f;
    pid->last_yaw_error = 0.0f;
    pid->turn_output = 0.0f;

    pid->left_pwm = 0;
    pid->right_pwm = 0;

    // 加载PID参数（使用默认参数如果未指定）
    if (params == NULL) {
        memcpy(&pid->params, &DEFAULT_PARAMS, sizeof(PIDParams));
    } else {
        memcpy(&pid->params, params, sizeof(PIDParams));
    }
}

// 计算速度（基于陀螺仪Y轴角速度估算）
static float calculateSpeed(IMUData* imu) {
    // 前进/后退时，Y轴角速度与速度正相关（需校准）
    return imu->gyroY * 0.5f;  // 系数根据实际测试调整
}

// 更新PID控制（主控制逻辑）
void pidUpdate(PIDController* pid) {
    // 1. 读取传感器数据
    fetchIMUData(&pid->imu);
    fetchLTMData(&pid->ltm);  // 循迹数据（预留扩展）

    // 2. 直立环控制（核心：维持平衡）
    pid->angle = pid->imu.Pitch + pid->params.balance_offset;  // 倾角=Pitch+补偿
    pid->balance_error = pid->angle;                           // 目标倾角为0（平衡位置）
    
    // 积分项限幅（防止积分饱和）
    pid->balance_integral += pid->balance_error;
    pid->balance_integral = constrain(pid->balance_integral, -50.0f, 50.0f);
    
    // 微分项（抑制震荡）
    float balance_derivative = pid->angle - pid->last_angle;
    pid->last_angle = pid->angle;
    
    // 直立环输出
    pid->balance_output = 
        pid->params.kp_balance * pid->balance_error +
        pid->params.ki_balance * pid->balance_integral +
        pid->params.kd_balance * balance_derivative;

    // 3. 速度环控制（前进/后退）
    pid->speed = calculateSpeed(&pid->imu);
    pid->speed_error = pid->target_speed - pid->speed;
    
    // 积分项限幅
    pid->speed_integral += pid->speed_error;
    pid->speed_integral = constrain(pid->speed_integral, -100.0f, 100.0f);
    
    // 速度环输出
    pid->speed_output = 
        pid->params.kp_speed * pid->speed_error +
        pid->params.ki_speed * pid->speed_integral;
    pid->speed_output = constrain(pid->speed_output, -pid->params.speed_max, pid->params.speed_max);

    // 4. 转向环控制（左右转向）
    // 目标偏航角 = 初始偏航角 + 转向偏移（通过模式设置）
    pid->yaw_error = pid->target_yaw - pid->imu.Yaw;
    
    // 微分项（抑制转向震荡）
    float turn_derivative = pid->yaw_error - pid->last_yaw_error;
    pid->last_yaw_error = pid->yaw_error;
    
    // 转向环输出
    pid->turn_output = 
        pid->params.kp_turn * pid->yaw_error +
        pid->params.kd_turn * turn_derivative;
    pid->turn_output = constrain(pid->turn_output, -pid->params.turn_max, pid->params.turn_max);

    // 5. 模式映射（将模式转换为目标值）
    switch (pid->mode) {
        case MODE_BALANCE:
            pid->target_speed = 0.0f;
            pid->target_yaw = pid->imu.Yaw;  // 维持当前方向
            break;
        case MODE_FORWARD:
            pid->target_speed = pid->params.speed_max * 0.6f;  // 60%最大速度
            pid->target_yaw = pid->imu.Yaw;
            break;
        case MODE_BACKWARD:
            pid->target_speed = -pid->params.speed_max * 0.6f; // 反向60%速度
            pid->target_yaw = pid->imu.Yaw;
            break;
        case MODE_TURN_LEFT:
            pid->target_speed = 0.0f;
            pid->target_yaw = pid->imu.Yaw + 15.0f;  // 左偏15度
            break;
        case MODE_TURN_RIGHT:
            pid->target_speed = 0.0f;
            pid->target_yaw = pid->imu.Yaw - 15.0f;  // 右偏15度
            break;
        case MODE_STOP:
            pid->target_speed = 0.0f;
            pid->target_yaw = pid->imu.Yaw;
            pid->balance_output = 0;  // 停止时关闭平衡输出
            pid->speed_output = 0;
            pid->turn_output = 0;
            break;
    }

    // 6. 计算电机PWM（融合三环输出）
    pid->left_pwm = pid->balance_output + pid->speed_output - pid->turn_output;
    pid->right_pwm = pid->balance_output + pid->speed_output + pid->turn_output;
    
    // PWM限幅（保护电机和驱动模块）
    pid->left_pwm = constrain(pid->left_pwm, -200, 200);
    pid->right_pwm = constrain(pid->right_pwm, -200, 200);

    // 7. 输出到电机
    SetPWM(MOTOR_LEFT, pid->left_pwm);
    SetPWM(MOTOR_RIGHT, pid->right_pwm);
}

// 设置控制模式
void pidSetMode(PIDController* pid, ControlMode mode) {
    pid->mode = mode;
    // 切换模式时重置积分项，防止冲击
    if (mode == MODE_STOP) {
        pid->balance_integral = 0.0f;
        pid->speed_integral = 0.0f;
    }
}

// 手动设置目标速度（用于精细控制）
void pidSetTargetSpeed(PIDController* pid, float speed) {
    pid->target_speed = constrain(speed, -pid->params.speed_max, pid->params.speed_max);
}

// 手动设置转向目标（用于精细控制）
void pidSetTargetTurn(PIDController* pid, float yaw_offset) {
    pid->target_yaw = pid->imu.Yaw + yaw_offset;
}