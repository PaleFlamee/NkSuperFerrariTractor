#ifndef NK_SUPER_FERRARI_TRACTOR_H
#define NK_SUPER_FERRARI_TRACTOR_H

// Debugging options
#define DEBUG /*ADJUSTABLE*/
#ifdef DEBUG
  #define DEBUG_IMU /*ADJUSTABLE*/
  #define DEBUG_LTM /*ADJUSTABLE*/
  //#define DEBUG_MOTOR /*ADJUSTABLE*/
  //#define DEBUG_LIGHT_SOUND /*ADJUSTABLE*/
#endif

// When using PLAN A, that is, using Serial RX to receive IMU data,
// at the same time, Serial TX cannot be used for debugging output.
// I am a god damn genius.
#define LogSerial Serial // TX only
//#define IMUSerial Serial // RX only /*ADJUSTABLE*/
// BTW, the PLAN B is to change into a brand new IMU with I2C interface.



#include <Arduino.h>
#include <hardwareSerial.h>
#include <Wire.h>


// IMU data packet
struct IMUData {
    float accelX;   // Unit: g
    float accelY;   // Unit: g
    float accelZ;   // Unit: g
    float gyroX;    // Unit: degrees/sec
    float gyroY;    // Unit: degrees/sec
    float gyroZ;    // Unit: degrees/sec
    float Row;      // Unit: degrees
    float Pitch;    // Unit: degrees
    float Yaw;      // Unit: degrees
};

// LTM data packet
struct LTMData {
    // IR sensor states, [0]->[7] ~ x1 -> x8 ~ L -> R
    // isGlow       true    false
    // isLine       true    false
    // boolValue    true    false *
    bool ir[8]; 
};

enum OperationMode {
    Stop,
    FollowLine,
    FollowInertia,
    FollowInertiaReverse
};

struct LocationData {
    float x; // Unit: m
    float y; // Unit: m
};

struct Title{
    enum OperationMode Operations[9];
};

extern enum OperationMode currentOperation;
extern int initialYaw;

#endif