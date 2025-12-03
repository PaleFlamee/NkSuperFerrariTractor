#include "NkSuperFerrariTractor.h"

#include "FetchData.h"
#ifndef IMUSerial
#ifdef JY61P
void fetchIMUData(struct IMUData *pData) {
    Wire.beginTransmission(IMU);
    Wire.write(IMU_ACC_X_REG);
    Wire.endTransmission(false);
    Wire.requestFrom(IMU, 6, true); // Request 6 bytes for Acceleration
    // int16_t AcX = (Wire.read() << 8) | Wire.read();
    // int16_t AcY = (Wire.read() << 8) | Wire.read();
    // int16_t AcZ = (Wire.read() << 8) | Wire.read();
    // Don't know why the code above does not work properly, change to the code below
    // Chat-GPT, could you please tell me why? (Use Enter plz)
	// The reason the original code did not work properly is likely due to the order of byte reading.
	// In the original code, the high byte and low byte were read sequentially without storing them first, which can lead to incorrect values if the reading order is not maintained properly.
	// By using an array and a pointer, we ensure that the bytes are stored in the correct order before combining them into a single int16_t value.
	// This method reduces the risk of misreading the byte order.
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

    Wire.beginTransmission(IMU);
    Wire.write(IMU_ROLL_REG); 
    Wire.endTransmission(false);
    Wire.requestFrom(IMU, 6, true); // Request 6 bytes for Roll, Pitch, Yaw
    int16_t RpY[3];
    char *pRpY = (char *)RpY;
    for (int i = 0; i < 6; i++) {
        *(pRpY + i) = Wire.read();
    }
    int16_t Roll = RpY[0];
    int16_t Pitch = RpY[1];
    int16_t Yaw = RpY[2];

    pData->accelX = (float)AcX / 32768 * 16; // Unit: g
    pData->accelY = (float)AcY / 32768 * 16; // Unit: g
    pData->accelZ = (float)AcZ / 32768 * 16; // Unit: g
    pData->gyroX = (float)GyX / 32768 * 2000; // Unit: °/s
    pData->gyroY = (float)GyY / 32768 * 2000; // Unit: °/s
    pData->gyroZ = (float)GyZ / 32768 * 2000; // Unit: °/s
    pData->Row = (float)Roll / 32768 * 180; // Unit: °
    pData->Pitch = (float)Pitch / 32768 * 180; // Unit: °
    pData->Yaw = (float)Yaw / 32768 * 180; // Unit: °

    #ifdef DEBUG_IMU
    char buf[16];
    dtostrf(pData->accelX, 8, 4, buf);
    LogSerial.print("AX: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->accelY, 8, 4, buf);
    LogSerial.print("AY: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->accelZ, 8, 4, buf);
    LogSerial.print("AZ: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->gyroX, 8, 2, buf);
    LogSerial.print("GX: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->gyroY, 8, 2, buf);
    LogSerial.print("GY: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->gyroZ, 8, 2, buf);
    LogSerial.print("GZ: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->Row, 7, 2, buf);
    LogSerial.print("Roll: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->Pitch, 7, 2, buf);
    LogSerial.print("Pitch: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->Yaw, 7, 2, buf);
    LogSerial.print("Yaw: ");LogSerial.print(buf);LogSerial.print(" | ");
    #endif
}
#endif
#ifdef MPU6050
void fetchIMUData(struct IMUData *pData) {
}
#endif
#ifdef NEWIMU
void fetchIMUData(struct IMUData *pData) {
    Wire.beginTransmission(0x23);
    Wire.write(0x04); // Starting register for Accel Readings
    Wire.endTransmission(false);
    Wire.requestFrom(0x23, 6, true); // Request 6 bytes for Acceleration
    int16_t Raw[3];
    uint8_t low;
    uint8_t high;
    for (int i = 0; i < 3; i++) {
        low = Wire.read();
        high = Wire.read();
        Raw[i] = (int16_t)((high << 8) | low);
    }
    pData->accelX = (float)Raw[0] * 16.0 / 32767.0; // Unit: g
    pData->accelY = (float)Raw[1] * 16.0 / 32767.0; // Unit: g
    pData->accelZ = (float)Raw[2] * 16.0 / 32767.0; // Unit: g
    Wire.beginTransmission(0x23);
    Wire.write(0x0A); // Starting register for Gyro Readings
    Wire.endTransmission(false);
    Wire.requestFrom(0x23, 3, true); // Request 6 bytes for Gyroscope
    for (int i = 0; i < 3; i++) {
        low = Wire.read();
        high = Wire.read();
        Raw[i] = (int16_t)((high << 8) | low);
    }
    pData->gyroX = (float)Raw[0] * 2000.0 / 32767.0; // Unit: °/s
    pData->gyroY = (float)Raw[1] * 2000.0 / 32767.0; // Unit: °/s
    pData->gyroZ = (float)Raw[2] * 2000.0 / 32767.0; // Unit: °/s
    Wire.beginTransmission(0x23);
    Wire.write(0x26); // Starting register for Roll, Pitch, Yaw Read
    Wire.endTransmission(false);
    Wire.requestFrom(0x23, 12, true); // Request 12 bytes for Roll, Pitch, Yaw
    union {
        float value;
        uint8_t bytes[4];
    } eulerRaw[3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            eulerRaw[i].bytes[j] = Wire.read();
        }
        pData->Row = eulerRaw[0].value * 180.0 / 3.14159;   // Unit: °
        pData->Pitch = eulerRaw[1].value * 180.0 / 3.14159; // Unit: °
        pData->Yaw = eulerRaw[2].value * 180.0 / 3.14159;   // Unit: °
    }
    #ifdef DEBUG_IMU
    char buf[16];
    dtostrf(pData->accelX, 8, 4, buf);
    LogSerial.print("AX: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->accelY, 8, 4, buf);
    LogSerial.print("AY: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->accelZ, 8, 4, buf);
    LogSerial.print("AZ: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->gyroX, 8, 2, buf);
    LogSerial.print("GX: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->gyroY, 8, 2, buf);
    LogSerial.print("GY: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->gyroZ, 8, 2, buf);
    LogSerial.print("GZ: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->Row, 7, 2, buf);
    LogSerial.print("Roll: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->Pitch, 7, 2, buf);
    LogSerial.print("Pitch: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(pData->Yaw, 7, 2, buf);
    LogSerial.print("Yaw: ");LogSerial.print(buf);LogSerial.print(" | ");
    #endif
}
#endif
#else

CJY901 JY901;
void CJY901 ::CopeSerialData(unsigned char ucData){
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	
	ucRxBuffer[ucRxCnt++]=ucData;
	if (ucRxBuffer[0]!=0x55) {
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<11) {return;}
	else {
		switch(ucRxBuffer[1]){
			case 0x51:	memcpy(&stcAcc,&ucRxBuffer[2],8);break;
			case 0x52:	memcpy(&stcGyro,&ucRxBuffer[2],8);break;
			case 0x53:	memcpy(&stcAngle,&ucRxBuffer[2],8);break;
			default:	break;
		}
		ucRxCnt=0;
	}
}
void serialEvent() {
    while (IMUSerial.available()) {
        JY901.CopeSerialData(IMUSerial.read()); //Call JY901 data cope function
        LogSerial.print("Acc:");LogSerial.print((float)JY901.stcAcc.a[0]/32768*16);LogSerial.print(" ");LogSerial.print((float)JY901.stcAcc.a[1]/32768*16);LogSerial.print(" ");LogSerial.println((float)JY901.stcAcc.a[2]/32768*16);
  
        LogSerial.print("Gyro:");LogSerial.print((float)JY901.stcGyro.w[0]/32768*2000);LogSerial.print(" ");LogSerial.print((float)JY901.stcGyro.w[1]/32768*2000);LogSerial.print(" ");LogSerial.println((float)JY901.stcGyro.w[2]/32768*2000);
  
        LogSerial.print("Angle:");LogSerial.print((float)JY901.stcAngle.Angle[0]/32768*180);LogSerial.print(" ");LogSerial.print((float)JY901.stcAngle.Angle[1]/32768*180);LogSerial.print(" ");LogSerial.println((float)JY901.stcAngle.Angle[2]/32768*180);
    }
}


void fetchIMUData(struct IMUData *pData) {
    // Just a placeholder when using Serial IMU
    // Because IMUData is fetched in Serial Interrupt Handler function
}
#endif



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
    LogSerial.print("LTM IR States: ");
    for (int i = 0; i < 8; i++) {
        if(pData->ir[i]==true) {
            LogSerial.print("■");
        } else {
            LogSerial.print("_");
        }
        LogSerial.print(" ");
    }
    #endif
}

