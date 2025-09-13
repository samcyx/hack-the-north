/*
  ESP32 Motor Driver Template
  Supports DC motors with PWM speed control and direction
  Author: Sam Xiao (template)
*/
#include <PingPongBallColourClassifier_inferencing.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/edgeimpulse/fomo.h>

using eloq::camera;
using eloq::ei::fomo;
#include <Arduino.h>

// --- MOTOR PINS ---
// Motor A
const int motorA_in1 = 35;  // direction pin 1
const int motorA_in2 = 15;  // direction pin 2
const int motorA_pwm = 16;  // PWM pin (speed)

// Motor B
const int motorB_in1 = 22;
const int motorB_in2 = 23;
const int motorB_pwm = 14;

// --- PWM SETTINGS ---
const int pwmFreq = 5000;     // 5 kHz
const int pwmResolution = 8;  // 8-bit (0-255)
const int motorChannelA = 0;  // PWM channel for motor A
const int motorChannelB = 1;  // PWM channel for motor B

int numPink = 0;
int numWhite = 0;
int pinkWins = 0;
int whiteWins = 0;
int round = 1;
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
  randomSeed(analogRead(0));
  delay(3000);
  Serial.println("__EDGE IMPULSE FOMO (NO-PSRAM)__");

  // camera settings
  // replace with your own model!
  camera.pinout.freenove_s3();
  camera.brownout.disable();
  // NON-PSRAM FOMO only works on 96x96 (yolo) RGB565 images
  camera.resolution.yolo();
  camera.pixformat.rgb565();

  // init camera
  while (!camera.begin().isOk())
    Serial.println(camera.exception.toString());

  Serial.println("Camera OK");
  Serial.println("Put object in front of camera");
}

void checkRound() {
  if (fomo.count() >= 1) {
    fomo.forEach([](int i, bbox_t bbox) {
      if (bbox.label == "Pink") {
        numPink++;
      } else if (bbox.label == "White") {
        numWhite++;
      }
    });
  }
  if (numPink >= 2) {
    pinkWins++;
    round++;
  } else if (numWhite >= 2) {
    whiteWins++;
    round++;
  }
  numWhite = 0;
  numPink = 0;

}


void loop() {
  if (!camera.capture().isOk()) {
    Serial.println(camera.exception.toString());
    return;
  }

  // run FOMO
  if (!fomo.run().isOk()) {
    Serial.println(fomo.exception.toString());
    return;
  }

  // how many objects were found?
  Serial.printf(
    "Found %d object(s) in %dms\n",
    fomo.count(),
    fomo.benchmark.millis());

  // if no object is detected, return


  // if you expect to find many objects, use fomo.forEach
  if (fomo.count() > 1) {
    fomo.forEach([](int i, bbox_t bbox) {
      Serial.printf(
        "#%d) Found %s at (x = %d, y = %d) (size %d x %d). "
        "Proba is %.2f\n",
        i + 1,
        bbox.label,
        bbox.x,
        bbox.y,
        bbox.width,
        bbox.height,
        bbox.proba);
    });
  }

  Serial.printf("Round %d | Pink: %d | White: %d\n", round, pinkWins, whiteWins);


  switch (round) {
    case 1:
      checkRound();
      break;
    case 2:
      round2();
      checkRound();
      break;
    case 3:
      round3();
      checkRound();
      break;
    case 4:
      round4();
      checkRound();
      break;
  }
}

void round2() {
  setMotor(motorChannelA, motorA_in1, motorA_in2, 50);
  setMotor(motorChannelB, motorB_in1, motorB_in2, 50);

  delay(4000);
  setMotor(motorChannelA, motorA_in1, motorA_in2, -50);
  setMotor(motorChannelB, motorB_in1, motorB_in2, -50);

  delay(4000);
}

void round3() {

  setMotor(motorChannelA, motorA_in1, motorA_in2, 80);
  setMotor(motorChannelB, motorB_in1, motorB_in2, 80);

  delay(2000);
  setMotor(motorChannelA, motorA_in1, motorA_in2, 40);
  setMotor(motorChannelB, motorB_in1, motorB_in2, 40);
  delay(2000);

  setMotor(motorChannelA, motorA_in1, motorA_in2, -80);
  setMotor(motorChannelB, motorB_in1, motorB_in2, -80);

  delay(2000);

  setMotor(motorChannelA, motorA_in1, motorA_in2, -40);
  setMotor(motorChannelB, motorB_in1, motorB_in2, -40);

  delay(2000);
}


void round4() {
  setMotor(motorChannelA, motorA_in1, motorA_in2, 80);
  setMotor(motorChannelB, motorB_in1, motorB_in2, -80);
  int randNumber = random(2000, 5000);
  delay(randNumber);

  stopMotor(motorChannelA, motorA_in1, motorA_in2);
  stopMotor(motorChannelB, motorB_in1, motorB_in2);
  delay(2000);
}
