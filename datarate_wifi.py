import socket
import time

# 1. ESP32 AP 모드의 기본 IP는 '192.168.4.1' 입니다.
ESP32_IP = "192.168.4.1" 
ESP32_PORT = 8080

TEST_DATA = b'x' * 1460 
BUFFER_SIZE = 4096     

print(f"Connecting to ESP32 AP at {ESP32_IP}:{ESP32_PORT}...")

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((ESP32_IP, ESP32_PORT))
    print("Connected!")
    
    # ---------------------------------
    # 테스트 1: PC가 송신 (ESP32의 Rx 측정용)
    # ---------------------------------
    print("Starting Part 1: Sending data to ESP32 (Rx Test)...")
    startTimeTx = time.time()
    total_sent = 0
    
    while (time.time() - startTimeTx < 10.5): # 10.5초간 전송
        bytes_sent = s.send(TEST_DATA)
        total_sent += bytes_sent
        
    print(f"Finished sending {total_sent} bytes.")
    
    # ---------------------------------
    # 테스트 2: PC가 수신 (ESP32의 Tx 측정용)
    # ---------------------------------
    print("\nStarting Part 2: Receiving data from ESP32 (Tx Test)...")
    total_received = 0
    startTimeRx = time.time()

    s.settimeout(12.0) 

    while True:
        data = s.recv(BUFFER_SIZE)
        if not data:
            break 
        total_received += len(data)

    durationRx = time.time() - startTimeRx
    print(f"Finished receiving {total_received} bytes in {durationRx:.2f}s.")

except socket.timeout:
    print("Socket timed out.")
except socket.error as e:
    print(f"Socket error: {e}")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    if 's' in locals():
        s.close()
    print("\nConnection closed. Check ESP32 Serial Monitor for FINAL Rx/Tx results.")
