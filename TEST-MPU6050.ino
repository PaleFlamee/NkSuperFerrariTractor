// //To run on Arduino R3
// //To fetch data from MPU6050 sensor, via I2C protocol


// #include <Wire.h>

// void setup() {
//   Serial.begin(9600);
//   Serial.println("Ciallo World!");
//   Wire.begin();
//   Wire.beginTransmission(0x68); //MPU6050 I2C address is 0x68
//   Wire.write(0x6B); //PWR_MGMT_1 register
//   Wire.write(0);    //set to zero (wakes up the MPU-6050)
//   Wire.endTransmission(true);
//   Serial.println("MPU6050 is ready");
// }
// void loop() {
//   Wire.beginTransmission(0x68);
//   Wire.write(0x3B); //starting with register 0x3B (ACCEL_XOUT_H)
//   Wire.endTransmission(false);
//   Wire.requestFrom(0x68, 14, true); //request a total of 14 registers
//   int16_t AcX = Wire.read() << 8 | Wire.read(); //0x3B (ACCEL_XOUT_H) & 0x3C (  ACCEL_XOUT_L)
//   int16_t AcY = Wire.read() << 8 | Wire.read(); //0x3D (ACCEL_YOUT_H) & 0x3E (  ACCEL_YOUT_L)
//   int16_t AcZ = Wire.read() << 8 | Wire.read(); //0x3F (ACCEL_ZOUT_H) & 0x40 (  ACCEL_ZOUT_L)
//   int16_t Tmp = Wire.read() << 8 | Wire.read(); //0x41 (TEMP_OUT_H) & 0x42 (  TEMP_OUT_L)
//   int16_t GyX = Wire.read() << 8 | Wire.read(); //0x43 (GYRO_XOUT_H) & 0x44 (  GYRO_XOUT_L)
//   int16_t GyY = Wire.read() << 8 | Wire.read(); //0x45 (GYRO_YOUT_H) & 0x46 (  GYRO_YOUT_L)
//   int16_t GyZ = Wire.read() << 8 | Wire.read(); //0x47 (GYRO_ZOUT_H) & 0x48 (  GYRO_ZOUT_L) 
  
//   char buf[16];

//   float temperature = Tmp / 340.00 + 36.53;// Unit: °C

//   double accelX = AcX / 16384.00; // Unit: g
//   double accelY = AcY / 16384.00; // Unit: g
//   double accelZ = AcZ / 16384.00; // Unit: g

//   double gyroX = GyX / 131.00; // Unit: °/s
//   double gyroY = GyY / 131.00; // Unit: °/s
//   double gyroZ = GyZ / 131.00; // Unit: °/s

//   dtostrf(accelX, 7, 4, buf);
//   Serial.print("AX:");Serial.print(buf);Serial.print(" | ");
//   dtostrf(accelY, 7, 4, buf);
//   Serial.print("AY:");Serial.print(buf);Serial.print(" | ");
//   dtostrf(accelZ, 7, 4, buf);
//   Serial.print("AZ:");Serial.print(buf);Serial.print(" | ");
//   dtostrf(temperature, 7, 2, buf);
//   Serial.print("Tmp:");Serial.print(buf);Serial.print(" | ");
//   dtostrf(gyroX, 7, 2, buf);
//   Serial.print("GX:");Serial.print(buf);Serial.print(" | ");
//   dtostrf(gyroY, 7, 2, buf);
//   Serial.print("GY:");Serial.print(buf);Serial.print(" | ");
//   dtostrf(gyroZ, 7, 2, buf);
//   Serial.print("GZ:");Serial.println(buf);
//   //Rewrtite the outputs
//   //char buffer[20];
//   //sprintf(buffer,"AX:%6d | AY:%6d | AZ:%6d | Tmp:%s | GX:%6d | GY:%6d | GZ:%6d",AcX,AcY,AcZ,tmpBuf,GyX,GyY,GyZ);
//   //Serial.println(buffer);
//   //why the %.2f does not work in the sprintf above???
  
//   delay(100);
// }