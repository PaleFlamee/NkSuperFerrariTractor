#ifndef FETCH_DATA_H
#define FETCH_DATA_H


//#define MPU6050
//#define JY61P
#define NEWIMU

// I2C device address definitions
#define IMU 0x50 // Inertial Measurement Unit (JY61P)
#define LTM 0x12 // IR Line Tracking Module

// I2C device registers addresses
#define IMU_ACC_X_REG 0x34 // X->Z 0x34->0x36, 16 bits per axis
#define IMU_GYR_X_REG 0x37 // X->Z 0x37->0x39, 16 bits per axis
#define IMU_ROLL_REG  0x3D // Roll->Pitch->Yaw 0x3D->0x3E, 16 bits
#define LTM_IR_STATE_REG 0x30 // 8 bits, each bit for one IR sensor

#ifdef IMUSerial
 struct SAcc{
	short a[3];
	short T;
};
struct SGyro{
	short w[3];
	short T;
};
struct SAngle{
	short Angle[3];
	short T;
};
class CJY901 {
  public: 
	struct SAcc 		stcAcc;
	struct SGyro 		stcGyro;
	struct SAngle 	    stcAngle;
	
    void CopeSerialData(unsigned char ucData);
	
  private: 
};
extern CJY901 JY901;
extern void serialEvent();
#endif

extern void fetchIMUData(struct IMUData *pData);
extern void fetchLTMData(struct LTMData *pData);

#endif