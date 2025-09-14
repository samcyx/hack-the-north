#include <WiFi.h>
#include <WebServer.h>
#include "html_page.h"

// ======= WIFI SETTINGS =======
const char* ssid     = "BasketBot_AP";
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
int pinkScore    = 0;
int whiteScore   = 0;
int currentLevel = 1;
String lastWinner = "-";
String message    = "";

// ======= MOTOR CONTROL =======
void setMotor(bool motorAForward, bool motorBForward) {
  digitalWrite(IN1, motorAForward ? HIGH : LOW);
  digitalWrite(IN2, motorAForward ? LOW  : HIGH);
  // Motor B wiring is inverted
  digitalWrite(IN3, motorBForward ? LOW  : HIGH);
  digitalWrite(IN4, motorBForward ? HIGH : LOW);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ======= LEVEL MOTION STATE =======
bool levelRunning = false;
unsigned long stepStart = 0;
bool forwardPhase = true;   // used for L2 & L3 alternating motion

void startLevel(int level) {
  levelRunning = true;
  stepStart = millis();
  forwardPhase = true;
  message = "Round " + String(level) + " starting...";
}

void updateLevel() {
  if (!levelRunning) return;

  unsigned long now = millis();

  switch (currentLevel) {
    case 1:
      // ----- Level 1: Stationary -----
      stopMotors();
      break;

    case 2:
      // ----- Level 2: Forward 3 s, Backward 3 s -----
      if (now - stepStart >= 3000) {          // swap every 3 s
        forwardPhase = !forwardPhase;
        stepStart = now;
      }
      // Both motors same direction for forward/back
      setMotor(forwardPhase, forwardPhase);
      break;

    default:
      // ----- Level 3+: Spin in place, alternate CW/CCW every 2 s -----
      if (now - stepStart >= 2000) {          // swap every 2 s
        forwardPhase = !forwardPhase;
        stepStart = now;
      }
      // Oppose motors to spin in place
      setMotor(forwardPhase, !forwardPhase);
      break;
  }
}

// ======= ROUND CONTROL =======
void checkRound() {
  if (pinkScore >= currentLevel * 3 || whiteScore >= currentLevel * 3) {
    lastWinner = (pinkScore > whiteScore) ? "Pink Team" : "White Team";
    message = lastWinner + " wins Round " + String(currentLevel) + "!";

    stopMotors();
    levelRunning = false;

    // advance to next level if not last and total < 15
    if (currentLevel < 5 && (pinkScore < 15 && whiteScore < 15)) {
      currentLevel++;
      startLevel(currentLevel);
    } else {
      message = (pinkScore > whiteScore)
                  ? "Pink Team wins the game!"
                  : "White Team wins the game!";
    }
  }
}

// ======= WEB HANDLERS =======
void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleUpdateScore() {
  if (server.hasArg("team") && server.hasArg("delta")) {
    String team = server.arg("team");
    int delta   = server.arg("delta").toInt();
    if (team == "pink")  pinkScore  = max(0, pinkScore  + delta);
    if (team == "white") whiteScore = max(0, whiteScore + delta);
    checkRound();
  }

  String json = "{";
  json += "\"pinkScore\":"   + String(pinkScore)    + ",";
  json += "\"whiteScore\":"  + String(whiteScore)   + ",";
  json += "\"currentLevel\":" + String(currentLevel)+ ",";
  json += "\"lastWinner\":\"" + lastWinner + "\",";
  json += "\"message\":\""    + message    + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleReset() {
  pinkScore = 0;
  whiteScore = 0;
  currentLevel = 1;
  lastWinner = "-";
  message = "";
  stopMotors();
  levelRunning = false;

  String json = "{";
  json += "\"pinkScore\":"   + String(pinkScore)    + ",";
  json += "\"whiteScore\":"  + String(whiteScore)   + ",";
  json += "\"currentLevel\":" + String(currentLevel)+ ",";
  json += "\"lastWinner\":\"" + lastWinner + "\",";
  json += "\"message\":\""    + message    + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

// ======= SETUP =======
void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  server.on("/", handleRoot);
  server.on("/updateScore", handleUpdateScore);
  server.on("/reset", handleReset);
  server.begin();

  startLevel(currentLevel);  // start first level
}

// ======= LOOP =======
void loop() {
  server.handleClient();
  updateLevel();  // non-blocking motion control
}
