#ifndef LIGHT_N_SOUND_H
#define LIGHT_N_SOUND_H

// Pin definitions
#define EXTERNAL_LED_PIN 12
#define BUZZER_PIN 13

extern void Light(bool isGlow);
extern void Sound(bool isBuzz);
extern void lightNSoundInit();
extern void lightNSoundTest();

#endif