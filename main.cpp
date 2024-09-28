#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Pin Allocations
// RFID Readers
#define RFID1_RX 2
#define RFID1_TX 3
#define RFID2_RX 4
#define RFID2_TX 5
#define RFID3_RX 6
#define RFID3_TX 7

// Servo Motors
#define SERVO1_PIN 8
#define SERVO2_PIN 9
#define SERVO3_PIN 10

// IR Sensors (Each has 3 pins: VCC, GND, Output)
#define IR_SENSOR1_PIN 11
#define IR_SENSOR1_VCC 12
#define IR_SENSOR1_GND 13

#define IR_SENSOR2_PIN 14
#define IR_SENSOR2_VCC 15
#define IR_SENSOR2_GND 16

#define IR_SENSOR3_PIN 17
#define IR_SENSOR3_VCC 18
#define IR_SENSOR3_GND 19

// LEDs for each locker (2 pins each: Red and Green)
#define RED_LED1_PIN 20
#define GREEN_LED1_PIN 21
#define RED_LED2_PIN 22
#define GREEN_LED2_PIN 23
#define RED_LED3_PIN 24
#define GREEN_LED3_PIN 25

// GSM Module
#define GSM_RX 26
#define GSM_TX 27

// Sanitizer Control
#define SANITIZER_PIN 28

// Seven-Segment Display Pins
#define SEG_A 29
#define SEG_B 30
#define SEG_C 31
#define SEG_D 32
#define SEG_E 33
#define SEG_F 34
#define SEG_G 35
#define SEG_DP 36

// Initialize objects
Servo servo1;
Servo servo2;
Servo servo3;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust I2C address as necessary
SoftwareSerial gsm(GSM_RX, GSM_TX);

// RFID Variables
String rfidInput;

// Initialize locker states and counters
bool lockerState[3] = {false, false, false}; // false = available, true = occupied
int availableCount = 3;

// Function declarations
void setup();
void loop();
void sanitizeHands();
void displayAvailableLockers();
void checkRFID();
void operateLocker(int lockerIndex, bool lock);
void alertUnauthorizedAccess();
void displayCountOnSevenSegment(int count);
void updateLEDs();

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  gsm.begin(9600);

  // Initialize Servos
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);

  // Initialize LCD
  lcd.begin();
  lcd.backlight();
  lcd.print("Safe Hold Locker");

  // Initialize IR Sensors
  pinMode(IR_SENSOR1_PIN, INPUT);
  pinMode(IR_SENSOR1_VCC, OUTPUT);
  pinMode(IR_SENSOR1_GND, OUTPUT);

  pinMode(IR_SENSOR2_PIN, INPUT);
  pinMode(IR_SENSOR2_VCC, OUTPUT);
  pinMode(IR_SENSOR2_GND, OUTPUT);

  pinMode(IR_SENSOR3_PIN, INPUT);
  pinMode(IR_SENSOR3_VCC, OUTPUT);
  pinMode(IR_SENSOR3_GND, OUTPUT);

  // Set IR Sensor VCC to HIGH and GND to LOW
  digitalWrite(IR_SENSOR1_VCC, HIGH);
  digitalWrite(IR_SENSOR1_GND, LOW);

  digitalWrite(IR_SENSOR2_VCC, HIGH);
  digitalWrite(IR_SENSOR2_GND, LOW);

  digitalWrite(IR_SENSOR3_VCC, HIGH);
  digitalWrite(IR_SENSOR3_GND, LOW);

  // Initialize LED pins
  pinMode(RED_LED1_PIN, OUTPUT);
  pinMode(GREEN_LED1_PIN, OUTPUT);
  pinMode(RED_LED2_PIN, OUTPUT);
  pinMode(GREEN_LED2_PIN, OUTPUT);
  pinMode(RED_LED3_PIN, OUTPUT);
  pinMode(GREEN_LED3_PIN, OUTPUT);

  // Initialize Sanitizer
  pinMode(SANITIZER_PIN, OUTPUT);
  
  // Initialize Seven-Segment Display
  for (int pin = SEG_A; pin <= SEG_G; pin++) {
    pinMode(pin, OUTPUT);
  }
  digitalWrite(SEG_DP, LOW); // Decimal point off

  displayAvailableLockers();
}

void loop() {
  checkRFID();
  updateLEDs();
}

void checkRFID() {
  // Simulating RFID readings for the sake of example
  // Replace this with actual RFID reading code
  if (Serial.available()) {
    rfidInput = Serial.readStringUntil('\n'); // Read RFID input

    // Example RFID matching (replace with your actual RFID values)
    if (rfidInput == "CARD1") {
      operateLocker(0, true); // Lock/unlock locker 1
    } else if (rfidInput == "CARD2") {
      operateLocker(1, true); // Lock/unlock locker 2
    } else if (rfidInput == "CARD3") {
      operateLocker(2, true); // Lock/unlock locker 3
    } else {
      alertUnauthorizedAccess();
    }
  }
}

void operateLocker(int lockerIndex, bool lock) {
  if (lock) {
    if (!lockerState[lockerIndex]) { // If locker is available
      lockerState[lockerIndex] = true; // Set to occupied
      availableCount--;
      lcd.clear();
      lcd.print("Locker ");
      lcd.print(lockerIndex + 1);
      lcd.print(" Locked");

      // Open the locker using servo
      switch (lockerIndex) {
        case 0: servo1.write(90); break; // Open locker 1
        case 1: servo2.write(90); break; // Open locker 2
        case 2: servo3.write(90); break; // Open locker 3
      }
      delay(5000); // Keep locker open for 5 seconds
      switch (lockerIndex) {
        case 0: servo1.write(0); break; // Close locker 1
        case 1: servo2.write(0); break; // Close locker 2
        case 2: servo3.write(0); break; // Close locker 3
      }
    } else {
      lcd.clear();
      lcd.print("Locker Occupied");
    }
  }

  sanitizeHands();
}

void sanitizeHands() {
  digitalWrite(SANITIZER_PIN, HIGH);
  delay(2000); // Dispense for 2 seconds
  digitalWrite(SANITIZER_PIN, LOW);
}

void displayAvailableLockers() {
  // Display available lockers on seven-segment display
  displayCountOnSevenSegment(availableCount);
}

void displayCountOnSevenSegment(int count) {
  // Logic to display count on 7-segment display
  switch (count) {
    case 3:
      digitalWrite(SEG_A, HIGH);
      digitalWrite(SEG_B, HIGH);
      digitalWrite(SEG_C, HIGH);
      digitalWrite(SEG_D, LOW);
      digitalWrite(SEG_E, LOW);
      digitalWrite(SEG_F, LOW);
      digitalWrite(SEG_G, LOW);
      break;
    case 2:
      digitalWrite(SEG_A, HIGH);
      digitalWrite(SEG_B, HIGH);
      digitalWrite(SEG_C, LOW);
      digitalWrite(SEG_D, HIGH);
      digitalWrite(SEG_E, LOW);
      digitalWrite(SEG_F, HIGH);
      digitalWrite(SEG_G, LOW);
      break;
    case 1:
      digitalWrite(SEG_A, LOW);
      digitalWrite(SEG_B, LOW);
      digitalWrite(SEG_C, HIGH);
      digitalWrite(SEG_D, LOW);
      digitalWrite(SEG_E, LOW);
      digitalWrite(SEG_F, LOW);
      digitalWrite(SEG_G, LOW);
      break;
    case 0:
      digitalWrite(SEG_A, HIGH);
      digitalWrite(SEG_B, HIGH);
      digitalWrite(SEG_C, HIGH);
      digitalWrite(SEG_D, HIGH);
      digitalWrite(SEG_E, HIGH);
      digitalWrite(SEG_F, HIGH);
      digitalWrite(SEG_G, LOW);
      break;
  }
}

void updateLEDs() {
  // Update LED indicators based on locker states
  for (int i = 0; i < 3; i++) {
    if (lockerState[i]) {
      digitalWrite(RED_LED1_PIN + i * 2, HIGH);  // Turn on red LED
      digitalWrite(GREEN_LED1_PIN + i * 2, LOW); // Turn off green LED
    } else {
      digitalWrite(RED_LED1_PIN + i * 2, LOW);   // Turn off red LED
      digitalWrite(GREEN_LED1_PIN + i * 2, HIGH);// Turn on green LED
    }
  }
}

void alertUnauthorizedAccess() {
  // Trigger GSM alert
  gsm.println("Unauthorized access attempt detected!");
  lcd.clear();
  lcd.print("Alert Sent!");
  delay(2000); // Display alert message for 2 seconds
}
