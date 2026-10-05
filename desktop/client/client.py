import os
import sys
import socket
import json
from PyQt5 import QtWidgets, QtCore

from client_ui import Ui_MainWindow


SERVER_IP = os.environ.get("RTOS_SERVER_IP", "127.0.0.1")
SERVER_PORT = 5000


class SocketThread(QtCore.QThread):

    data_received = QtCore.pyqtSignal(str)

    def __init__(self, sock):
        super().__init__()
        self.sock = sock
        self.running = True

    def run(self):
        buffer = ""
        while self.running:
            try:
                data = self.sock.recv(2048)
                if data:
                    buffer += data.decode()
                    while "\n" in buffer:
                        line, buffer = buffer.split("\n", 1)
                        self.data_received.emit(line.strip())
            except:
                break

    def stop(self):
        self.running = False


class MainWindow(QtWidgets.QMainWindow):

    def __init__(self):
        super().__init__()

        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.socket = None
        self.thread = None

        self.connect_server()

        self.setup_buttons()

        self.start_clock()

    # =========================
    # CONNECT SERVER
    # =========================
    def connect_server(self):

        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((SERVER_IP, SERVER_PORT))

            print("Connected to server")

            self.thread = SocketThread(self.socket)
            self.thread.data_received.connect(self.update_data)
            self.thread.start()

        except:
            print("Server connection failed")

    # =========================
    # BUTTON
    # =========================
    def setup_buttons(self):

        self.ui.led1_on.clicked.connect(lambda: self.send_cmd("LED1_ON"))
        self.ui.led1_off.clicked.connect(lambda: self.send_cmd("LED1_OFF"))

        self.ui.led2_on.clicked.connect(lambda: self.send_cmd("LED2_ON"))
        self.ui.led2_off.clicked.connect(lambda: self.send_cmd("LED2_OFF"))

        self.ui.send.clicked.connect(self.send_text)

    # =========================
    # SEND COMMAND
    # =========================
    def send_cmd(self, cmd):

        try:
            self.socket.sendall((cmd + "\n").encode())
        except:
            print("Send error")

    def send_text(self):
        text = self.ui.textbox.text()
        N = len(text)
        if not text:
            return

        for i, c in enumerate(text):
            try:
                self.socket.sendall((c + "\n").encode())
                print(f"Sent char {i+1}/{N}: {c}")
            except:
                print("Send error")
            QtCore.QThread.msleep(500)  # delay 0.5s giữa các ký tự
    # =========================
    # UPDATE GUI
    # =========================
    def update_data(self, msg):

        print("Received:", msg)

        try:
            data = json.loads(msg)

            lux = data["lux"]
            led1 = data["led_sema"]
            led2 = data["led_tg"]

            self.ui.light_sensor.setText(f"LIGHT SENSOR: {lux:.1f} lux")

            if led1:
                self.ui.status_led1.setText("LED 1: ON")
            else:
                self.ui.status_led1.setText("LED 1: OFF")

            if led2:
                self.ui.status_led2.setText("LED 2: ON")
            else:
                self.ui.status_led2.setText("LED 2: OFF")

        except:
            print("Data format error")

    # =========================
    # CLOCK
    # =========================
    def start_clock(self):

        timer = QtCore.QTimer(self)
        timer.timeout.connect(self.update_clock)
        timer.start(1000)

    def update_clock(self):

        now = QtCore.QDateTime.currentDateTime()

        self.ui.time.setText(now.toString("hh:mm:ss"))
        self.ui.date.setText(now.toString("dd/MM/yyyy"))

    # =========================
    # CLOSE EVENT
    # =========================
    def closeEvent(self, event):

        if self.thread:
            self.thread.stop()

        if self.socket:
            self.socket.close()

        event.accept()


# =========================
# MAIN
# =========================
if __name__ == "__main__":

    app = QtWidgets.QApplication(sys.argv)

    window = MainWindow()
    window.show()

    sys.exit(app.exec_())
