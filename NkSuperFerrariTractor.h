#ifndef NK_SUPER_FERRARI_TRACTOR_H
#define NK_SUPER_FERRARI_TRACTOR_H

// IMU data packet
struct IMUData {
    float accelX;   // Unit: g
    float accelY;   // Unit: g
    float accelZ;   // Unit: g
    float gyroX;    // Unit: degrees/sec
    float gyroY;    // Unit: degrees/sec
    float gyroZ;    // Unit: degrees/sec
};

// LTM data packet
struct LTMData {
    // IR sensor states, [0]->[7] ~ x1 -> x8 ~ L -> R
    // isGlow       true    false
    // isLine       true    false
    // boolValue    true    false *
    bool ir[8]; 
};

// Pin definitions
#define EXTERNAL_LED_PIN 
#define BUZZER_PIN








#endif