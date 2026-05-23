"""import requests

# ================= OLLAMA CONFIG =================

OLLAMA_URL = "http://localhost:11434/api/generate"

# CHANGE THIS TO YOUR MODEL NAME FROM: ollama list
# Example: "phi", "tinyllama", "mistral", "llama2", "llama3"

MODEL_NAME = "tinyllama"   # <-- CHANGE IF NEEDED


# ================= AI FUNCTION =================

def ask_ai(prompt):

    print("🧠 Sending to Ollama:", prompt)

    payload = {
    "model": MODEL_NAME,
    "prompt": f""" """You are ASHA robot, a helpful assistant.
Give short, complete answers in one sentence.

User question: {prompt}
ASHA answer:""" """,
    "stream": False,
    "options": {
        "temperature": 0.3,
        "num_predict": 30,
        "stop": ["User:", "\n\n"]
    }
}

    try:
        response = requests.post(OLLAMA_URL, json=payload, timeout=120)

        print("Status Code:", response.status_code)

        if response.status_code != 200:
            print("OLLAMA ERROR RESPONSE:", response.text)
            return "AI server error"

        data = response.json()
        print("RAW AI DATA:", data)

        # Extract response safely
        if "response" in data:
            return data["response"].strip()

        else:
            return "AI response format error"

    except requests.exceptions.ConnectionError:
        return "Ollama server not running"

    except Exception as e:
        print("AI ERROR:", e)
        return "AI system error"
"""


import requests

# ================= OLLAMA CONFIG =================

OLLAMA_URL = "http://localhost:11434/api/generate"

# CHANGE THIS TO YOUR MODEL NAME FROM: ollama list
# Example: "phi", "tinyllama", "mistral", "llama2", "llama3"

MODEL_NAME = "tinyllama"   # <-- CHANGE IF NEEDED


# ================= AI FUNCTION =================

def ask_ai(prompt):

    print("🧠 Sending to Ollama:", prompt)

    payload = {
        "model": MODEL_NAME,
        "prompt": prompt,
        "stream": False
    }

    try:
        response = requests.post(OLLAMA_URL, json=payload, timeout=120)

        print("Status Code:", response.status_code)

        if response.status_code != 200:
            print("OLLAMA ERROR RESPONSE:", response.text)
            return "AI server error"

        data = response.json()
        print("RAW AI DATA:", data)

        # Extract response safely
        if "response" in data:
            return data["response"].strip()

        else:
            return "AI response format error"

    except requests.exceptions.ConnectionError:
        return "Ollama server not running"

    except Exception as e:
        print("AI ERROR:", e)
        return "AI system error"
