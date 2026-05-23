import socket
import cv2
import mediapipe as mp
import time


# ================= ESP UDP CONFIG =================
ESP_IP = "10.196.112.173"   # change if needed
ESP_PORT = 4210

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

SEND_DELAY = 0.8
last_sent_command = None
last_sent_time = 0


# ================= SEND COMMAND FUNCTION =================
def send_command(command):
    global last_sent_command, last_sent_time

    # Prevent spamming same command
    if command == last_sent_command and time.time() - last_sent_time < SEND_DELAY:
        return

    last_sent_command = command
    last_sent_time = time.time()

    try:
        sock.sendto(command.encode(), (ESP_IP, ESP_PORT))
        print("📨 Sent →", command)
    except Exception as e:
        print("❌ UDP Error:", e)


# ================= MAIN GESTURE FUNCTION =================
def run_gesture():

    mp_hands = mp.solutions.hands
    hands = mp_hands.Hands(
        max_num_hands=2,
        min_detection_confidence=0.7,
        min_tracking_confidence=0.7
    )

    mp_draw = mp.solutions.drawing_utils
    cap = cv2.VideoCapture(0,cv2.CAP_V4L2)

    # 🔥 Camera Safety Check
    if not cap.isOpened():
        print("❌ Camera failed to open")
        return

    print("🖐️ ASHA Gesture Control Started")
    send_command("gesture mode on")

    current_command = None
    command_buffer = []
    BUFFER_SIZE = 7
    COOLDOWN_TIME = 3.0
    last_command_time = 0
    two_hand_start = None

    while True:

        success, frame = cap.read()
        if not success:
            print("❌ Camera read failed")
            break

        frame = cv2.flip(frame, 1)
        rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        result = hands.process(rgb)

        detected_command = None

        if result.multi_hand_landmarks:

            # ========= TWO HAND EXIT =========
            if len(result.multi_hand_landmarks) == 2:
                if two_hand_start is None:
                    two_hand_start = time.time()
                elif time.time() - two_hand_start > 2:
                    print("🛑 Two Hands Detected - EXIT")
                    send_command("00")
                    time.sleep(1)
                    break
            else:
                two_hand_start = None

            # ========= SINGLE HAND =========
            handLms = result.multi_hand_landmarks[0]
            mp_draw.draw_landmarks(frame, handLms, mp_hands.HAND_CONNECTIONS)

            fingers = []

            # Thumb
            thumb_tip = handLms.landmark[4]
            thumb_joint = handLms.landmark[2]
            fingers.append(1 if thumb_tip.x > thumb_joint.x else 0)

            # Other fingers
            tips = [8, 12, 16, 20]
            for tip in tips:
                fingers.append(
                    1 if handLms.landmark[tip].y <
                    handLms.landmark[tip - 2].y else 0
                )

            count = sum(fingers)

            # Map finger count to command
            if count == 0:
                detected_command = "00"
            elif count == 1:
                detected_command = "01"
            elif count == 2:
                detected_command = "02"
            elif count == 3:
                detected_command = "03"
            elif count == 4:
                detected_command = "04"

        else:
            two_hand_start = None

        # ========= SMOOTHING + SAFETY =========
        if detected_command:

            command_buffer.append(detected_command)

            if len(command_buffer) > BUFFER_SIZE:
                command_buffer.pop(0)

            most_common = max(set(command_buffer), key=command_buffer.count)

            if command_buffer.count(most_common) > BUFFER_SIZE // 2:

                if most_common != current_command:

                    if time.time() - last_command_time > COOLDOWN_TIME:

                        print("✅ STABLE:", most_common)

                        # Safety stop before changing
                        if current_command is not None:
                            print("🛑 Safety Stop")
                            send_command("00")
                            time.sleep(1)

                        send_command(most_common)

                        current_command = most_common
                        last_command_time = time.time()

        # ========= DISPLAY =========
        cv2.putText(
            frame,
            f"Current: {current_command}",
            (10, 40),
            cv2.FONT_HERSHEY_SIMPLEX,
            1,
            (0, 255, 0),
            2
        )

        cv2.imshow("ASHA Gesture Control", frame)

        # Press Q to exit
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

    # Cleanup
    send_command("gesture mode off")
    cap.release()
    cv2.destroyAllWindows()
    print("🔁 Returning to main system")
if __name__ == "__main__":
    run_gesture()
