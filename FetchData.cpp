#include "NkSuperFerrariTractor.h"

#include "FetchData.h"

#ifdef IMU_SERIAL

struct IMUData imuDataSerial;


void CopeSerialData(unsigned char ucData){
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	
	ucRxBuffer[ucRxCnt++]=ucData;
	if (ucRxBuffer[0]!=0x55) 
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<11) {return;}
	else
	{
        struct SAcc{
            short a[3];
        }sacc;
        struct SGyro{
            short w[3];
        }sgyro;
        struct SAngle{
            short Angle[3];
        }sangle;

		switch(ucRxBuffer[1]){
			case 0x50:	break;
			case 0x51:	memcpy(&sacc.a,&ucRxBuffer[2],8);break;
			case 0x52:	memcpy(&sgyro.w,&ucRxBuffer[2],8);break;
			case 0x53:	memcpy(&sangle.Angle,&ucRxBuffer[2],8);break;
			case 0x54:	break;
			case 0x55:	break;
			case 0x56:	break;
			case 0x57:	break;
			case 0x58:	break;
		}

        imuDataSerial.accelX = (float)sacc.a[0] / 32768 * 16; // Unit: g
        imuDataSerial.accelY = (float)sacc.a[1] / 32768 * 16; // Unit: g
        imuDataSerial.accelZ = (float)sacc.a[2] / 32768 * 16; // Unit: g
        imuDataSerial.gyroX = (float)sgyro.w[0] / 32768 * 2000; // Unit: °/s
        imuDataSerial.gyroY = (float)sgyro.w[1] / 32768 * 2000; // Unit: °/s
        imuDataSerial.gyroZ = (float)sgyro.w[2] / 32768 * 2000; // Unit: °/s
        imuDataSerial.Row = (float)sangle.Angle[0] / 32768 * 180; // Unit: °
        imuDataSerial.Pitch = (float)sangle.Angle[1] / 32768 * 180; // Unit: °
        imuDataSerial.Yaw = (float)sangle.Angle[2] / 32768 * 180; // Unit: °

		ucRxCnt=0;
	}
}

void serialEvent(){// Hardware serial, also IMU serial
    while (ImuSerial.available()) 
    {
        CopeSerialData(ImuSerial.read()); //Call JY901 data cope function
    }
    #ifdef DEBUG_IMU
    char buf[16];
    dtostrf(imuDataSerial.accelX, 8, 4, buf);
    LogSerial.print("AX: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(imuDataSerial.accelY, 8, 4, buf);
    LogSerial.print("AY: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(imuDataSerial.accelZ, 8, 4, buf);
    LogSerial.print("AZ: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(imuDataSerial.gyroX, 8, 2, buf);
    LogSerial.print("GX: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(imuDataSerial.gyroY, 8, 2, buf);
    LogSerial.print("GY: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(imuDataSerial.gyroZ, 8, 2, buf);
    LogSerial.print("GZ: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(imuDataSerial.Row, 8, 2, buf);
    LogSerial.print("Roll: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(imuDataSerial.Pitch, 8, 2, buf);
    LogSerial.print("Pitch: ");LogSerial.print(buf);LogSerial.print(" | ");
    dtostrf(imuDataSerial.Yaw, 8, 2, buf);
    LogSerial.print("Yaw: ");LogSerial.print(buf);LogSerial.println();
    #endif
}

#else
 void fetchIMUData(struct IMUData *pData) {
    Wire.beginTransmission(IMU);
    Wire.write(IMU_ACC_X_REG);
    Wire.endTransmission(false);
    Wire.requestFrom(IMU, 6, true); // Request 6 bytes for Acceleration
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

