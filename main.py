from speech import speak, listen_command as listen
from voice_mode import run_voice_mode
from gesture import run_gesture

WAKE_WORD = "hello"

print("\n🤖 Ayesha ROBOT READY")
speak("ASHA robot online. Say hello")

while True:

    print("\n🎧 Waiting wake word...")
    wake = listen()
    
    print(wake)
    if not wake:
        
        continue

    wake = wake.lower().strip()
    print("Heard:", wake)

    # -------- WAKE SYSTEM --------
    if WAKE_WORD in wake:

        speak("System activated,say voice mode or gesture mode ")

        while True:

            print("\n🧭 Waiting mode...")
            mode = listen()

            if not mode:
                continue

            mode = mode.lower().strip()
            print("Mode:", mode)

            # -------- VOICE MODE --------
            if "voice" in mode:
                speak("Opening voice mode on")
                result = run_voice_mode()
                

                # DIRECT SWITCH TO GESTURE
                if result == "gesture":
                    speak("stopping voice mode and opening gesture mode")
                    run_gesture()
                    speak("Returned to main menu")
                elif result =="shutdown":
                    speak("Going to sleep mode")
                    break
                    
                else:
                    speak("Returned to main menu")
    

            # -------- GESTURE MODE --------
            elif "gesture" in mode:
                speak("Opening gesture mode")
                run_gesture()
                speak("Returned to main menu")
        
            # -------- SLEEP MODE --------
            elif "shutdown" in mode:
                speak("Going to sleep mode")
                wake=""
                break

            else:
                speak("Please say voice mode or gesture mode")
    elif "shutdown completely" in wake:
        speak("Completly  shutdowning . power on manuvally to do other tasks ")
        break

