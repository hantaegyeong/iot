import serial
import time

# 1. '업로드'할 때 사용한 COM 포트 번호로 수정 (CH340 또는 CP210x)
# (블루투스 COM7이 아님!)
ESP32_COM_PORT = "COM4" 
# 2. ESP32 코드와 동일한 보드레이트 설정
ESP32_BAUD_RATE = 921600 

TEST_DATA = b'x' * 1024 
BUFFER_SIZE = 4096     

print(f"Connecting to ESP32 at {ESP32_COM_PORT} (Baud: {ESP32_BAUD_RATE})...")
print("!!! Arduino Serial Monitor가 닫혀 있는지 확인하세요 !!!")

try:
    ser = serial.Serial(ESP32_COM_PORT, ESP32_BAUD_RATE, timeout=1)
    print("Connected!")
    
    # ESP32가 준비될 시간 (연결 후 첫 데이터 전송 전 잠시 대기)
    time.sleep(2) 
    # (연결 신호 보내기)
    ser.write(b'Start') 

    # ---------------------------------
    # 테스트 1: PC Tx / ESP32 Rx
    # ---------------------------------
    print("\nStarting Part 1: PC Sending data (Rx Test)...")
    startTimeTx = time.time()
    total_sent = 0
    
    while (time.time() - startTimeTx < 10.5): # 10.5초간 전송
        bytes_sent = ser.write(TEST_DATA)
        total_sent += bytes_sent
        
    durationTx = time.time() - startTimeTx
    datarateRx_mbps = (total_sent * 8) / (durationTx * 1000 * 1000)
    print(f"Finished sending {total_sent} bytes in {durationTx:.2f}s.")
    
    # ---------------------------------
    # 테스트 2: PC Rx / ESP32 Tx
    # ---------------------------------
    print("\nStarting Part 2: PC Receiving data (Tx Test)...")
    total_received = 0
    startTimeRx = time.time()

    ser.timeout = 12.0 # 12초 타임아웃

    while (time.time() - startTimeRx < 10): # ESP32가 10초간 보냄
        data = ser.read(BUFFER_SIZE)
        if data:
            total_received += len(data)

    durationRx = time.time() - startTimeRx
    datarateTx_mbps = (total_received * 8) / (durationRx * 1000 * 1000)
    print(f"Finished receiving {total_received} bytes in {durationRx:.2f}s.")

    # ---------------------------------
    # 결과 종합 출력
    # ---------------------------------
    print("\n--- Combined UART Test Finished (PC-side) ---")
    print(f"PC Tx (ESP32 Rx) Rate: {datarateRx_mbps:.4f} Mbps")
    print(f"PC Rx (ESP32 Tx) Rate: {datarateTx_mbps:.4f} Mbps")
    print("---------------------------------------------")


except serial.SerialException as e:
    print(f"Serial error: {e}")
    print(f"'{ESP32_COM_PORT}'가 맞는지, Arduino 시리얼 모니터가 닫혀있는지 확인하세요.")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
    print("\nConnection closed.")
