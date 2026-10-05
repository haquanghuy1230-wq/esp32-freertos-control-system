import socket
import threading

HOST = '0.0.0.0'
PORT = 5000

esp_client = None
gui_clients = []

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen()

print("Server running...")

def handle_client(conn, addr):

    global esp_client

    print("Connected:", addr)

    while True:
        try:
            data = conn.recv(1024)

            if not data:
                break

            message = data.decode().strip()

            print("Received:", message)

            # ESP32 gửi JSON
            if message.startswith("{"):

                esp_client = conn

                # forward cho GUI
                for g in gui_clients:
                    try:
                        g.send((message+"\n").encode())
                    except:
                        pass


            # GUI gửi lệnh
            else:

                gui_clients.append(conn)

                print("Command from GUI:", message)

                if esp_client:
                    esp_client.send((message+"\n").encode())

        except:
            break

    conn.close()

while True:

    conn, addr = server.accept()

    thread = threading.Thread(target=handle_client, args=(conn, addr))
    thread.start()