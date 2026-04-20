// ── Input pins ─────────────────────────────
const int inPins[4] = {2, 3, 4, 5};

// ── Output pins ────────────────────────────
const int outPins[4] = {6, 7, 8, 9};

// ── Store previous states (to avoid spam) ─
bool prevState[4] = {LOW, LOW, LOW, LOW};

void setup() {

  Serial.begin(9600);

  // Setup pins
  for (int i = 0; i < 4; i++) {
    pinMode(inPins[i], INPUT);      // change to INPUT_PULLUP if needed
    pinMode(outPins[i], OUTPUT);
    digitalWrite(outPins[i], LOW);
  }

  Serial.println("System Ready...");
}

// ── Main Loop ─────────────────────────────
void loop() {

  for (int i = 0; i < 4; i++) {

    bool currentState = digitalRead(inPins[i]);

    // Mirror input → output
    digitalWrite(outPins[i], currentState);

    // Print only when state changes
    if (currentState != prevState[i]) {

      Serial.print("Input Pin ");
      Serial.print(inPins[i]);

      if (currentState == HIGH) {
        Serial.print(" → HIGH | Output Pin ");
        Serial.print(outPins[i]);
        Serial.println(" → HIGH");
      } else {
        Serial.print(" → LOW  | Output Pin ");
        Serial.print(outPins[i]);
        Serial.println(" → LOW");
      }

      prevState[i] = currentState;
    }
  }
}
