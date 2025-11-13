#include <EEPROM.h>

const int dotButtonPin = 26;
const int dashButtonPin = 27;
const int slashButtonPin = 28;
const int replayButtonPin = 29;
const int dotLedPin = 6;
const int dashLedPin = 7;
const int slashLedPin = 0;
const int replayLedPin = 1;

const int EEPROM_SIZE = 128;

String morseSequence = "";
bool recording = false;

void saveMorseSequence(const String &seq) {
  int len = seq.length();
  if (len > EEPROM_SIZE - 1) len = EEPROM_SIZE - 1;
  for (int i = 0; i < len; i++) {
    EEPROM.write(i, seq[i]);
  }
  EEPROM.write(len, 0);
  EEPROM.commit();
}

String loadMorseSequence() {
  char buffer[EEPROM_SIZE];
  int i = 0;
  while (i < EEPROM_SIZE - 1) {
    buffer[i] = EEPROM.read(i);
    if (buffer[i] == 0) break;
    i++;
  }
  buffer[i] = '\0';
  return String(buffer);
}

void setup() {
  EEPROM.begin(EEPROM_SIZE);
  morseSequence = loadMorseSequence();
  Serial.begin(115200);

  pinMode(dotButtonPin, INPUT_PULLUP);
  pinMode(dashButtonPin, INPUT_PULLUP);
  pinMode(slashButtonPin, INPUT_PULLUP);
  pinMode(replayButtonPin, INPUT_PULLUP);

  pinMode(dotLedPin, OUTPUT);
  pinMode(dashLedPin, OUTPUT);
  pinMode(slashLedPin, OUTPUT);
  pinMode(replayLedPin, OUTPUT);

  digitalWrite(dotLedPin, LOW);
  digitalWrite(dashLedPin, LOW);
  digitalWrite(slashLedPin, LOW);
  digitalWrite(replayLedPin, LOW);
}

void loop() {
  if (!recording && (
      digitalRead(dotButtonPin) == LOW ||
      digitalRead(dashButtonPin) == LOW ||
      digitalRead(slashButtonPin) == LOW )) {
    morseSequence = "";
    recording = true;
  }

  if (digitalRead(replayButtonPin) == LOW) {
    delay(50);
    if (recording) {
      saveMorseSequence(morseSequence);
      recording = false;
    }
    replayMorseCode();
    while (digitalRead(replayButtonPin) == LOW) delay(10);
  }

  if (recording) {
    if (digitalRead(dotButtonPin) == LOW) {
      lightOne(dotLedPin);
      morseSequence += ".";
      while (digitalRead(dotButtonPin) == LOW) delay(10);
    } else if (digitalRead(dashButtonPin) == LOW) {
      lightOne(dashLedPin);
      morseSequence += "-";
      while (digitalRead(dashButtonPin) == LOW) delay(10);
    } else if (digitalRead(slashButtonPin) == LOW) {
      lightOne(slashLedPin);
      morseSequence += "/";
      while (digitalRead(slashButtonPin) == LOW) delay(10);
    } else {
      lightOne(-1);
    }
  } else {
    lightOne(-1);
  }
}

void lightOne(int ledPin) {
  digitalWrite(dotLedPin, ledPin == dotLedPin ? HIGH : LOW);
  digitalWrite(dashLedPin, ledPin == dashLedPin ? HIGH : LOW);
  digitalWrite(slashLedPin, ledPin == slashLedPin ? HIGH : LOW);
}

void replayMorseCode() {
  Serial.print("Replay Morse: ");
  Serial.println(morseSequence);
  for (unsigned int i = 0; i < morseSequence.length(); i++) {
    char c = morseSequence[i];
    if (c == '.') {
      digitalWrite(replayLedPin, HIGH);
      delay(200);
    } else if (c == '-') {
      digitalWrite(replayLedPin, HIGH);
      delay(600);
    } else if (c == '/') {
      digitalWrite(replayLedPin, LOW);
      delay(1000);
      continue;
    }
    digitalWrite(replayLedPin, LOW);
    delay(200);
  }
  digitalWrite(replayLedPin, LOW);
}
