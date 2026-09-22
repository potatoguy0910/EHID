import serial
import time

PORT = "COM5"
BAUD = 115200

patients = {
    1: {
        "name": "Aarav Patel",
        "age": 24,
        "blood_group": "B+",
        "allergies": "Penicillin",
        "conditions": "Asthma",
        "surgeries": "Appendectomy - 2024",
        "medications": "Salbutamol",
        "emergency_contact": "+91 XXXXX XXXXX"
    },

    2: {
        "name": "Rohan Shah",
        "age": 31,
        "blood_group": "O+",
        "allergies": "None",
        "conditions": "Diabetes",
        "surgeries": "None",
        "medications": "Metformin",
        "emergency_contact": "+91 XXXXX XXXXX"
    }
}


# Connect
esp32 = serial.Serial()
esp32.port = PORT
esp32.baudrate = BAUD
esp32.timeout = 1
esp32.dtr = False
esp32.rts = False
esp32.open()

time.sleep(2)
esp32.reset_input_buffer()

print("\n==============================")
print("   EMERGENCY HEALTH ID")
print("==============================")
print("ESP32 Connected")


def read_esp32():

    while True:

        line = esp32.readline().decode(
            "utf-8",
            errors="ignore"
        ).strip()

        if not line:
            continue

        print("ESP32:", line)

        if line.startswith("FINGERPRINT_ID:"):

            fingerprint_id = int(
                line.split(":")[1]
            )

            return fingerprint_id

        if line == "NO_MATCH":

            return None


def scan():

    print("\nPlace finger on the sensor...")

    esp32.write(b"SCAN\n")

    fingerprint_id = read_esp32()

    if fingerprint_id is None:

        print("\n❌ Fingerprint not recognized.")
        return

    print("\n✓ Fingerprint identified!")
    print("ID:", fingerprint_id)

    if fingerprint_id not in patients:

        print("⚠ No medical record found.")
        return

    patient = patients[fingerprint_id]

    print("\n==============================")
    print("       PATIENT RECORD")
    print("==============================")

    print("Name:", patient["name"])
    print("Age:", patient["age"])
    print("Blood Group:", patient["blood_group"])
    print("Allergies:", patient["allergies"])
    print("Conditions:", patient["conditions"])
    print("Surgeries:", patient["surgeries"])
    print("Medications:", patient["medications"])
    print("Emergency Contact:",
          patient["emergency_contact"])

    print("==============================")


def enroll():

    try:
        id = int(input("\nEnter fingerprint ID (1-127): "))

        if id < 1 or id > 127:
            print("Invalid ID.")
            return

        esp32.write(
            f"ENROLL:{id}\n".encode()
        )

        while True:

            line = esp32.readline().decode(
                "utf-8",
                errors="ignore"
            ).strip()

            if not line:
                continue

            print("ESP32:", line)

            if line == "ENROLL_SUCCESS":

                print("\n✓ Fingerprint enrolled successfully!")
                break

            if line == "ENROLL_FAILED":

                print("\n❌ Enrollment failed.")
                break

    except ValueError:

        print("Please enter a valid number.")


# Main menu

while True:

    print("\n==============================")
    print("1. Scan Fingerprint")
    print("2. Enroll Fingerprint")
    print("3. Exit")
    print("==============================")

    choice = input("Choose: ")

    if choice == "1":
        scan()

    elif choice == "2":
        enroll()

    elif choice == "3":
        break

    else:
        print("Invalid choice.")


esp32.close()
print("Disconnected.")