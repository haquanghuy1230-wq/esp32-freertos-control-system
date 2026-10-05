# Phần cứng và GPIO

## RTOS của nhóm — ESP32 DevKit V1 / BH1750

| Thiết bị / tín hiệu | GPIO theo mã còn lưu |
| --- | --- |
| LED_BLINK | 27 |
| LED_TG | 26 |
| LED_SEMA | 25 |
| BUTTON_1 | 32, INPUT_PULLUP |
| BUTTON_2 | 33, INPUT_PULLUP, ngắt FALLING |
| I2C SDA / SCL | 21 / 22 |
| BH1750 | I2C, địa chỉ 0x23 |
| OLED SSD1306 | 128×64, I2C địa chỉ 0x3C |

## Non-OS khôi phục từ báo cáo

| Tín hiệu | GPIO |
| --- | --- |
| LED_1 / LED_2 / LED_3 | 15 / 4 / 23 |
| BUTTON_1 / BUTTON_2 | 5 / 18 |
| I2C SDA / SCL | 21 / 22 |

GPIO khác bản RTOS, phải kiểm tra dây khi đổi firmware. Tên LED1 trong UI client hiện chỉ LED semaphore, không phải LED_BLINK; xem các hạn chế trước khi demo.

## Snapshot tham khảo — ESP32-S3 / MPU6050

| Tín hiệu | GPIO theo snapshot |
| --- | --- |
| LED1 / LED2 / LED3 | 9 / 10 / 11 |
| BTN1 / BTN2 / BTN3 | 16 / 17 / 18 (BTN3 chỉ OS) |
| SDA / SCL | 4 / 5 |
| OLED | I2C địa chỉ 0x3C |

Đây là bản khác; không nạp lên DevKit V1 với sơ đồ của nhóm. Non-OS tham khảo khai báo board 4d_systems_esp32s3_gen4_r8n16 trong platformio.ini; phải xác nhận board thực tế trước khi upload.

Các nút INPUT_PULLUP nối xuống GND khi nhấn. Dùng điện trở hạn dòng cho LED; kiểm tra điện áp/module I2C phù hợp logic ESP32 3.3V và nối chung GND.
