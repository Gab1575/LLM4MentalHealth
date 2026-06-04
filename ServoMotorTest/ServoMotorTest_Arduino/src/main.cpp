#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>

#define SERVOMIN       80
#define SERVOMAX       510
#define SERVO_CENTER   295
#define CURRENT_DELAY  2
#define NUM_SERVOS     16

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
uint16_t servoPositions[NUM_SERVOS];

void printHelp();
void processCommand(const String &command);
bool parseInteger(const String &text, int &value);
void moveServo(uint8_t servoNum, uint16_t position);
void resetAllServos();
void parseServoCommand(const String &args);

void setup() {
  Serial.begin(9600);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);

  resetAllServos();
  printHelp();
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.length() > 0) {
      processCommand(command);
    }
  }
}

void printHelp() {
  Serial.println(F("Available commands:"));
  Serial.println(F("  servo <number> <delta>  - move a single servo by delta"));
  Serial.println(F("  reset                   - reset all servos to middle"));
  Serial.println(F("  help                    - show this message"));
  Serial.println(F("Example: servo 3 20"));
}

void processCommand(const String &command) {
  String lower = command;
  lower.toLowerCase();

  if (lower.startsWith("servo ")) {
    parseServoCommand(command.substring(6));
    return;
  }

  if (lower == "reset") {
    resetAllServos();
    Serial.println(F("All servos reset to center."));
    return;
  }

  if (lower == "help") {
    printHelp();
    return;
  }

  Serial.println(F("Unknown command. Type help for usage."));
}

bool parseInteger(const String &text, int &value) {
  char buffer[16];
  text.toCharArray(buffer, sizeof(buffer));
  char *endPtr;
  long parsed = strtol(buffer, &endPtr, 10);

  if (endPtr == buffer || *endPtr != '\0') {
    return false;
  }
  value = (int)parsed;
  return true;
}

void parseServoCommand(const String &args) {
  String trimmed = args;
  trimmed.trim();

  int firstSpace = trimmed.indexOf(' ');
  if (firstSpace < 0) {
    Serial.println(F("Usage: servo <number> <delta>"));
    return;
  }

  String servoStr = trimmed.substring(0, firstSpace);
  String deltaStr = trimmed.substring(firstSpace + 1);
  servoStr.trim();
  deltaStr.trim();

  int servoNum;
  int delta;
  if (!parseInteger(servoStr, servoNum) || !parseInteger(deltaStr, delta)) {
    Serial.println(F("Invalid servo number or delta. Please enter integers."));
    return;
  }

  if (servoNum < 0 || servoNum >= NUM_SERVOS) {
    Serial.print(F("Servo number must be 0 to "));
    Serial.println(NUM_SERVOS - 1);
    return;
  }

  int target = servoPositions[servoNum] + delta;
  if (target < SERVOMIN) {
    target = SERVOMIN;
  } else if (target > SERVOMAX) {
    target = SERVOMAX;
  }

  servoPositions[servoNum] = (uint16_t)target;
  moveServo((uint8_t)servoNum, servoPositions[servoNum]);

  Serial.print(F("Moved servo "));
  Serial.print(servoNum);
  Serial.print(F(" to position "));
  Serial.println(servoPositions[servoNum]);
}

void moveServo(uint8_t servoNum, uint16_t position) {
  if (position < SERVOMIN) {
    position = SERVOMIN;
  }
  if (position > SERVOMAX) {
    position = SERVOMAX;
  }

  pwm.setPWM(servoNum, 0, position);
  delay(CURRENT_DELAY);
}

void resetAllServos() {
  for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
    servoPositions[i] = SERVO_CENTER;
    moveServo(i, SERVO_CENTER);
  }
}
