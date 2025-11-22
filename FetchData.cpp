#include "NkSuperFerrariTractor.h"

#include "FetchData.h"

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
    Serial.println("Fetching IMU Data...");
    Wire.beginTransmission(IMU);
    Serial.println("Sent beginTransmission");
    Wire.write(IMU_ACC_X_REG);
    Serial.println("Requested from ACC register");
    Wire.endTransmission(true);
    Serial.println("Ended transmission");
    Wire.requestFrom(IMU, 6, true); // Request 6 bytes for Acceleration
    Serial.println("Requested 6 bytes for ACC");
    // int16_t AcX = (Wire.read() << 8) | Wire.read();
    // int16_t AcY = (Wire.read() << 8) | Wire.read();
    // int16_t AcZ = (Wire.read() << 8) | Wire.read();
    // Don't know why the code above does not work properly, change to the code below
    int16_t Ac[3];
    char *pAc = (char *)Ac;
    for (int i = 0; i < 6; i++) {
        *(pAc + i) = Wire.read();
    }
    int16_t AcX = Ac[0];
    int16_t AcY = Ac[1];
    int16_t AcZ = Ac[2];

    Wire.beginTransmission(IMU);
    Wire.write(IMU_GYR_X_REG); 
    Wire.endTransmission(false);
    Wire.requestFrom(IMU, 6, true); // Request 6 bytes for Gyroscope
    // int16_t GyX = Wire.read() << 8 | Wire.read();
    // int16_t GyY = Wire.read() << 8 | Wire.read();
    // int16_t GyZ = Wire.read() << 8 | Wire.read();
    // Also...
    int16_t Gy[3];
    char *pGy = (char *)Gy;
    for (int i = 0; i < 6; i++) {
        *(pGy + i) = Wire.read();
    }
    int16_t GyX = Gy[0];
    int16_t GyY = Gy[1];
    int16_t GyZ = Gy[2];

    pData->accelX = (float)AcX / 32768 * 16; // Unit: g
    pData->accelY = (float)AcY / 32768 * 16; // Unit: g
    pData->accelZ = (float)AcZ / 32768 * 16; // Unit: g
    pData->gyroX = (float)GyX / 32768 * 2000; // Unit: °/s
    pData->gyroY = (float)GyY / 32768 * 2000; // Unit: °/s
    pData->gyroZ = (float)GyZ / 32768 * 2000; // Unit: °/s

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
