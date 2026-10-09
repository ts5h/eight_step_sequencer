// Pin definitions
const int STEP_PINS[8] = {2, 3, 4, 5, 6, 7, 8, 9}; // 各ステップの5V供給ピン
const int TEMPO_POT_PIN = A0;                      // BPM調整ノブ
const int RUN_BTN_PIN   = 10;                      // Run/Stopモーメンタリボタン
const int RESET_BTN_PIN = 11;                      // Resetモーメンタリボタン

// State values
int currentStep = 0;
bool isRunning = false;
unsigned long lastStepTime = 0;

// ボタンのチャタリング防止用変数
bool lastRunState = HIGH;
bool lastResetState = HIGH;

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(STEP_PINS[i], OUTPUT);
    digitalWrite(STEP_PINS[i], LOW);
  }
  pinMode(RUN_BTN_PIN, INPUT_PULLUP);
  pinMode(RESET_BTN_PIN, INPUT_PULLUP);

  // 初期状態（ステップ0をアクティブに）
  digitalWrite(STEP_PINS[currentStep], HIGH);
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Run/Stop ボタンの判定（トグル動作）
  bool runState = digitalRead(RUN_BTN_PIN);
  if (lastRunState == HIGH && runState == LOW) { // 押された瞬間
    isRunning = !isRunning;
    delay(50); // 簡易デバウンス
  }
  lastRunState = runState;

  // 2. Reset ボタンの判定（即座にステップ1へ）
  bool resetState = digitalRead(RESET_BTN_PIN);
  if (lastResetState == HIGH && resetState == LOW) {
    digitalWrite(STEP_PINS[currentStep], LOW);
    currentStep = 0; // ステップ1（インデックス0）に戻す
    digitalWrite(STEP_PINS[currentStep], HIGH);
    delay(50);
  }
  lastResetState = resetState;

  // 3. シーケンスの進行処理（再生中の場合のみ）
  if (isRunning) {
    // BPM 40 (375ms) 〜 BPM 240 (62.5ms)
    int tempoVal = analogRead(TEMPO_POT_PIN);
    int stepInterval = map(tempoVal, 0, 1023, 375, 62); 

    if (currentMillis - lastStepTime >= stepInterval) {
      lastStepTime = currentMillis;

      // 現在のステップをOFFにして、次のステップへ
      digitalWrite(STEP_PINS[currentStep], LOW);
      currentStep = (currentStep + 1) % 8; // 0~7をループ
      digitalWrite(STEP_PINS[currentStep], HIGH);
    }
  }
}