// NkSuperFerrariTractor.ino
// To run on Arduino R3
// 

#include "NkSuperFerrariTractor.h"

#ifdef IMU_SERIAL
 SoftwareSerial Softerial(2,6); // RX, TX
#endif

#include "FetchData.h"
#include "LightNSound.h"
#include "Motor.h"

// Timers
#define LASTMILLIS(MILLIS) lastMillis ## MILLIS
#define TIMESUP(MILLIS) LASTMILLIS(MILLIS) - nowMilliis >= MILLIS

void setup() {
    LogSerial.begin(9600);
    LogSerial.println();
    LogSerial.println("-------------------------");
    LogSerial.print("Ciallo~(∠·ω< )⌒★ ");
    #ifdef IMU_SERIAL
     ImuSerial.begin(9600);
    #endif
    Wire.begin();
    lightNSoundInit();
    MotorSetup();
    LogSerial.println("World!");
    LogSerial.println("-------------------------");
    delay(500);

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
    delay(500);
    Light(true);
    Sound(true);
    delay(300);
    Light(false);
    Sound(false);
}

void loop(){
    static unsigned long LASTMILLIS(100) = 0, LASTMILLIS(500) = 0;
    unsigned long nowMilliis = millis();

    struct IMUData imuData;
    struct LTMData ltmData;

    if(TIMESUP(100)){
        LASTMILLIS(100) = nowMilliis;
        // TODO: Add codes here
    }
    #ifdef DEBUG
     if(TIMESUP(500)){// For testing purposes
        LASTMILLIS(500) = nowMilliis;
        #ifdef DEBUG_IMU
         #ifndef IMU_SERIAL
          fetchIMUData(&imuData);
         #endif
        #endif
        #ifdef DEBUG_LTM
         fetchLTMData(&ltmData);
        #endif
        LogSerial.println();
     }
    #endif
}