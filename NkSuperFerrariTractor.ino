#include "NkSuperFerrariTractor.h"
#include "Motor.h"
#include "FetchData.h"
#include "LightNSound.h"
#include "BalanceControl.h"  // 新增的直立控制头文件


// Timer1 for PID
#include <TimerOne.h>
// Timer0 is used for delay, millis
// Timer2 for PWM motor control

// Timers
#define LASTMILLIS(MILLIS) lastMillis ## MILLIS
#define TIMESUP(MILLIS) nowMillis - LASTMILLIS(MILLIS) >= MILLIS

// Pin definitions for title selection
#define SWITCH_PIN_1  A0//上拉电阻，开关连接到GND
#define SWITCH_PIN_2  A5//上拉电阻，开关连接到GND


struct IMUData imuData;
struct LTMData ltmData;
bool isBalancing = true;     // 直立状态标志
//enum OperationMode currentMode = Stop;

const int numberOfSteps[4] = {2,4,5,9};
const struct Title title[4]={{{FollowInertia,Stop}},
    {{FollowInertia,Stop,FollowInertiaReverse,Stop}},
    {{FollowInertia,FollowLine,FollowInertia,FollowLine,Stop}},
    {{FollowInertia,FollowLine,FollowInertia,FollowLine,FollowInertia,FollowLine,FollowInertia,FollowLine,Stop}}};
int currentTitleIndex;
int currentOperationIndex;
enum OperationMode currentOperation = Stop;

void setup() {
    LogSerial.begin(9600);
    LogSerial.println();
    LogSerial.println("-------------------------");
    LogSerial.print("Ciallo~(∠·ω< )⌒★ ");
    Wire.begin();
    lightNSoundInit();
    MotorSetup();
    // Timer1, 微秒级别的定时器，开心吗
    class TimerOne Timer1;
    Timer1.initialize(10000);// 10000us = 10ms
    Timer1.attachInterrupt(Timer1_ISR);
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
    // bool switch1State = digitalRead(SWITCH_PIN_1);
    // bool switch2State = digitalRead(SWITCH_PIN_2);
    // if (!switch1State && !switch2State) {
    //     currentTitleIndex = 0;// 第一题 
    // } else if (!switch1State && switch2State) {
    //     currentTitleIndex = 1;// 第二题
    // } else if (switch1State && !switch2State) {
    //     currentTitleIndex = 2;// 第三题
    // } else {
    //     currentTitleIndex = 3;// 第四题
    // }
    currentTitleIndex = 3;// 暂时写死为第四题，方便调试
    currentOperationIndex = 0;
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
         fetchLTMData(&ltmData);
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

//Timer1 Interrupt Service Routine
void Timer1_ISR() {
//     // 全局变量currentOperation决定当前的PID操作模式
    
//   if (isBalancing) {

//     // 执行直立控制
//     balanceControl(&imuData);  // 传入IMU数据进行直立控制

//     // 角度超限保护（防止翻车后持续输出）
//     float currentAngle = getCurrentAngle(&imuData);
//     if (abs(currentAngle) > ANGLE_LIMIT) {
//       stopMotors();            // 超过安全角度则停止电机
//       isBalancing = false;
//       Sound(true);             // 蜂鸣器报警
//       delay(1000);  //////////////////////////////////////////////////////////不要用delay啊啊啊啊啊啊啊啊啊啊
//       Sound(false);
//       LogSerial.println("Angle out of limit. Balance stopped.");
//     }
//   } else {
//     // 等待重新启动（可通过串口指令扩展）
//     delay(100);////////////////////////////////////////////////////////////////////////////////////////////
//   }
}

