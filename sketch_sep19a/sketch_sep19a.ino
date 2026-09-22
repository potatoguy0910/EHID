#include <Adafruit_Fingerprint.h>

HardwareSerial fingerSerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

void setup() {
  Serial.begin(115200);

  // R307S TX -> GPIO 12
  // R307S RX -> GPIO 13
  fingerSerial.begin(57600, SERIAL_8N1, 12, 13);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("SENSOR_READY");
  } else {
    Serial.println("SENSOR_ERROR");
    while (1);
  }

  finger.getTemplateCount();

  Serial.print("STORED_FINGERPRINTS:");
  Serial.println(finger.templateCount);

  Serial.println("READY");
}

void loop() {

  if (Serial.available()) {
    char command = Serial.read();

    if (command == 'E' || command == 'e') {
      enrollFingerprint();
    }

    if (command == 'S' || command == 's') {
      scanFingerprint();
    }
  }
}


// ================= ENROLL =================

void enrollFingerprint() {

  Serial.println("ENROLL_START");
  Serial.println("ENTER_ID");

  while (!Serial.available());
  int id = Serial.parseInt();

  if (id < 1 || id > 127) {
    Serial.println("INVALID_ID");
    return;
  }

  Serial.print("ENROLLING_ID:");
  Serial.println(id);

  int p = -1;

  Serial.println("PLACE_FINGER");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();

    if (p == FINGERPRINT_OK) {
      Serial.println("IMAGE_CAPTURED");
    }
  }

  p = finger.image2Tz(1);

  if (p != FINGERPRINT_OK) {
    Serial.println("IMAGE_PROCESS_ERROR");
    return;
  }

  Serial.println("REMOVE_FINGER");

  delay(2000);

  while (finger.getImage() != FINGERPRINT_NOFINGER);

  Serial.println("PLACE_FINGER_AGAIN");

  p = -1;

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();

    if (p == FINGERPRINT_OK) {
      Serial.println("SECOND_IMAGE_CAPTURED");
    }
  }

  p = finger.image2Tz(2);

  if (p != FINGERPRINT_OK) {
    Serial.println("SECOND_PROCESS_ERROR");
    return;
  }

  p = finger.createModel();

  if (p != FINGERPRINT_OK) {
    Serial.println("FINGERPRINT_MISMATCH");
    return;
  }

  p = finger.storeModel(id);

  if (p == FINGERPRINT_OK) {

    Serial.println("ENROLL_SUCCESS");

    Serial.print("FINGERPRINT_ID:");
    Serial.println(id);

  } else {

    Serial.println("ENROLL_FAILED");
  }
}


// ================= SCAN =================

void scanFingerprint() {

  Serial.println("SCAN_START");
  Serial.println("PLACE_FINGER");

  int p = -1;

  while (p != FINGERPRINT_OK) {

    p = finger.getImage();

    if (p == FINGERPRINT_OK) {
      Serial.println("FINGER_DETECTED");
    }
  }

  p = finger.image2Tz();

  if (p != FINGERPRINT_OK) {
    Serial.println("PROCESS_ERROR");
    return;
  }

  p = finger.fingerSearch();

  if (p == FINGERPRINT_OK) {

    Serial.println("MATCH_FOUND");

    Serial.print("FINGERPRINT_ID:");
    Serial.println(finger.fingerID);

    Serial.print("CONFIDENCE:");
    Serial.println(finger.confidence);

  } else {

    Serial.println("NO_MATCH");
  }

  delay(1000);
}