#include <LiquidCrystal.h>
#include <Keypad.h>

// Defines the LCD's parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// For water pump
int enA = 9;  // Enable pin for motor driver (PWM pin)
int in1 = 53;  // Control pin 1 pin 10
int in2 = 52;  // Control pin 2  pin 8

// Keypad setup
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {22, 24, 26, 28}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {23, 25, 27, 29}; // connect to the column pinouts of the keypad

// Create the Keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Flow sensor setup
volatile int flow_frequency = 0; // Measures flow sensor pulses
unsigned int l_hour = 0; // Calculated litres/hour
float totalMilliLitres = 0; // Total amount of water flowed through (mL)
const int flowSensorPin = 11; // Digital pin 11

void setup() {
  delay(200);
  Serial.begin(9600);

  pinMode(flowSensorPin, INPUT);
  digitalWrite(flowSensorPin, HIGH);

  // Initialize motor off
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);

  Serial.println("Hello Board A2");

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Welcome to Juice!!");
  delay(2000); // Wait for 2 seconds
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Glass size:");
  lcd.setCursor(0, 1);
  lcd.print("1) Custom ml  2) 250ml");
}

void loop() {
  static boolean inputVolume = false;
  static unsigned int volume = 0; // Default volume is 0ml for custom input
  static int digitCount = 0; // Count the number of digits entered
  char key = keypad.getKey();

  if (key) {
    Serial.println(key);
    lcd.clear();
    if (key == '1') {
      lcd.setCursor(0, 0);
      lcd.print("Enter(ml):");
      inputVolume = true;
      volume = 0; // Reset to 0ml for custom input
      digitCount = 0; // Reset digit count
    } else if (key == '2') {
      lcd.setCursor(0, 0);
      lcd.print("Selected: 250ml");
      volume = 250;
      inputVolume = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Selected: ");
      lcd.print(volume);
      lcd.print("ml");
      fillVolume(volume);
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Invalid choice");
      delay(2000); // Display "Invalid choice" for 2 seconds
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Glass size:");
      lcd.setCursor(0, 1);
      lcd.print("1) Custom ml  2) 250ml");
    }
  }

  if (inputVolume) {
    char key = keypad.getKey();
    if (key >= '0' && key <= '9' && digitCount < 3) {
      lcd.print(key);
      volume = (volume * 10) + (key - '0'); // Accumulate digits
      digitCount++;
    } else if (key == 'D') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Selected: ");
      lcd.print(volume);
      lcd.print("ml");
      inputVolume = false;
      fillVolume(volume);
    }
  }
}

void fillVolume(unsigned int volume) {
  float liters = volume / 1000.0; // Convert milliliters to liters

  flow_frequency = 0; // Reset the flow frequency count
  totalMilliLitres = 0; // Reset total millilitres
  unsigned long startTime = millis();

  // Turn on the pump
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 255);  // Adjust the speed as needed (0-255)

  // Calculate the volume of water
  while (totalMilliLitres < volume) {
    if ((millis() - startTime) >= 1000) { // Every second
      startTime = millis();

      // Directly monitor flow sensor pin state change
      if (digitalRead(flowSensorPin) == HIGH) {
        flow_frequency++;
      }
      
      l_hour = 44; // (Pulse frequency x 60) / 7.5Q, Q = 7.5 (for YF-S201)
      totalMilliLitres += (l_hour * 1000 / 3600); // Convert liters/hour to milliliters/second

      // Update and display current milliliters on LCD
      lcd.setCursor(0, 1);
      lcd.print("Current ml: ");
      lcd.print(totalMilliLitres); // Display as milliliters
    }
  }

  // Turn off the pump
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);

  lcd.setCursor(0, 1);
  lcd.print("Filling complete");
  delay(2000);

  // Display final total milliliters flowed
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total ml:");
  lcd.setCursor(0, 1);
  lcd.print(totalMilliLitres); // Display as milliliters
}
