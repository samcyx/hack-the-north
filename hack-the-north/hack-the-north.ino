#include <WiFi.h>
#include <WebServer.h>
#include "html_page.h"

// ======= WIFI SETTINGS =======
const char* ssid = "BasketBot_AP";
const char* password = "12345678";

// ======= WEB SERVER =======
WebServer server(80);

// ======= MOTOR PINS =======
const int ENA = 4;   // always HIGH
const int IN1 = 5;
const int IN2 = 6;
const int IN3 = 7;
const int IN4 = 15;
const int ENB = 16;  // always HIGH

// ======= GAME VARIABLES =======
int pinkScore = 0;
int whiteScore = 0;
int currentLevel = 1;
bool gameOver = false;
String finalWinner = "";

// ======= ROUND CONTROL =======
struct RoundStep {
  unsigned long duration; // ms
  bool motorAForward;
  bool motorBForward;
};

RoundStep levelSteps[3][10] = {
  // Level 1: stationary
  {{3000, false, false}},
  // Level 2: forward/backward sequence
  {{4000, true, true}, {4000, false, false}},
  // Level 3: variable speeds (simplified as durations + direction)
  {{2000, true, true}, {2000, true, true}, {2000, false, false}, {2000, false, false}}
};

int currentStep = 0;
unsigned long stepStartTime = 0;
bool levelRunning = false;

// ======= MOTOR CONTROL =======
void setMotor(bool motorAForward, bool motorBForward) {
  digitalWrite(IN1, motorAForward ? HIGH : LOW);
  digitalWrite(IN2, motorAForward ? LOW : HIGH);
  digitalWrite(IN3, motorBForward ? HIGH : LOW);
  digitalWrite(IN4, motorBForward ? LOW : HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ======= START LEVEL =======
void startLevel(int level) {
  currentStep = 0;
  stepStartTime = millis();
  levelRunning = true;
}

// ======= UPDATE LEVEL =======
void updateLevel() {
  if (!levelRunning) return;

  RoundStep step = levelSteps[currentLevel - 1][currentStep];
  if (!step.motorAForward && !step.motorBForward && step.duration > 0) {
    stopMotors();
  } else {
    setMotor(step.motorAForward, step.motorBForward);
  }

  if (millis() - stepStartTime >= step.duration) {
    currentStep++;
    stepStartTime = millis();
    if (currentStep >= sizeof(levelSteps[currentLevel - 1]) / sizeof(RoundStep)) {
      levelRunning = false;
      stopMotors();
    }
  }
}

// ======= CHECK ROUND WIN =======
void checkRound() {
  if (pinkScore >= currentLevel * 3 || whiteScore >= currentLevel * 3) {
    stopMotors();
    levelRunning = false;

    if (currentLevel < 3) {
      currentLevel++;
      startLevel(currentLevel);
    } else {
      gameOver = true;
      finalWinner = (pinkScore > whiteScore) ? "Pink Team" : "White Team";
    }
  }
}

// ======= WEB HANDLERS =======
void handleRoot() { server.send(200, "text/html", INDEX_HTML); }

void handleUpdateScore() {
  if (server.hasArg("team") && server.hasArg("delta")) {
    String team = server.arg("team");
    int delta = server.arg("delta").toInt();
    if (team == "pink") pinkScore = max(0, pinkScore + delta);
    else if (team == "white") whiteScore = max(0, whiteScore + delta);

    checkRound();
  }

  String json = "{";
  json += "\"pinkScore\":" + String(pinkScore) + ",";
  json += "\"whiteScore\":" + String(whiteScore) + ",";
  json += "\"currentLevel\":" + String(currentLevel) + ",";
  json += "\"gameOver\":" + String(gameOver ? "true" : "false") + ",";
  json += "\"finalWinner\"😕"" + finalWinner + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleReset() {
  pinkScore = 0; whiteScore = 0; currentLevel = 1; gameOver = false; finalWinner = "";
  startLevel(currentLevel);

  String json = "{";
  json += "\"pinkScore\":" + String(pinkScore) + ",";
  json += "\"whiteScore\":" + String(whiteScore) + ",";
  json += "\"currentLevel\":" + String(currentLevel) + ",";
  json += "\"gameOver\":" + String(gameOver ? "true" : "false") + ",";
  json += "\"finalWinner\"😕"" + finalWinner + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

// ======= SETUP =======
void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: "); Serial.println(WiFi.softAPIP());

  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  digitalWrite(ENA, HIGH); digitalWrite(ENB, HIGH);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  server.on("/", handleRoot);
  server.on("/updateScore", handleUpdateScore);
  server.on("/reset", handleReset);
  server.begin();

  startLevel(currentLevel);
}

// ======= LOOP =======
void loop() {
  server.handleClient();
  updateLevel();
}