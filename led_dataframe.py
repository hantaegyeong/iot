import serial
import time

# ==========================================
# [설정] 본인의 포트 번호로 수정하세요!
# ==========================================
PORT = 'COM5'        # 윈도우
BAUDRATE = 115200

# --- 프로토콜 상수 ---
HEADER = 0xAA
CMD_LED = 0x10
FOOTER = 0x55

def calculate_crc(cmd, data):

    return (cmd + data) & 0xFF

def run_terminal():
    try:
        ser = serial.Serial(PORT, BAUDRATE, timeout=1)
        print(f"✅ {PORT} 포트에 연결 성공! (CRC 모드)")
        print("------------------------------------------------------")
        print(" [사용법]")
        print(" 1. 단축키: '1'(ON), '0'(OFF) -> CRC 자동 계산됨")
        print(" 2. 수동입력: 'AA 10 01 11 55' (CRC 에러 테스트용)")
        print(" 3. 종료: 'q'")
        print("------------------------------------------------------")

        while True:
            user_input = input("\n명령 입력 >> ").strip().upper()

            if user_input == 'Q':
                print("종료합니다.")
                break

            packet = bytearray()

            # --- 1. 단축키 모드 (CRC 자동 계산) ---
            if user_input == '1':
                data = 0x01
                crc = calculate_crc(CMD_LED, data) # 자동 계산: 0x10 + 0x01 = 0x11
                packet = bytearray([HEADER, CMD_LED, data, crc, FOOTER])
                print(f"[자동생성] LED ON 패킷 (CRC: {crc:02X})")
            
            elif user_input == '0':
                data = 0x00
                crc = calculate_crc(CMD_LED, data) # 자동 계산: 0x10 + 0x00 = 0x10
                packet = bytearray([HEADER, CMD_LED, data, crc, FOOTER])
                print(f"[자동생성] LED OFF 패킷 (CRC: {crc:02X})")

            # --- 2. 수동 입력 모드 (CRC 에러 테스트 등) ---
            else:
                try:
                    hex_string = user_input.replace(" ", "")
                    packet = bytearray.fromhex(hex_string)
                except ValueError:
                    print("형식이 잘못되었습니다.")
                    continue

            # --- 3. 데이터 전송 ---
            ser.write(packet)
            
            # 보낸 데이터 눈으로 확인하기 (Hex)
            hex_view = ' '.join(f'{b:02X}' for b in packet)
            print(f"전송됨: {hex_view}")

            # --- 4. ESP32 응답 받기 ---
            time.sleep(0.1) 
            if ser.in_waiting > 0:
                response = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
                print(f"ESP32 : {response.strip()}")
            else:
                print("응답 없음")

    except serial.SerialException:
        print(f"포트 연결 실패! ({PORT})")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()

if __name__ == "__main__":
    run_terminal()
