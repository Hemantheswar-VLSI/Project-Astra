import speech_recognition as sr
import pyttsx3

engine = pyttsx3.init()
engine.setProperty("rate", 150)

recognizer = sr.Recognizer()
mic = sr.Microphone()


def speak(text):
    print("🤖 ASHA:", text)
    engine.say(text)
    engine.runAndWait()


def listen_command():

    with mic as source:
        recognizer.adjust_for_ambient_noise(source, duration=0.3)
        audio = recognizer.listen(source)

    try:
        text = recognizer.recognize_google(audio)
        return text

    except:
        text="waiting for wake word "
        return None
