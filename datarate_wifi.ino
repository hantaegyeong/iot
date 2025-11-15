#include <WiFi.h>

// 1. 본인의 Wi-Fi 정보
const char* ssid = "U+Net6C68";
const char* password = "B413G#B011";

WiFiServer server(8080);
WiFiClient client;

// 버퍼 설정
const int TEST_BUFFER_SIZE = 1460;
uint8_t dataBuffer[TEST_BUFFER_SIZE];

void setup() {
  Serial.begin(115200);
  delay(100);

  // Wi-Fi 연결
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // 2. 이 IP를 PC 스크립트에 사용

  // 서버 시작
  server.begin();
  Serial.println("Server started. Waiting for client for combined Rx/Tx test...");
}

void loop() {
  if (!client.connected()) {
    client = server.available();
    if (!client) {
      return;
    }
    Serial.println("\nClient connected! Starting combined test...");
    
    // ---------------------------------
    // 테스트 1: Rx 측정 (PC -> ESP32)
    // ---------------------------------
    Serial.println("Starting Rx Test (Receiving for 10s)...");
    unsigned long startTimeRx = millis();
    long totalDataReceived = 0;

    while (millis() - startTimeRx < 10000) {
      if (client.available()) {
        int bytesRead = client.read(dataBuffer, TEST_BUFFER_SIZE);
        if (bytesRead > 0) {
          totalDataReceived += bytesRead;
        }
      } else if (!client.connected()) {
          Serial.println("Client disconnected during Rx test.");
          break;
      }
    }
    // 남은 버퍼 비우기
    while(client.available() && client.connected()) {
       client.read(dataBuffer, TEST_BUFFER_SIZE);
    }
    
    unsigned long endTimeRx = millis();
    float durationRx = (endTimeRx - startTimeRx) / 1000.0;
    float datarateRx = (totalDataReceived * 8.0) / (durationRx * 1000.0 * 1000.0);

    if (!client.connected()) {
        Serial.println("Client disconnected. Aborting.");
        return;
    }

    // ---------------------------------
    // 테스트 2: Tx 측정 (ESP32 -> PC)
    // ---------------------------------
    Serial.println("\nStarting Tx Test (Sending for 10s)...");
    unsigned long startTimeTx = millis();
    long totalDataSent = 0;

    // 버퍼 초기화 (의미 없는 데이터)
    for(int i=0; i<TEST_BUFFER_SIZE; i++) dataBuffer[i] = (uint8_t)i;

    while (millis() - startTimeTx < 10000) {
      int bytesSent = client.write(dataBuffer, TEST_BUFFER_SIZE);
      if (bytesSent > 0) {
        totalDataSent += bytesSent;
      } else if (bytesSent < 0 || !client.connected()) {
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
    Serial.printf("\n--- Combined Test Finished ---\n");
    Serial.printf("Rx Rate: %.2f Mbps (%ld bytes in %.2fs)\n", datarateRx, totalDataReceived, durationRx);
    Serial.printf("Tx Rate: %.2f Mbps (%ld bytes in %.2fs)\n", datarateTx, totalDataSent, durationTx);
    Serial.println("---------------------------------");

    client.stop();
    Serial.println("Client disconnected. Waiting for new client...");
  }
}
