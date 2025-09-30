#include <WiFi.h>
#include <WiFiClient.h>

const char* ssid = "TaeTaeTae";
const char* password = "1234567890a";

// TCP 서버 객체 생성, 포트 8080 사용
WiFiServer server(8080);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Access Point mode...");

  // AP 모드 시작
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // TCP 서버 시작
  server.begin();
  Serial.println("TCP server started on port 8080");
}

void loop() {
  // 클라이언트(노트북) 연결 대기
  WiFiClient client = server.available();

  // 새로운 클라이언트가 연결되면
  if (client) {
    Serial.println("New client connected!");

    // 클라이언트로부터 데이터 수신
    String receivedData = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        receivedData += c;
        // 데이터의 끝을 알리는 줄바꿈 문자 확인
        if (c == '\n') {
          break;
        }
      }
    }
    
    // 수신된 데이터 출력
    if (receivedData.length() > 0) {
      receivedData.trim(); // 공백 제거
      Serial.print("Received from Python: ");
      Serial.println(receivedData);

      // 클라이언트로 응답 전송
      client.println("Message received successfully!");
    }

    // 연결 종료
    client.stop();
    Serial.println("Client disconnected.");
  }
} 