#include "NkSuperFerrariTractor.h"

#include "LightNSound.h"

void Light(bool isGlow) {
    if (isGlow) {
        digitalWrite(EXTERNAL_LED_PIN, HIGH); // Turn on the LED
    } else {
        digitalWrite(EXTERNAL_LED_PIN, LOW); // Turn off the LED
    }
}

void Sound(bool isBuzz) {
    if (isBuzz) {
        digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer
    } else {
        digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer
    }
}

void lightNSoundInit() {
    pinMode(EXTERNAL_LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    // Initialize both to off state
    Light(false);
    Sound(false);
}

void lightNSoundTest() {
    // Test Sound
    Sound(true);
    delay(500);
    Sound(false);
    delay(500);
    // Test Light
    Light(true);
    delay(500);
    Light(false);
    delay(500);
    // Test Sound
    Sound(true);
    delay(500);
    Sound(false);
    delay(500);
    // Test Light
    Light(true);
    delay(500);
    Light(false);
    delay(500);
}
