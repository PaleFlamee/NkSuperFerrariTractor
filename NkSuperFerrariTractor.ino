// NkSuperFerrariTractor.ino
// To run on Arduino R3
// 

#include "NkSuperFerrariTractor.h"

#include "FetchData.h"
#include "LightNSound.h"

// Timers
#define LASTMILLIS(MILLIS) lastMillis ## MILLIS
#define TIMESUP(MILLIS) LASTMILLIS(MILLIS) - nowMilliis >= MILLIS

void setup() {
    Serial.begin(9600);
    Serial.println("Ciallo World!");
    Wire.begin();
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
         fetchIMUData(&imuData);
        #endif
        #ifdef DEBUG_LTM
         fetchLTMData(&ltmData);
        #endif
     }
    #endif
}