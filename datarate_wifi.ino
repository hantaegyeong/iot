#include <WiFi.h>

// 1. ESP32가 만들 Wi-Fi 이름과 비밀번호 설정
const char* ap_ssid = "ESP32_AP_Test";
const char* ap_password = "password123"; // 8자리 이상 필수

// ESP32의 AP 모드 IP는 기본적으로 192.168.4.1로 고정됩니다.
WiFiServer server(8080);
WiFiClient client;

// 버퍼 설정
const int TEST_BUFFER_SIZE = 1460;
uint8_t dataBuffer[TEST_BUFFER_SIZE];

void setup() {
  Serial.begin(115200);
  delay(100);

  // 2. Wi-Fi AP 모드 시작
  Serial.printf("Starting AP: %s\n", ap_ssid);
  if (!WiFi.softAP(ap_ssid, ap_password)) {
    Serial.println("AP Start Failed!");
    while(1);
  }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP); // 3. 이 주소는 192.168.4.1 일 것입니다.

  // 서버 시작
  server.begin();
  Serial.println("Server started. Waiting for client to connect to this AP...");
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
    Serial.printf("\n--- Combined AP Test Finished ---\n");
    Serial.printf("Rx Rate: %.2f Mbps (%ld bytes in %.2fs)\n", datarateRx, totalDataReceived, durationRx);
    Serial.printf("Tx Rate: %.2f Mbps (%ld bytes in %.2fs)\n", datarateTx, totalDataSent, durationTx);
    Serial.println("---------------------------------");

    client.stop();
    Serial.println("Client disconnected. Waiting for new client...");
  }
}#include <WiFi.h>

// 1. ESP32가 만들 Wi-Fi 이름과 비밀번호 설정
const char* ap_ssid = "ESP32_AP_Test";
const char* ap_password = "password123"; // 8자리 이상 필수

// ESP32의 AP 모드 IP는 기본적으로 192.168.4.1로 고정됩니다.
WiFiServer server(8080);
WiFiClient client;

// 버퍼 설정
const int TEST_BUFFER_SIZE = 1460;
uint8_t dataBuffer[TEST_BUFFER_SIZE];

void setup() {
  Serial.begin(115200);
  delay(100);

  // 2. Wi-Fi AP 모드 시작
  Serial.printf("Starting AP: %s\n", ap_ssid);
  if (!WiFi.softAP(ap_ssid, ap_password)) {
    Serial.println("AP Start Failed!");
    while(1);
  }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP); // 3. 이 주소는 192.168.4.1 일 것입니다.

  // 서버 시작
  server.begin();
  Serial.println("Server started. Waiting for client to connect to this AP...");
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
    Serial.printf("\n--- Combined AP Test Finished ---\n");
    Serial.printf("Rx Rate: %.2f Mbps (%ld bytes in %.2fs)\n", datarateRx, totalDataReceived, durationRx);
    Serial.printf("Tx Rate: %.2f Mbps (%ld bytes in %.2fs)\n", datarateTx, totalDataSent, durationTx);
    Serial.println("---------------------------------");

    client.stop();
    Serial.println("Client disconnected. Waiting for new client...");
  }
}
