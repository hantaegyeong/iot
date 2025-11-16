// 921600 보드레이트 사용
const int BAUD_RATE = 115200;

// 버퍼 설정
const int TEST_BUFFER_SIZE = 1024;
uint8_t dataBuffer[TEST_BUFFER_SIZE];

void setup() {
  // 1. USB 시리얼을 921600으로 시작
  Serial.begin(BAUD_RATE);
  // (참고: 아무것도 출력하지 않음. PC 스크립트가 바로 연결할 수 있도록 대기)
}

void loop() {
  // 2. PC가 연결할 때까지 대기
  if (Serial.available()) {
    // (연결 신호로 들어온 데이터 비우기)
    while(Serial.available()) Serial.read();

    // ---------------------------------
    // 테스트 1: Rx 측정 (PC -> ESP32)
    // ---------------------------------
    unsigned long startTimeRx = millis();
    long totalDataReceived = 0;

    while (millis() - startTimeRx < 10000) {
      if (Serial.available()) {
        int bytesRead = Serial.readBytes(dataBuffer, TEST_BUFFER_SIZE);
        if (bytesRead > 0) {
          totalDataReceived += bytesRead;
        }
      }
    }
    // 남은 버퍼 비우기
    while(Serial.available()) {
       Serial.readBytes(dataBuffer, TEST_BUFFER_SIZE);
    }
    
    // ---------------------------------
    // 테스트 2: Tx 측정 (ESP32 -> PC)
    // ---------------------------------
    unsigned long startTimeTx = millis();
    long totalDataSent = 0;

    for(int i=0; i<TEST_BUFFER_SIZE; i++) dataBuffer[i] = (uint8_t)i;

    while (millis() - startTimeTx < 10000) {
      // 버퍼가 비어있을 때만 데이터를 보냄 (블로킹 방지)
      if (Serial.availableForWrite() > TEST_BUFFER_SIZE) {
         int bytesSent = Serial.write(dataBuffer, TEST_BUFFER_SIZE);
         if (bytesSent > 0) {
            totalDataSent += bytesSent;
         }
      }
    }

    // 테스트가 끝나면 다시 PC의 연결을 기다림
  }
}
