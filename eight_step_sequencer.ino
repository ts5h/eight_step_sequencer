// Pin definitions
// const int STEP_PINS[8] = {2, 3, 4, 5, 6, 7, 8, 9};
const int STEP_PINS[8] = { 9, 8, 7, 6, 5, 4, 3, 2 };
const int RUN_BTN_PIN = 10;
const int RESET_BTN_PIN = 11;
const int STEP_BTN_PIN = 15;
const int TEMPO_POT_PIN = A0;

// State values
bool isRunning = false;
int currentStep = 0;
unsigned long lastStepTime = 0;

bool lastRunState = HIGH;
bool lastResetState = HIGH;
bool lastStepState = HIGH;

int delayWaitTime = 80;

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(STEP_PINS[i], OUTPUT);
    digitalWrite(STEP_PINS[i], LOW);
  }

  pinMode(RUN_BTN_PIN, INPUT_PULLUP);
  pinMode(RESET_BTN_PIN, INPUT_PULLUP);
  pinMode(STEP_BTN_PIN, INPUT_PULLUP);

  // Initialize
  digitalWrite(STEP_PINS[currentStep], HIGH);
}


void loop() {
  unsigned long currentMillis = millis();

  // "Run/Stop" button check (Toggle)
  bool runState = digitalRead(RUN_BTN_PIN);
  if (lastRunState == HIGH && runState == LOW) {
    isRunning = !isRunning;
    delay(delayWaitTime);
  }
  lastRunState = runState;

  // "Reset" button check (Go immediately to Step 1)
  bool resetState = digitalRead(RESET_BTN_PIN);
  if (lastResetState == HIGH && resetState == LOW) {
    digitalWrite(STEP_PINS[currentStep], LOW);
    currentStep = 0;
    digitalWrite(STEP_PINS[currentStep], HIGH);
    delay(delayWaitTime);
  }
  lastResetState = resetState;

  // "Forward" button check
  bool stepState = digitalRead(STEP_BTN_PIN);
  if (!isRunning && lastStepState == HIGH && stepState == LOW) {
    stepForward();
    delay(delayWaitTime);
  }

  // Sequence progression (Only during running)
  if (isRunning) {
    // BPM 40 (375ms)-240 (62.5ms)
    int tempoVal = analogRead(TEMPO_POT_PIN);
    int stepInterval = map(tempoVal, 0, 1023, 375, 62);

    if (currentMillis - lastStepTime >= stepInterval) {
      lastStepTime = currentMillis;
      stepForward();
    }
  }
}

void stepForward() {
  digitalWrite(STEP_PINS[currentStep], LOW);
  currentStep = (currentStep + 1) % 8;
  digitalWrite(STEP_PINS[currentStep], HIGH);
}
