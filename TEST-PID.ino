#include "NkSuperFerrariTractor.h"
#include "Motor.h"
#include "FetchData.h"
#include "LightNSound.h"
#include "Encoder.h"
#include "BalanceControl.h"

// Timer1 for PID
#include <TimerOne.h>

// 调试模式配置（强制开启串口调试）
#define DEBUG_PID 1
#define BAUD_RATE 9600

// 全局数据结构
struct IMUData imuData;
struct LTMData ltmData;
struct EncoderData encoderData;
bool isBalancing = true;  // 平衡使能标志
enum OperationMode currentMode = Stop;  // 调试模式（默认停止）

// PID参数（支持串口实时修改）
float angleKp = 2.5, angleKi = 0.1, angleKd = 0.05;
float speedKp = 1.0, speedKi = 0.02, speedKd = 0.0;
float steerKp = 0.8, steerKi = 0.01, steerKd = 0.02;

// 调试参数（支持串口修改）
float targetSpeed = 0.0;  // 目标速度（r/s）：正数前进，负数后退
int targetSteer = 0;       // 目标转向（-50~50）：负数左偏，正数右偏
bool useLineTrack = false; // 是否启用循迹转向（false=手动转向，true=循迹自动转向）

void setup() {
    // 串口初始化（调试核心）
    LogSerial.begin(BAUD_RATE);
    LogSerial.println("\n==================== PID调试模式 ====================");
    LogSerial.println("指令说明：");
    LogSerial.println("  Px,Py,Pz  - 设置角度环PID（x=Kp,y=Ki,z=Kd）");
    LogSerial.println("  Sx,Sy,Sz  - 设置速度环PID（x=Kp,y=Ki,z=Kd）");
    LogSerial.println("  Tx,Ty,Tz  - 设置转向环PID（x=Kp,y=Ki,z=Kd）");
    LogSerial.println("  Vx        - 设置目标速度（x=r/s，如V2.0=前进2r/s）");
    LogSerial.println("  Dx        - 设置手动转向（x=-50~50，如D10=右偏10）");
    LogSerial.println("  L0/L1     - 关闭/开启循迹转向（L1=启用循迹，L0=手动）");
    LogSerial.println("  B0/B1     - 关闭/开启平衡功能（B1=平衡，B0=停止）");
    LogSerial.println("  R         - 重置所有参数为默认值");
    LogSerial.println("====================================================");

    // I2C总线初始化（传感器通信）
    Wire.begin();

    // 硬件模块初始化
    lightNSoundInit();  // 声光提示
    MotorSetup();       // 电机驱动
    EncoderSetup();     // 编码器
    fetchIMUData(&imuData);  // 初始化IMU数据
    fetchLTMData(&ltmData);  // 初始化循迹数据

    // PID参数初始化
    PIDInit(&anglePID, angleKp, angleKi, angleKd, 200.0);
    PIDInit(&speedPID, speedKp, speedKi, speedKd, 100.0);
    PIDInit(&steerPID, steerKp, steerKi, steerKd, 50.0);

    // Timer1初始化（10ms PID中断，核心频率）
    Timer1.initialize(10000);
    Timer1.attachInterrupt(Timer1_ISR);

    // 声光提示：调试模式启动成功
    Light(true);
    Sound(true);
    delay(500);
    Light(false);
    Sound(false);
}

void loop() {
    static unsigned long dataPrintTimer = 0;  // 数据打印定时器
    unsigned long nowMillis = millis();

    // 1. 10ms采集传感器数据（与PID中断同步）
    static unsigned long sensorTimer = 0;
    if (nowMillis - sensorTimer >= 10) {
        sensorTimer = nowMillis;
        fetchIMUData(&imuData);                // 读取IMU角度/加速度
        fetchLTMData(&ltmData);                // 读取循迹传感器
        CalculateEncoderSpeed(&encoderData, 0.01);  // 计算电机转速
    }

    // 2. 500ms打印调试数据（串口输出，便于观察）
    if (nowMillis - dataPrintTimer >= 500) {
        dataPrintTimer = nowMillis;
        printDebugData();
    }

    // 3. 处理串口指令（实时修改参数）
    if (LogSerial.available() > 0) {
        processSerialCommand();
    }
}

// Timer1中断：PID控制核心（10ms执行一次）
void Timer1_ISR() {
    if (!isBalancing) {
        // 关闭平衡：电机停转
        SetPWM(MotorLeft, 0);
        SetPWM(MotorRight, 0);
        return;
    }

    // 1. 角度环PID（维持平衡，目标角度0°）
    float angleOutput = PIDCalculate(&anglePID, 0.0, imuData.Pitch, 0.01);

    // 2. 速度环PID（控制目标速度）
    float avgSpeed = (encoderData.leftSpeed + encoderData.rightSpeed) / 2.0;
    float speedOutput = PIDCalculate(&speedPID, targetSpeed, avgSpeed, 0.01);

    // 3. 转向环PID（手动转向/循迹转向二选一）
    float steerError = 0.0;
    if (useLineTrack) {
        // 循迹模式：计算循迹误差（8路传感器）
        for (int i = 0; i < 8; i++) {
            if (ltmData.ir[i]) {
                steerError += (i - 3.5) * 10;  // 中间3.5为基准，量化误差
            }
        }
    } else {
        // 手动模式：目标转向直接作为误差（-50~50）
        steerError = targetSteer;
    }
    float steerOutput = PIDCalculate(&steerPID, 0.0, steerError, 0.01);

    // 4. 计算电机PWM并输出
    int leftPwm = angleOutput + speedOutput + steerOutput;
    int rightPwm = angleOutput + speedOutput - steerOutput;
    leftPwm = constrain(leftPwm, -255, 255);
    rightPwm = constrain(rightPwm, -255, 255);
    SetPWM(MotorLeft, leftPwm);
    SetPWM(MotorRight, rightPwm);
}

// 打印调试数据到串口
void printDebugData() {
    LogSerial.println("\n---------------- 调试数据 ----------------");
    // PID参数
    LogSerial.print("角度环PID：Kp="); LogSerial.print(angleKp);
    LogSerial.print(" Ki="); LogSerial.print(angleKi);
    LogSerial.print(" Kd="); LogSerial.println(angleKd);
    LogSerial.print("速度环PID：Kp="); LogSerial.print(speedKp);
    LogSerial.print(" Ki="); LogSerial.print(speedKi);
    LogSerial.print(" Kd="); LogSerial.println(speedKd);
    LogSerial.print("转向环PID：Kp="); LogSerial.print(steerKp);
    LogSerial.print(" Ki="); LogSerial.print(steerKi);
    LogSerial.print(" Kd="); LogSerial.println(steerKd);

    // 核心状态数据
    LogSerial.print("平衡角度："); LogSerial.print(imuData.Pitch); LogSerial.println("°");
    LogSerial.print("目标速度："); LogSerial.print(targetSpeed); LogSerial.print("r/s  实际速度："); LogSerial.println((encoderData.leftSpeed + encoderData.rightSpeed)/2.0);
    LogSerial.print("转向模式："); LogSerial.print(useLineTrack ? "循迹" : "手动");
    LogSerial.print("  转向值："); LogSerial.println(targetSteer);

    // 循迹传感器状态（8路IR：0=未检测，1=检测到黑线）
    LogSerial.print("循迹传感器：");
    for (int i = 0; i < 8; i++) {
        LogSerial.print(ltmData.ir[i]);
        if (i < 7) LogSerial.print(",");
    }
    LogSerial.println("\n----------------------------------------");
}

// 处理串口指令（修改PID参数/模式）
void processSerialCommand() {
    String cmd = LogSerial.readStringUntil('\n');  // 读取指令（以换行结束）
    cmd.trim();  // 去除空格和换行符
    LogSerial.print("收到指令："); LogSerial.println(cmd);

    // 1. 设置角度环PID（指令格式：Px,Py,Pz 如P2.5,0.1,0.05）
    if (cmd.startsWith("P")) {
        cmd.remove(0, 1);  // 去掉开头的"P"
        float kp = cmd.substring(0, cmd.indexOf(',')).toFloat();
        cmd.remove(0, cmd.indexOf(',') + 1);
        float ki = cmd.substring(0, cmd.indexOf(',')).toFloat();
        cmd.remove(0, cmd.indexOf(',') + 1);
        float kd = cmd.toFloat();
        angleKp = kp; angleKi = ki; angleKd = kd;
        PIDInit(&anglePID, kp, ki, kd, 200.0);  // 重新初始化PID
        LogSerial.println("角度环PID更新成功！");
    }

    // 2. 设置速度环PID（指令格式：Sx,Sy,Sz 如S1.0,0.02,0.0）
    else if (cmd.startsWith("S")) {
        cmd.remove(0, 1);
        float kp = cmd.substring(0, cmd.indexOf(',')).toFloat();
        cmd.remove(0, cmd.indexOf(',') + 1);
        float ki = cmd.substring(0, cmd.indexOf(',')).toFloat();
        cmd.remove(0, cmd.indexOf(',') + 1);
        float kd = cmd.toFloat();
        speedKp = kp; speedKi = ki; speedKd = kd;
        PIDInit(&speedPID, kp, ki, kd, 100.0);
        LogSerial.println("速度环PID更新成功！");
    }

    // 3. 设置转向环PID（指令格式：Tx,Ty,Tz 如T0.8,0.01,0.02）
    else if (cmd.startsWith("T")) {
        cmd.remove(0, 1);
        float kp = cmd.substring(0, cmd.indexOf(',')).toFloat();
        cmd.remove(0, cmd.indexOf(',') + 1);
        float ki = cmd.substring(0, cmd.indexOf(',')).toFloat();
        cmd.remove(0, cmd.indexOf(',') + 1);
        float kd = cmd.toFloat();
        steerKp = kp; steerKi = ki; steerKd = kd;
        PIDInit(&steerPID, kp, ki, kd, 50.0);
        LogSerial.println("转向环PID更新成功！");
    }

    // 4. 设置目标速度（指令格式：Vx 如V2.0=前进2r/s，V-1.5=后退1.5r/s）
    else if (cmd.startsWith("V")) {
        float speed = cmd.substring(1).toFloat();
        targetSpeed = constrain(speed, -3.0, 3.0);  // 速度限制在±3r/s
        LogSerial.print("目标速度更新为："); LogSerial.println(targetSpeed);
    }

    // 5. 设置手动转向（指令格式：Dx 如D10=右偏10，D-5=左偏5）
    else if (cmd.startsWith("D")) {
        int steer = cmd.substring(1).toInt();
        targetSteer = constrain(steer, -50, 50);  // 转向限制在±50
        LogSerial.print("手动转向更新为："); LogSerial.println(targetSteer);
    }

    // 6. 切换循迹/手动转向（L1=循迹，L0=手动）
    else if (cmd == "L0") {
        useLineTrack = false;
        LogSerial.println("转向模式：手动转向");
    } else if (cmd == "L1") {
        useLineTrack = true;
        LogSerial.println("转向模式：循迹转向");
    }

    // 7. 切换平衡功能（B1=开启，B0=关闭）
    else if (cmd == "B0") {
        isBalancing = false;
        LogSerial.println("平衡功能关闭，电机停转");
    } else if (cmd == "B1") {
        isBalancing = true;
        LogSerial.println("平衡功能开启");
    }

    // 8. 重置参数为默认值
    else if (cmd == "R") {
        angleKp = 2.5; angleKi = 0.1; angleKd = 0.05;
        speedKp = 1.0; speedKi = 0.02; speedKd = 0.0;
        steerKp = 0.8; steerKi = 0.01; steerKd = 0.02;
        targetSpeed = 0.0; targetSteer = 0; useLineTrack = false;
        PIDInit(&anglePID, angleKp, angleKi, angleKd, 200.0);
        PIDInit(&speedPID, speedKp, speedKi, speedKd, 100.0);
        PIDInit(&steerPID, steerKp, steerKi, steerKd, 50.0);
        LogSerial.println("所有参数已重置为默认值！");
    }

    // 无效指令
    else {
        LogSerial.println("无效指令！请参考指令说明");
    }
}