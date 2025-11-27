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
// Timer1 for PID
#include <MsTimer1.h>
// Timer0 is used for delay, millis
// Timer2 for PWM motor control

// Timers
#define LASTMILLIS(MILLIS) lastMillis ## MILLIS
#define TIMESUP(MILLIS) nowMillis - LASTMILLIS(MILLIS) >= MILLIS

// Pin definitions for title selection
#define SWITCH_PIN_1  //上拉电阻，开关连接到GND
#define SWITCH_PIN_2  //上拉电阻，开关连接到GND


struct IMUData imuData;
struct LTMData ltmData;
//enum OperationMode currentMode = Stop;

const int numberOfSteps[4] = {2,4,5,9};
const struct Title title[4]={{{FollowInertia,Stop}},
    {{FollowInertia,Stop,FollowInertiaReverse,Stop}},
    {{FollowInertia,FollowLine,FollowInertia,FollowLine,Stop}},
    {{FollowInertia,FollowLine,FollowInertia,FollowLine,FollowInertia,FollowLine,FollowInertia,FollowLine,Stop}}};
int currentTitleIndex;
int currentOperationIndex = 0;
enum OperationMode currentOperation = Stop;

void setup() {
    LogSerial.begin(9600);
    LogSerial.println();
    LogSerial.println("-------------------------");
    LogSerial.print("Ciallo~(∠·ω< )⌒★ ");
    Wire.begin();
    lightNSoundInit();
    MotorSetup();
    MsTimer1::set(10, Timer1_ISR); // 10ms interrupt
    MsTimer1::start();
    LogSerial.println("World!");
    LogSerial.println("-------------------------");

    #ifdef DEBUG_LIGHT_SOUND
     LogSerial.println("Light and Sound Test Start:");
     lightNSoundTest();
     LogSerial.println("Light and Sound Test End.");
    #endif
    #ifdef DEBUG_MOTOR
     LogSerial.println("Motor Test Start:");
     MotorTest();
     LogSerial.println("Motor Test End.");
    #endif

    pinMode(SWITCH_PIN_1, INPUT_PULLUP);
    pinMode(SWITCH_PIN_2, INPUT_PULLUP);
    // Determine current title based on switch positions
    bool switch1State = digitalRead(SWITCH_PIN_1);
    bool switch2State = digitalRead(SWITCH_PIN_2);
    if (!switch1State && !switch2State) {
        currentTitleIndex = 0;// 第一题 
    } else if (!switch1State && switch2State) {
        currentTitleIndex = 1;// 第二题
    } else if (switch1State && !switch2State) {
        currentTitleIndex = 2;// 第三题
    } else {
        currentTitleIndex = 3;// 第四题
    }
}

void loop(){
    static unsigned long LASTMILLIS(100) = 0, LASTMILLIS(300) = 0, LASTMILLIS(200) = 0, LASTMILLIS(10) = 0;
    unsigned long nowMillis = millis();
    static bool Blink = false;
    if(TIMESUP(10)){
        LASTMILLIS(10) = nowMillis;
        fetchIMUData(&imuData);
        fetchLTMData(&ltmData);
    }
    if(TIMESUP(100)){
        LASTMILLIS(100) = nowMillis;
        switch(title[currentTitleIndex].Operations[currentOperationIndex]){
        case Stop:
            if(currentOperationIndex >= numberOfSteps[currentTitleIndex]-1){
                Blink = true;
                break; // 已经是最后一步，保持停止状态
            }
            switch(title[currentTitleIndex].Operations[currentOperationIndex+1]){
            case FollowInertia: // 停止->跟随白线
                if(!ltmData.ir[3] && !ltmData.ir[4]){// 如果中间两个传感器未检测到线
                    currentOperationIndex++;
                    currentOperation = FollowInertia;
                    Blink = true;
                }
                break;
            case FollowInertiaReverse: // 停止->跟随白线倒车
                if(ltmData.ir[3] || ltmData.ir[4]){// 如果中间两个传感器检测到线
                    currentOperationIndex++;
                    currentOperation = FollowInertiaReverse;
                    Blink = true;
                }
                break;
            }
            break;
        case FollowLine:
            switch (title[currentTitleIndex].Operations[currentOperationIndex+1]) {
            case FollowInertia: // 跟随黑线 -> 跟随白线
                if(!ltmData.ir[3] && !ltmData.ir[4]){// 如果中间两个传感器未检测到线
                    currentOperationIndex++;
                    currentOperation = FollowInertia;
                    Blink = true;
                }
                break;
            case Stop: // 跟随黑线 -> 停止
                if(!ltmData.ir[3] && !ltmData.ir[4]){// 如果中间两个传感器未检测到线
                    currentOperationIndex++;
                    currentOperation = Stop;
                    Blink = true;
                }
                break;
            }
            break;
        case FollowInertia:
            switch (title[currentTitleIndex].Operations[currentOperationIndex+1]) {
            case FollowLine: // 跟随白线 -> 跟随黑线
                if(ltmData.ir[3] || ltmData.ir[4]){// 如果中间两个传感器检测到线
                    currentOperationIndex++;
                    currentOperation = FollowLine;
                    Blink = true;
                }
                break;
            case Stop: // 跟随白线 -> 停止
                if(ltmData.ir[3] || ltmData.ir[4]){// 如果中间两个传感器检测到线
                    currentOperationIndex++;
                    currentOperation = Stop;
                    Blink = true;
                }
                break;
            }
            break;
        case FollowInertiaReverse:
            switch (title[currentTitleIndex].Operations[currentOperationIndex+1]) {
            case Stop: // FollowInertiaReverse -> Stop
                if(ltmData.ir[3] || ltmData.ir[4]){// 如果中间两个传感器检测到线
                    currentOperationIndex++;
                    currentOperation = Stop;
                    Blink = true;
                }
                break;
            }
            break;
        }
    }
    #ifdef DEBUG
     if(TIMESUP(300)){// For testing purposes
        LASTMILLIS(300) = nowMillis;
        #ifdef DEBUG_IMU
         fetchIMUData(&imuData);
        #endif
        #ifdef DEBUG_LTM
         //fetchLTMData(&ltmData);
        #endif
        LogSerial.println();
     }
    #endif
    if(TIMESUP(200)){// 当Blink为true时，启动蜂鸣器和LED灯，下次定时(200ms后)关闭以产生单次脉冲
        LASTMILLIS(200) = nowMillis;
        if(Blink){
            Light(true);
            Sound(true);
            Blink = false;
        }else{
            Light(false);
            Sound(false);
        }
    }
}

// Timer2 Interrupt Service Routine
void Timer1_ISR() {
    // 全局变量currentOperation决定当前的PID操作模式
}

