import socket

ESP_IP = "10.196.112.173"
ESP_PORT = 4210

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def send_mode_command(command):
    try:
        sock.sendto(command.encode(), (ESP_IP, ESP_PORT))
        print("📡 MODE SENT →", command)
    except Exception as e:
        print("❌ Mode UDP Error:", e)
