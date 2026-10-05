# Nguồn mã và phạm vi khôi phục

## Dự án của nhóm Hà Quang Huy

Thông tin học phần, thành viên và giảng viên lấy từ `C:/RTOS/RTOS_report.docx`, bìa tháng 3/2026. Source RTOS lấy từ `C:/RTOS/OS_Project/src/main.cpp`; cấu hình PlatformIO lấy từ OS_Project. Server lấy từ `C:/RTOS/server.py`, client và UI lấy từ `C:/RTOS/client`, tài nguyên Qt lấy từ `C:/RTOS/qt_design`.

Bản Non-OS trích nguyên khối C++ ở mục 4.2.2 của báo cáo, chuyển khoảng trắng NBSP của Word thành khoảng trắng thường. platformio.ini được bổ sung dựa trên cấu hình DevKit V1/BH1750 còn lưu; chưa phải file build gốc của phiên bản Non-OS.

Thay đổi để công khai: tách SSID/password/IP firmware sang config.local.h (file mẫu đi kèm); GUI đọc RTOS_SERVER_IP từ môi trường; bỏ mã số sinh viên khỏi UI và nguồn Qt Designer. Không sửa logic task hoặc giao thức. Không đưa DOCX, cache .pio/.vscode, backup và workspace cá nhân lên GitHub.

## Mã tham khảo tải theo yêu cầu

- Nguồn: [TuanNguyn-11/RTOS-FreeRTOS-ESP32](https://github.com/TuanNguyn-11/RTOS-FreeRTOS-ESP32).
- Commit snapshot: [`a67da79e560c07d1899444f2b6e41f4f37455aca`](https://github.com/TuanNguyn-11/RTOS-FreeRTOS-ESP32/tree/a67da79e560c07d1899444f2b6e41f4f37455aca).
- Thời điểm lấy bản: 05/10/2026.
- Ghi nhận tác giả theo README nguồn: Phan Ngọc Tuấn Nguyên, Nguyễn Trọng Nhân, Hà Quang Minh, Phạm Trọng An Nam.
- Nội dung: codeOS/final_version, ứng dụng QT cùng tài nguyên và codeNonOS/Non-OS.

Snapshot nằm dưới reference/upstream, có cấu trúc nguồn riêng. Không sao chép lịch sử Git vì chứa phiên bản cấu hình tài khoản/mạng; commit nguồn vẫn được dẫn ở trên. Không kèm README gốc vì phần hướng dẫn credential không còn phù hợp sau khi làm sạch; xem README gốc tại liên kết commit.

Thay đổi trong snapshot: các macro Wi-Fi/Firebase chuyển vào config.local.h và file mẫu; Python window.py dùng pathlib chuẩn và GOOGLE_APPLICATION_CREDENTIALS thay đường dẫn service-account cố định; bổ sung requirements.txt; bỏ tempCodeRunnerFile.py. Không thay thành viên nguồn bằng thành viên nhóm Hà Quang Huy. Tài nguyên/logo được giữ để UI tham khảo có đủ resource.

README upstream nói 8 task, nhưng mã snapshot có 9 lệnh xTaskCreatePinnedToCore, gồm taskBTN3. Thông tin task trong tài liệu này dựa trên mã. Snapshot không kèm service-account JSON; các tài khoản/mật khẩu hard-code không được đăng lại.

Repository nguồn không có file LICENSE trong commit đã tải. Không tự gán MIT hoặc một giấy phép khác cho source tham khảo; ghi nhận tác giả không đồng nghĩa với việc được cấp giấy phép phân phối lại. Tài nguyên bên thứ ba giữ quyền của chủ sở hữu tương ứng.
