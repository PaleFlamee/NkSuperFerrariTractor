#include "NkSuperFerrariTractor.h"
#include "Motor.h"
#include "FetchData.h"
#include "LightNSound.h"
#include "BalanceControl.h"  // 新增的直立控制头文件

// 全局变量
struct IMUData imuData;       // IMU数据结构体
struct LTMData ltmData;       // 循迹模块数据结构体
bool isBalancing = false;     // 直立状态标志

void setup() {
  // 初始化硬件
  LogSerial.begin(9600);    // 日志串口初始化
  MotorSetup();               // 电机驱动初始化
  lightNSoundInit();          // 灯光蜂鸣器初始化
  #ifndef IMU_SERIAL
    Wire.begin();             // I2C初始化（用于IMU和循迹模块）
  #endif

  // 系统自检
  lightNSoundTest();          // 灯光蜂鸣器测试
  //MotorTest();                // 电机测试（短暂运行后停止）
  
  // 等待IMU初始化完成
  delay(1000);
  LogSerial.println("System initialized. Ready to balance.");
  
  // 启动直立控制
  isBalancing = true;
  Light(true);  // 点亮LED表示系统启动
}

void loop() {
  if (isBalancing) {
    // 获取传感器数据
    #ifdef IMU_SERIAL
      // 串口模式下IMU数据通过serialEvent自动更新
    #else
      fetchIMUData(&imuData);  // I2C模式下主动获取IMU数据
    #endif
    fetchLTMData(&ltmData);    // 获取循迹传感器数据（可选，用于扩展功能）

    // 执行直立控制
    balanceControl(&imuData);  // 传入IMU数据进行直立控制

    // 角度超限保护（防止翻车后持续输出）
    float currentAngle = getCurrentAngle(&imuData);
    if (abs(currentAngle) > ANGLE_LIMIT) {
      stopMotors();            // 超过安全角度则停止电机
      isBalancing = false;
      Sound(true);             // 蜂鸣器报警
      delay(1000);
      Sound(false);
      LogSerial.println("Angle out of limit. Balance stopped.");
    }
  } else {
    // 等待重新启动（可通过串口指令扩展）
    delay(100);
  }
}