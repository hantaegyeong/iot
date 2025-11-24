#define BOOT_BUTTON 0   // ESP32 BOOT 버튼 (GPIO0)
#define LED_PIN 2       // 트랜지스터 제어 출력 핀

bool ledState = false;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  pinMode(BOOT_BUTTON, INPUT_PULLUP); 
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW); // 시작할 때 LED OFF
}

void loop() {
  int reading = digitalRead(BOOT_BUTTON);

  // 디바운싱 처리 (버튼 노이즈 방지)
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // 버튼 상태가 안정적으로 바뀐 경우
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW) {  // 버튼 눌렸을 때
      ledState = !ledState; // LED 상태 토글
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);

      // 버튼을 떼기 전까지 추가 토글 방지
      while (digitalRead(BOOT_BUTTON) == LOW) {
        delay(10);
      }
    }
  }

  lastButtonState = reading;
}
