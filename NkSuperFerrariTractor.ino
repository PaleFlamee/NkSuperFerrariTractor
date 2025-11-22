// NkSuperFerrariTractor.ino
// To run on Arduino R3
// 

#include "NkSuperFerrariTractor.h"
//To be filled, PID header
#include <Wire.h>

// Debugging options
#define DEBUG
#ifdef DEBUG
  #define DEBUG_IMU
  #define DEBUG_LTM
#endif

// Pin definitions

// Timers
#define LASTMILLIS(MILLIS) lastMillis ## MILLIS
#define TIMESUP(MILLIS) LASTMILLIS(MILLIS) - nowMilliis >= MILLIS

// I2C device address definitions
#define IMU 0x50 // Inertial Measurement Unit (JY61P)
#define LTM 0x12 // IR Line Tracking Module

// I2C device registers addresses
#define IMU_ACC_X_REG 0x34 // X->Z 0x34->0x36, 16 bits per axis
#define IMU_GYR_X_REG 0x37 // X->Z 0x37->0x39, 16 bits per axis
#define LTM_IR_STATE_REG 0x30 // 8 bits, each bit for one IR sensor

void fetchLTMData(struct LTMData *pData) {
    Wire.beginTransmission(LTM);
    Wire.write(LTM_IR_STATE_REG);
    Wire.endTransmission(false);
    Wire.requestFrom(LTM, 1, true);
    uint8_t irState = Wire.read();
    for (int i = 0; i < 8; i++) {
        pData->ir[i] = !((irState >> (7 - i)) & 0x1);
        // Note: The original signal is inverted, 0 for "Line", 1 for "noLine"
        // We invert it here
    }

    #ifdef DEBUG_LTM
    Serial.print("LTM IR States: ");
    for (int i = 0; i < 8; i++) {
        Serial.print(pData->ir[i]);
        Serial.print(" ");
    }
    Serial.println();
    #endif
}

void fetchIMUData(struct IMUData *pData) {
    Wire.beginTransmission(IMU);
    Wire.write(IMU_ACC_X_REG);
    Wire.endTransmission(false);
    Wire.requestFrom(IMU, 6, true); // Request 6 bytes for Acceleration
    int16_t AcX = (Wire.read() << 8) | Wire.read();
    int16_t AcY = (Wire.read() << 8) | Wire.read();
    int16_t AcZ = (Wire.read() << 8) | Wire.read();

    Wire.beginTransmission(IMU);
    Wire.write(IMU_GYR_X_REG); 
    Wire.endTransmission(false);
    Wire.requestFrom(IMU, 6, true); // Request 6 bytes for Gyroscope
    int16_t GyX = Wire.read() ;//<< 8 | Wire.read();
    int16_t GyY = Wire.read() ;//<< 8 | Wire.read();
    int16_t GyZ = Wire.read() ;//<< 8 | Wire.read();

    pData->accelX = (float)AcX / 32768 * 16; // Unit: g
    pData->accelY = (float)AcY / 32768 * 16; // Unit: g
    pData->accelZ = (float)AcZ / 32768 * 16; // Unit: g
    pData->gyroX = GyX / 16.384; // Unit: °/s
    pData->gyroY = GyY / 16.384; // Unit: °/s
    pData->gyroZ = GyZ / 16.384; // Unit: °/s

    #ifdef DEBUG_IMU
    char buf[16];
    dtostrf(pData->accelX, 8, 4, buf);
    Serial.print("AX: ");Serial.print(buf);Serial.print(" | ");
    dtostrf(pData->accelY, 8, 4, buf);
    Serial.print("AY: ");Serial.print(buf);Serial.print(" | ");
    dtostrf(pData->accelZ, 8, 4, buf);
    Serial.print("AZ: ");Serial.print(buf);Serial.print(" | ");
    dtostrf(pData->gyroX, 8, 2, buf);
    Serial.print("GX: ");Serial.print(buf);Serial.print(" | ");
    dtostrf(pData->gyroY, 8, 2, buf);
    Serial.print("GY: ");Serial.print(buf);Serial.print(" | ");
    dtostrf(pData->gyroZ, 8, 2, buf);
    Serial.print("GZ: ");Serial.print(buf);Serial.println();
    #endif
}

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

    if(TIMESUP(500)){
        LASTMILLIS(500) = nowMilliis;
        // TODO: Add codes here
        fetchIMUData(&imuData);
    }
}