#define BOOT_BUTTON 0
#define LED_PIN 2
#define ADC_PIN 32   // ADC compatible pin

bool ledState = false;
bool lastButtonState = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  pinMode(BOOT_BUTTON, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  delay(200);

  Serial.println("======================================");
  Serial.println("  ESP32 LED TOGGLE + FAULT DETECTOR");
  Serial.println("======================================");
  Serial.println("READY: Press BOOT to toggle LED.\n");
}

void loop() {

  // ----- BUTTON READ -----
  int reading = digitalRead(BOOT_BUTTON);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);

      Serial.print("\n[BUTTON] LED State: ");
      Serial.println(ledState ? "ON" : "OFF");

      while (digitalRead(BOOT_BUTTON) == LOW) delay(5);
    }
  }

  lastButtonState = reading;


  // ----- ADC READ -----
  int adcValue = analogRead(ADC_PIN);

  Serial.print("[ADC] ");
  Serial.print(adcValue);
  Serial.print("  →  ");

  // NEW CORRECT FAULT LOGIC:
  // LOW ADC = LED physically present and load exists (NORMAL)
  // HIGH ADC = Open circuit, LED removed (FAULT)
  if (adcValue > 3500 || adcValue < 200) {
    Serial.println("❌ FAULT (LED disconnected / open circuit)");
  } 
  else {
    Serial.println("✔ NORMAL (LED connected)");
  }

  delay(100);
}
