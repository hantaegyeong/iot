#define LED_PIN 2       // 제어할 LED (보드 내장 LED or 외부 LED)
#define ADC_PIN 32      // 부하 전류/전압 감지용 ADC 핀

// --- 통신 프로토콜 정의 ---
#define FRAME_HEADER  0xAA
#define FRAME_FOOTER  0x55
#define CMD_LED_CTRL  0x10

// --- 응답 코드 ---
#define ACK  0x06
#define NACK 0x15

// --- 설정 값 ---
#define FAULT_THRESHOLD 200 // 이 값보다 낮으면 단선(Open)으로 판단

bool ledState = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  // ADC 핀은 별도 pinMode 설정 불필요 (analogRead 시 자동 설정됨)
  
  Serial.begin(115200);
  
  // 초기화: LED 끄기
  digitalWrite(LED_PIN, LOW);
  ledState = false;

  Serial.println("\n======================================");
  Serial.println("     ESP32 MINI ECU (CRC Ver) READY   ");
  Serial.println("======================================");
  Serial.println("Format: [AA] [10] [DATA] [CRC] [55]");
}

void loop() {
  // 1. 시리얼 버퍼에 5바이트 이상 들어왔는지 확인 (CRC 포함 5바이트)
  if (Serial.available() >= 5) {
    
    // 2. 헤더 확인 (Sync)
    if (Serial.peek() != FRAME_HEADER) {
      Serial.read(); // 헤더가 아니면 1바이트 버림
      return; 
    }

    // 3. 패킷 읽기
    byte header = Serial.read();  // 0xAA
    byte cmd    = Serial.read();  // 명령
    byte data   = Serial.read();  // 데이터
    byte crc    = Serial.read();  // 수신된 CRC
    byte footer = Serial.read();  // 0x55

    // 4. Footer 검사 (프레임 유효성)
    if (footer != FRAME_FOOTER) {
      Serial.print("Error: Invalid Footer -> ");
      Serial.println(NACK, HEX);
      return;
    }

    // 5. CRC 검사 (데이터 무결성)
    // 규칙: CMD + DATA의 합 (하위 1바이트)
    byte calcCRC = (cmd + data) & 0xFF;

    if (crc == calcCRC) {
      // CRC 통과 -> 명령 실행
      processCommand(cmd, data);
    } else {
      // CRC 실패 -> NACK 전송
      Serial.print("Error: CRC Mismatch! (Calc: ");
      Serial.print(calcCRC, HEX);
      Serial.print(" vs Recv: ");
      Serial.print(crc, HEX);
      Serial.println(", NACK)");
    }
  }
}

// --- 명령 처리 함수 ---
void processCommand(byte cmd, byte data) {
  if (cmd == CMD_LED_CTRL) {
    if (data == 0x01) {
      ledState = true;
      digitalWrite(LED_PIN, HIGH);
    } 
    else if (data == 0x00) {
      ledState = false;
      digitalWrite(LED_PIN, LOW);
    } 
    else {
      Serial.println("Error: Invalid Data Value");
      return;
    }

    // 제어 후 고장 진단 수행
    checkFaultAndRespond();
    
  } else {
    Serial.println("Error: Unknown Command");
  }
}

// --- 고장 진단 및 응답 함수 ---
void checkFaultAndRespond() {
  delay(50); // 전압 안정화 대기
  
  // ADC 값 읽기 (0 ~ 4095)
  int adcValue = analogRead(ADC_PIN);
  
  Serial.print("[RX] Executed. ADC: ");
  Serial.print(adcValue);

  bool isFault = false;

  // 진단 로직: LED를 켰는데 ADC 값이 너무 낮으면 '단선'
  if (ledState == true && adcValue < FAULT_THRESHOLD) {
    isFault = true;
  }

}
