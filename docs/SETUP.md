# Hướng dẫn chạy

## Firmware của nhóm

Cài PlatformIO, mở thư mục firmware/rtos. Dependency trong platformio.ini: Arduino framework, Adafruit GFX Library, Adafruit SSD1306 và BH1750. Board: esp32doit-devkit-v1. Platform/dependency chưa khóa phiên bản nên cần ghi lại môi trường sau khi build thành công.

```powershell
Copy-Item firmware/rtos/include/config.example.h firmware/rtos/include/config.local.h
```

Điền SSID/password và TCP_SERVER_IP là IP LAN máy tính chạy server, không phải 127.0.0.1. Port firmware/server/GUI là 5000. Sau đó:

```powershell
Set-Location firmware/rtos
pio run
pio run --target upload
pio device monitor --baud 115200
```

Với Non-OS, dùng firmware/non-os và không cần config Wi-Fi. Kiểm tra bảng GPIO riêng trước khi nạp.

## Server và GUI

Tại thư mục gốc repository:

```powershell
python -m venv .venv
.\.venv\Scripts\python.exe -m pip install -r desktop/requirements.txt
.\.venv\Scripts\python.exe desktop/server.py
```

Mở terminal thứ hai ở thư mục gốc:

```powershell
$env:RTOS_SERVER_IP = '127.0.0.1'
.\.venv\Scripts\python.exe desktop/client/client.py
```

Dùng 127.0.0.1 khi server và GUI ở cùng máy; nếu khác máy, điền IP LAN của máy server. Server bind 0.0.0.0:5000; cho phép kết nối LAN phù hợp trong firewall khi cần. Đây là demo TCP chưa có xác thực/mã hóa.

Resource imag.py đã đi kèm nên không cần Qt Designer để chạy. Muốn sửa UI: dùng desktop/qt/client.ui; tái sinh client_ui.py và imag.py bằng công cụ PyQt5 sau khi sửa.

## Bản tham khảo upstream

Xem [PROVENANCE.md](PROVENANCE.md) trước khi dùng. Arduino sketch ở reference/upstream/codeOS/final_version/final_version.ino, dùng Adafruit GFX/SSD1306, MPU6050_tockn và Firebase_ESP_Client.h. Copy config.example.h thành config.local.h cùng thư mục sketch, điền project/tài khoản Firebase của bạn. Không dùng tài khoản cũ của source.

GUI Firestore:

```powershell
python -m pip install -r reference/upstream/codeOS/QT/requirements.txt
$env:GOOGLE_APPLICATION_CREDENTIALS = 'C:\private-config\firebase-service-account.json'
Set-Location reference/upstream/codeOS/QT
python window.py
```

Credential JSON giữ ngoài repository. GUI cập nhật data1–data5 vào sensor_data/device1 bằng update(), nên document phải tồn tại. ESP32 gửi led2/mpuX/led3 và đọc data1–data5. Snapshot không kèm Firestore rules; cần cấu hình tài khoản/rules của project riêng. Không mở database công khai để bỏ qua lỗi quyền.

Non-OS upstream ở reference/upstream/codeNonOS/Non-OS, dùng platformio.ini gốc; board ESP32-S3 khác bản của nhóm.

## Xác minh

Đã kiểm tra cú pháp file Python và dựng UI Qt của nhóm ở chế độ offscreen. Chưa thử kết nối mạng thực, nạp firmware hay kiểm tra cảm biến. Các lệnh trên là hướng dẫn tái dựng môi trường, không phải tuyên bố build đã thành công.
