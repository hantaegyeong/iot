import socket
import time

# ESP32의 AP IP 주소와 포트
HOST = '192.168.4.1'
PORT = 8080

def send_to_esp32(message):
    try:
        # 소켓 생성 및 ESP32에 연결
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT))
        print(f"Connected to ESP32 at {HOST}:{PORT}")

        # 메시지 전송 (줄바꿈 문자를 포함하여 ESP32가 데이터의 끝을 알게 함)
        s.sendall(message.encode('utf-8') + b'\n')
        print(f"Sent: {message}")

        # ESP32로부터 응답 수신
        response = s.recv(1024).decode('utf-8').strip()
        print(f"Received from ESP32: {response}")

    except Exception as e:
        print(f"An error occurred: {e}")

    finally:
        # 소켓 닫기
        s.close()
        print("Connection closed.")

if __name__ == "__main__":
    # 이 부분을 수정하여 원하는 데이터를 전송
    print("dddfsdfa")
    my_data = "Hello from Python!"
    send_to_esp32(my_data)
    
    # 여러 번 전송 테스트
    time.sleep(1)
    send_to_esp32("Python is sending another message.")