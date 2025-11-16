import serial
import time

# 1. 2단계에서 확인한 '나가는' COM 포트 번호로 수정하세요.
ESP32_COM_PORT = "COM7"

TEST_DATA = b'x' * 1024 # 보낼 데이터
BUFFER_SIZE = 4096     # 받을 버퍼

print(f"Connecting to ESP32 at {ESP32_COM_PORT}...")

try:
    # 2. pyserial로 COM 포트에 연결 (보드레이트는 상징적일 수 있음)
    ser = serial.Serial(ESP32_COM_PORT, 115200, timeout=1)
    print("Connected!")
    
    # ---------------------------------
    # 테스트 1: PC가 송신 (ESP32의 Rx 측정용)
    # ---------------------------------
    print("Starting Part 1: Sending data to ESP32 (Rx Test)...")
    startTimeTx = time.time()
    total_sent = 0
    
    while (time.time() - startTimeTx < 10.5): # 10.5초간 전송
        bytes_sent = ser.write(TEST_DATA)
        total_sent += bytes_sent
        
    print(f"Finished sending {total_sent} bytes.")
    
    # ---------------------------------
    # 테스트 2: PC가 수신 (ESP32의 Tx 측정용)
    # ---------------------------------
    print("\nStarting Part 2: Receiving data from ESP32 (Tx Test)...")
    total_received = 0
    startTimeRx = time.time()

    ser.timeout = 12.0 # 12초 타임아웃

    while True:
        data = ser.read(BUFFER_SIZE)
        if not data:
            break # ESP32가 10초 테스트를 마치고 데이터 전송을 멈춤
        total_received += len(data)

    durationRx = time.time() - startTimeRx
    print(f"Finished receiving {total_received} bytes in {durationRx:.2f}s.")

except serial.SerialException as e:
    print(f"Serial error: {e}")
    print(f"'{ESP32_COM_PORT}'가 맞는지, ESP32가 페어링되었는지 확인하세요.")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
    print("\nConnection closed. Check ESP32 Serial Monitor for FINAL Rx/Tx results.")
