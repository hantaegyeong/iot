#include "BluetoothSerial.h"

// BluetoothSerial 객체 생성
BluetoothSerial SerialBT;

// 버퍼 설정
const int TEST_BUFFER_SIZE = 1024; // (BT는 버퍼를 조금 작게 잡는 것이 안정적일 수 있음)
uint8_t dataBuffer[TEST_BUFFER_SIZE];

void setup() {
  Serial.begin(115200);
  delay(100);

  // 1. 블루투스 시작 (PC에서 보일 이름 설정)
  if (!SerialBT.begin("ESP32_DataTest")) {
    Serial.println("Bluetooth start failed!");
    while(1);
  }
  
  Serial.println("Bluetooth started. PC에서 'ESP32_DataTest'를 페어링하세요.");
  Serial.println("Waiting for PC client connection...");
}

void loop() {
  // 2. PC가 COM 포트를 통해 연결할 때까지 대기
  if (SerialBT.available()) {
    Serial.println("\nClient connected! Starting combined test...");
    
    // (연결 신호로 들어온 데이터 비우기)
    while(SerialBT.available()) SerialBT.read();

    // ---------------------------------
    // 테스트 1: Rx 측정 (PC -> ESP32)
    // ---------------------------------
    Serial.println("Starting Rx Test (Receiving for 10s)...");
    unsigned long startTimeRx = millis();
    long totalDataReceived = 0;

    while (millis() - startTimeRx < 10000) {
      if (SerialBT.available()) {
        int bytesRead = SerialBT.readBytes(dataBuffer, TEST_BUFFER_SIZE);
        if (bytesRead > 0) {
          totalDataReceived += bytesRead;
        }
      } else if (!SerialBT.connected()) {
          Serial.println("Client disconnected during Rx test.");
          break;
      }
    }
    // 남은 버퍼 비우기
    while(SerialBT.available() && SerialBT.connected()) {
       SerialBT.readBytes(dataBuffer, TEST_BUFFER_SIZE);
    }
    
    unsigned long endTimeRx = millis();
    float durationRx = (endTimeRx - startTimeRx) / 1000.0;
    float datarateRx = (totalDataReceived * 8.0) / (durationRx * 1000.0 * 1000.0);

    if (!SerialBT.connected()) {
        Serial.println("Client disconnected. Aborting.");
        return;
    }
    
    // ---------------------------------
    // 테스트 2: Tx 측정 (ESP32 -> PC)
    // ---------------------------------
    Serial.println("\nStarting Tx Test (Sending for 10s)...");
    unsigned long startTimeTx = millis();
    long totalDataSent = 0;

    for(int i=0; i<TEST_BUFFER_SIZE; i++) dataBuffer[i] = (uint8_t)i;

    while (millis() - startTimeTx < 10000) {
      int bytesSent = SerialBT.write(dataBuffer, TEST_BUFFER_SIZE);
      if (bytesSent > 0) {
        totalDataSent += bytesSent;
      } else if (!SerialBT.connected()) {
        Serial.println("Client disconnected during Tx test.");
        break;
      }
    }

    unsigned long endTimeTx = millis();
    float durationTx = (endTimeTx - startTimeTx) / 1000.0;
    float datarateTx = (totalDataSent * 8.0) / (durationTx * 1000.0 * 1000.0);

    // ---------------------------------
    // 결과 종합 출력
    // ---------------------------------
    Serial.printf("\n--- Combined Bluetooth Test Finished ---\n");
    Serial.printf("Rx Rate: %.2f Mbps (%ld bytes in %.2fs)\n", datarateRx, totalDataReceived, durationRx);
    Serial.printf("Tx Rate: %.2f Mbps (%ld bytes in %.2fs)\n", datarateTx, totalDataSent, durationTx);
    Serial.println("---------------------------------");

    // 연결을 바로 끊지 않고 다음 테스트를 위해 대기
    Serial.println("Test finished. Waiting for new client connection...");
  }
}
