/*
  ESP32 Motor Driver Template
  Supports DC motors with PWM speed control and direction
  Author: Sam Xiao (template)
*/

#include <Arduino.h>

// --- MOTOR PINS ---
// Motor A
const int motorA_in1 = 21; // direction pin 1
const int motorA_in2 = 36; // direction pin 2
const int motorA_pwm = 37; // PWM pin (speed)

// Motor B
const int motorB_in1 = 14;
const int motorB_in2 = 15;
const int motorB_pwm = 16;

// --- PWM SETTINGS ---
const int pwmFreq = 5000;      // 5 kHz
const int pwmResolution = 8;   // 8-bit (0-255)
const int motorChannelA = 0;   // PWM channel for motor A
const int motorChannelB = 1;   // PWM channel for motor B


// --- MOTOR FUNCTIONS ---
void setupMotorPins() {
  // Motor A pins
  pinMode(motorA_in1, OUTPUT);
  pinMode(motorA_in2, OUTPUT);
  ledcSetup(motorChannelA, pwmFreq, pwmResolution);
  ledcAttachPin(motorA_pwm, motorChannelA);

  // Motor B pins
  pinMode(motorB_in1, OUTPUT);
  pinMode(motorB_in2, OUTPUT);
  ledcSetup(motorChannelB, pwmFreq, pwmResolution);
  ledcAttachPin(motorB_pwm, motorChannelB);
}

/**
 * Set motor speed and direction
 * @param motorChannel PWM channel
 * @param in1Dir IN1 pin
 * @param in2Dir IN2 pin
 * @param speed -127..127 (- = reverse)
 */
void setMotor(int motorChannel, int in1Dir, int in2Dir, int speed) {
  bool forward = speed >= 0;
  int pwmVal = abs(speed);
  pwmVal = constrain(pwmVal, 0, 255);

  digitalWrite(in1Dir, forward ? HIGH : LOW);
  digitalWrite(in2Dir, forward ? LOW : HIGH);
  ledcWrite(motorChannel, pwmVal);
}

// Optional helper functions
void stopMotor(int motorChannel, int in1Dir, int in2Dir) {
  digitalWrite(in1Dir, LOW);
  digitalWrite(in2Dir, LOW);
  ledcWrite(motorChannel, 0);
}

void setup() {
  Serial.begin(115200);
  setupMotorPins();
  Serial.println("ESP32 Motor Driver Initialized");
}

void loop() {
  // Example: ramp Motor A forward
  for (int i = 0; i <= 255; i += 5) {
    setMotor(motorChannelA, motorA_in1, motorA_in2, i);
    delay(50);
  }

  delay(500);

  // Reverse Motor A
  for (int i = 0; i <= 255; i += 5) {
    setMotor(motorChannelA, motorA_in1, motorA_in2, -i);
    delay(50);
  }

  delay(500);

  // Stop motors
  stopMotor(motorChannelA, motorA_in1, motorA_in2);
  stopMotor(motorChannelB, motorB_in1, motorB_in2);
  delay(1000);
}

void round2() {
    setMotor(motorChannelA, motorA_in1, motorA_in2, 50);
    setMotor(motorChannelA, motorA_in1, motorA_in2, 50);

    delay(4000);  
    setMotor(motorChannelA, motorA_in1, motorA_in2, -50);
    setMotor(motorChannelA, motorA_in1, motorA_in2, -50);

    delay(4000);  
}

void round3() {

    setMotor(motorChannelA, motorA_in1, motorA_in2, 80);
    setMotor(motorChannelA, motorA_in1, motorA_in2, 80);

    delay(2000);  
    setMotor(motorChannelA, motorA_in1, motorA_in2, 40);
    setMotor(motorChannelA, motorA_in1, motorA_in2, 40);
    delay(2000);  

    setMotor(motorChannelA, motorA_in1, motorA_in2, -80);
    setMotor(motorChannelA, motorA_in1, motorA_in2, -80);

    delay(2000);  

    setMotor(motorChannelA, motorA_in1, motorA_in2, -40);
    setMotor(motorChannelA, motorA_in1, motorA_in2, -40);

    delay(2000);  
}


void round3() {

}
