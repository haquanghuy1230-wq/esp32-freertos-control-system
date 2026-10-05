# Hạn chế quan sát từ mã nguồn

Các nhận xét là kết quả đọc mã, chưa phải kết quả đo trên phần cứng. Logic gốc được giữ trong quá trình khôi phục.

| Điểm trong bản của nhóm | Ảnh hưởng / hướng hoàn thiện |
| --- | --- |
| I2C dùng chung cho OLED/BH1750, chưa có mutex bảo vệ | Cần kiểm tra và đồng bộ truy cập bus |
| lux, toggle_status, b và server_response chia sẻ giữa task | Cần cơ chế chia sẻ dữ liệu/snapshot thích hợp |
| wifiMutex được khai báo nhưng chưa sử dụng | Không mô tả đây là mutex đã bảo vệ mạng |
| OLED chỉ vẽ lại khi lux/toggle/server_response đổi | Bộ đếm b đổi riêng có thể không cập nhật màn hình |
| BTN2 ISR chưa chống dội | Có thể phát sinh sự kiện lặp hoặc chờ thêm chu kỳ LED |
| Wi-Fi setup chờ vô hạn trước khi tạo task | Chưa hoạt động độc lập khi không kết nối được mạng |
| TCP task busy-wait tối đa 2s khi chờ data | Cần nhường CPU hoặc thiết kế nhận không chặn phù hợp |
| Server xử lý mỗi recv như một message hoàn chỉnh | TCP có thể chia/gộp dòng; cần buffer theo delimiter |
| gui_clients thêm lại mỗi lệnh, không loại client ngắt | Có thể gửi trùng và giữ socket cũ |
| GUI đọc socket nhưng không xử lý recv trả rỗng | Có thể lặp khi kết nối đóng; cần thoát/reconnect |
| Gửi text với msleep trong main UI thread | Giao diện có thể đứng khi gửi chuỗi dài |
| GUI có LED1_OFF nhưng firmware chưa xử lý | LED1_ON kích hoạt LED_SEMA khoảng 3s; không phải điều khiển LED_BLINK |
| GUI hiển thị led_sema bằng nhãn LED1 | Cần thống nhất tên với sơ đồ và firmware |
| TCP không có xác thực/mã hóa | Giới hạn demo trong mạng phù hợp |

Non-OS khôi phục dùng các timer millis() và không có mã mạng trong đoạn báo cáo. Trạng thái LED3 là biến bool, không phải semaphore của kernel.

Bản upstream có các vấn đề cần kiểm chứng riêng: I2C/global dùng chung chưa được bảo vệ đầy đủ, vòng nhận Firestore thiếu timeout, queue giữ con trỏ String cấp phát động và có nhánh cần xử lý khi queue đầy. README nguồn ghi 8 task nhưng sketch thực tế tạo 9 task. Không diễn giải việc pin vào Core 1 là các task chạy đồng thời vật lý trên cùng một lõi.

Không kết luận FreeRTOS tự động đáp ứng mọi deadline, hoặc Non-OS không thể làm mạng. Trên Arduino ESP32, delay() cũng nhường task gọi nó cho scheduler; so sánh cần dựa vào tổ chức ứng dụng và đo timing thực tế.
