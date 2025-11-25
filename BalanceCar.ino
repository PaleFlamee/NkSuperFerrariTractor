#include "NkSuperFerrariTractor.h"
#include "Motor.h"
#include "FetchData.h"
#include "LightNSound.h"
#include "PIDControl.h"

// 创建PID控制器实例
PIDController pid;

void setup() {
    // 初始化硬件
    Serial.begin(115200);
    MotorSetup();
    lightNSoundInit();
    Wire.begin();  // 初始化I2C（传感器通信）

    // 初始化PID控制器（使用默认参数）
    pidInit(&pid, NULL);

    // 系统自检
    lightNSoundTest();
    delay(1000);
    Serial.println("System ready. Place the car upright to start.");
}

void loop() {
    // 持续更新PID控制（控制周期约5ms）
    pidUpdate(&pid);

    // 示例：通过串口指令切换模式（可替换为任务逻辑）
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        switch (cmd) {
            case 'b': pidSetMode(&pid, MODE_BALANCE); break;   // 平衡模式
            case 'f': pidSetMode(&pid, MODE_FORWARD); break;  // 前进
            case 'r': pidSetMode(&pid, MODE_BACKWARD); break; // 后退
            case 'l': pidSetMode(&pid, MODE_TURN_LEFT); break;// 左转
            case 't': pidSetMode(&pid, MODE_TURN_RIGHT); break;// 右转
            case 's': pidSetMode(&pid, MODE_STOP); break;     // 停止
            default: break;
        }
    }

    delay(5);  // 控制周期约200Hz
}