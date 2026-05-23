
import socket
import time
import sys
import threading
from speech import speak, listen_command as listen

# ================= ESP UDP CONFIG =================
ESP_IP = "10.196.112.173"
ESP_PORT = 4210

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

SEND_DELAY = 1.0
last_sent_command = None
last_sent_time = 0

# Movement control
movement_thread = None
stop_event = threading.Event()
movement_lock = threading.Lock()


# ================= UDP SEND =================
def send_command(command):
    global last_sent_command, last_sent_time

    if command == last_sent_command and time.time() - last_sent_time < SEND_DELAY:
        return

    last_sent_command = command
    last_sent_time = time.time()

    try:
        sock.sendto(command.encode(), (ESP_IP, ESP_PORT))
        print("📨 Sent →", command)
    except Exception as e:
        print("❌ UDP Error:", e)


# ================= INTERRUPTIBLE MOVEMENT =================
def start_movement(command_code, duration):
    global movement_thread, stop_event
    
    with movement_lock:

        # Stop previous movement
        stop_event.set()
        send_command("04")
        time.sleep(0.1)

        # Reset stop event for new movement
        stop_event = threading.Event()

        def movement_worker():
            send_command(command_code)
            print("🚗 Moving:", command_code)
            
            start_time = time.time()

            while time.time() - start_time < duration:
                if stop_event.is_set():
                    print("🛑 Movement interrupted")
                    return
                time.sleep(0.05)

            send_command("04")
            print("🛑 Movement completed")

        movement_thread = threading.Thread(target=movement_worker, daemon=True)
        movement_thread.start()



# ================= VOICE MODE =================
def run_voice_mode():
    speak("Voice mode activated")
    send_command("u")
    try:
        while True:
            print("\n🎤 Voice Mode Listening...")
            command = listen()

            if not command:
                continue

            command = command.lower().strip()
            print("➡ Command:", command)

            # -------- SWITCH TO GESTURE MODE --------
            if "gesture" in command:
                speak("Switching to gesture mode")
                send_command("u")
                return "gesture"

            # -------- EXIT VOICE MODE --------
            elif "exit" in command or command == "back":
                send_command("04")
                speak("Leaving voice mode ")
                break

            # -------- MOVEMENT COMMANDS --------
            elif "forward" in command:
                speak("Moving forward")
                start_movement("01", 2)
            elif "backward" in command:
                speak("Moving backward")
                start_movement("00", 0.5)

            elif "left" in command:
                speak("Turning left")
                start_movement("03", 1)

            elif "right" in command:
                speak("Turning right")
                start_movement("02", 1)

            elif "stop" in command:
                speak("Stopping robot")
                stop_event.set()
                send_command("04")
            
            elif "remote" in command:
                stop_event.set()
                speak("Stopping voice mode and remote mode activated")
                send_command("r")
                while True:
                    print("\n🎤 Voice Mode Listening...")
                    command = listen()

                    if not command:
                        continue

                    command = command.lower().strip()
                    print("➡ Command:", command)
                    if "voice" in command:
                        send_command("u")
                        speak("Stopping remote mode and voice mode activated")
                        break
                    elif "remote" in command:
                        speak("You are already in Remote mode ")
                    elif "shutdown" in command:
                        stop_event.set()
                        send_command("s")
                        return "shutdown"
                        break
                
            elif "voice" in command:
                    speak(" You are already in voice mode  ")
                    
            elif "shutdown" in command:
                    stop_event.set()
                    send_command("s")
                    return "shutdown"
                    break
            else:
                speak("Command not recognized")

    except KeyboardInterrupt:
        speak("Voice mode stopped by user")
        stop_event.set()
        send_command("04")
        return "back"


