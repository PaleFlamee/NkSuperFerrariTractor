#ifndef FETCH_DATA_H
#define FETCH_DATA_H

// I2C device address definitions
#define IMU 0x50 // Inertial Measurement Unit (JY61P)
#define LTM 0x12 // IR Line Tracking Module

// I2C device registers addresses
#define IMU_ACC_X_REG 0x34 // X->Z 0x34->0x36, 16 bits per axis
#define IMU_GYR_X_REG 0x37 // X->Z 0x37->0x39, 16 bits per axis
#define LTM_IR_STATE_REG 0x30 // 8 bits, each bit for one IR sensor

extern void fetchIMUData(struct IMUData *pData);
extern void fetchLTMData(struct LTMData *pData);

#endif