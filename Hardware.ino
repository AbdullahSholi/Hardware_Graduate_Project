#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo

// Defines the LCD's parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(33, 32, 35, 34, 37, 36);


// For water pump 1 (Juice 1)
int enA1 = 9;  // Enable pin for motor driver 1 (PWM pin)
int in1_1 = 53;  // Control pin 1 for motor driver 1
int in2_1 = 52;  // Control pin 2 for motor driver 1

// For water pump 2 (Juice 2)
int enA2 = 8;  // Enable pin for motor driver 2 (PWM pin)
int in1_2 = 50;  // Control pin 1 for motor driver 2
int in2_2 = 51;  // Control pin 2 for motor driver 2

// For water pump 3 (Juice 3)
int enA3 = 10;  // Enable pin for motor driver 3 (PWM pin)
int in1_3 = 48;  // Control pin 1 for motor driver 3
int in2_3 = 49;  // Control pin 2 for motor driver 3

// IR sensor setup
const int irSensorPin = 31;  // IR sensor pin

int sensorValue1 = 0;
int flag = 0;
int flag1 = 0;

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
volatile int flow_frequency1 = 0; // Measures flow sensor 1 pulses
volatile int flow_frequency2 = 0; // Measures flow sensor 2 pulses
volatile int flow_frequency3 = 0; // Measures flow sensor 3 pulses
unsigned int l_hour = 0; // Calculated litres/hour
float totalMilliLitres = 0; // Total amount of water flowed through (mL)

// Flow sensor pins
const int flowSensorPin1 = 11; // Digital pin 11 for flow sensor 1
const int flowSensorPin2 = 12; // Digital pin 12 for flow sensor 2
const int flowSensorPin3 = 13; // Digital pin 13 for flow sensor 3

// Additional DC motor setup for post-IR sensor detection
int dcMotorEnablePin = 2; // Enable pin for DC motor
int dcMotorInterruptPin1 = 46; // Interrupt pin 1 for DC motor
int dcMotorInterruptPin2 = 47; // Interrupt pin 2 for DC motor


// Additional DC motor setup for post-IR sensor detection
int dcMotorEnablePinYellow = 4; // Enable pin for DC motor
int dcMotorInterruptPinYellow1 = 38; // Interrupt pin 1 for DC motor
int dcMotorInterruptPinYellow2 = 39; // Interrupt pin 2 for DC motor


void setup() {
  delay(200);
  Serial.begin(9600);

  myservo.attach(3);  // attaches the servo on pin 9 to the servo object

  pinMode(flowSensorPin1, INPUT);
  digitalWrite(flowSensorPin1, HIGH);

  pinMode(flowSensorPin2, INPUT);
  digitalWrite(flowSensorPin2, HIGH);

  pinMode(flowSensorPin3, INPUT);
  digitalWrite(flowSensorPin3, HIGH);

  pinMode(irSensorPin, INPUT);

  // Initialize motor 1 off
  pinMode(in1_1, OUTPUT);
  pinMode(in2_1, OUTPUT);
  pinMode(enA1, OUTPUT);
  digitalWrite(in1_1, LOW);
  digitalWrite(in2_1, LOW);
  analogWrite(enA1, 0);

  // Initialize motor 2 off
  pinMode(in1_2, OUTPUT);
  pinMode(in2_2, OUTPUT);
  pinMode(enA2, OUTPUT);
  digitalWrite(in1_2, LOW);
  digitalWrite(in2_2, LOW);
  analogWrite(enA2, 0);

  // Initialize motor 3 off
  pinMode(in1_3, OUTPUT);
  pinMode(in2_3, OUTPUT);
  pinMode(enA3, OUTPUT);
  digitalWrite(in1_3, LOW);
  digitalWrite(in2_3, LOW);
  analogWrite(enA3, 0);

  // Initialize additional DC motor off
  pinMode(dcMotorEnablePin, OUTPUT);
  pinMode(dcMotorInterruptPin1, OUTPUT);
  pinMode(dcMotorInterruptPin2, OUTPUT);
  digitalWrite(dcMotorInterruptPin1, LOW);
  digitalWrite(dcMotorInterruptPin2, LOW);
  analogWrite(dcMotorEnablePin, 255);

  // Initialize additional DC motor off
  pinMode(dcMotorEnablePinYellow, OUTPUT);
  pinMode(dcMotorInterruptPinYellow1, OUTPUT);
  pinMode(dcMotorInterruptPinYellow2, OUTPUT);
  digitalWrite(dcMotorInterruptPinYellow1, LOW);
  digitalWrite(dcMotorInterruptPinYellow2, LOW);
  analogWrite(dcMotorEnablePinYellow, 0);



  Serial.println("Hello Board A2");

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Welcome to Juice!!");
  delay(2000); // Wait for 2 seconds
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Glass size:");
  lcd.setCursor(0, 1);
  lcd.print("1)330ml 2)250ml");
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
      lcd.print("Selected: 330ml");
      volume = 330;
      inputVolume = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Selected: ");
      lcd.print(volume);
      lcd.print("ml");
      
      enterPercentages(volume);

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
      
      enterPercentages(volume);
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
}

void enterPercentages(unsigned int volume) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter % Ju1:");
  int percentage1 = enterPercentage();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter % Ju2:");
  int percentage2 = enterPercentage();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter % Ju3:");
  int percentage3 = enterPercentage();

  int totalPercentage = percentage1 + percentage2 + percentage3;
  if (totalPercentage != 100) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Invalid % sum");
    delay(2000);
    return;
  }

  unsigned int volume1 = (volume * percentage1) / 100;
  unsigned int volume2 = (volume * percentage2) / 100;
  unsigned int volume3 = (volume * percentage3) / 100;

  fillVolume(volume1, "Juice 1", flowSensorPin1, in1_1, in2_1, enA1);
  fillVolume(volume2, "Juice 2", flowSensorPin2, in1_2, in2_2, enA2);
  fillVolume(volume3, "Juice 3", flowSensorPin3, in1_3, in2_3, enA3);

  digitalWrite(dcMotorInterruptPin1, LOW);
  digitalWrite(dcMotorInterruptPin2, LOW);
  analogWrite(dcMotorEnablePin, 0);

  // Start IR sensor detection after filling the last juice
  startIrSensor();
}

int enterPercentage() {
  int percentage = 0;
  int digitCount = 0;
  while (digitCount < 2) {
    char key = keypad.getKey();
    if (key >= '0' && key <= '9') {
      percentage = percentage * 10 + (key - '0');
      lcd.setCursor(digitCount, 1);
      lcd.print(key);
      digitCount++;
    }
  }
  return percentage;
}

void fillVolume(unsigned int volume, const char* juiceType, int flowSensorPin, int in1, int in2, int enA) {
  float liters = volume / 1000.0; // Convert milliliters to liters

  int flow_frequency = 0; // Reset the flow frequency count
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
  lcd.print(juiceType);
  lcd.print(" complete");
  delay(2000);

  // Display final total milliliters flowed
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total ml:");
  lcd.setCursor(0, 1);
  lcd.print(totalMilliLitres); // Display as milliliters
}


void startIrSensor() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting for cup");
  while(true){
  sensorValue1 = digitalRead(irSensorPin);  // Read the sensor value (HIGH or LOW)
    if(sensorValue1 == 0){
      flag++;
      if(flag1 == 0){
      Serial.print("Sensor Value: ");
      Serial.println(sensorValue1);  // Print the sensor value to the serial monitor
      // delay(4000);  // Wait for half a second before the next reading
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cup detected!");
      digitalWrite(dcMotorInterruptPin1, LOW);
      digitalWrite(dcMotorInterruptPin2, LOW);
      analogWrite(dcMotorEnablePin, 0);

      // delay(2000);
      
      digitalWrite(dcMotorInterruptPinYellow1, LOW);
      digitalWrite(dcMotorInterruptPinYellow2, HIGH);
      analogWrite(dcMotorEnablePinYellow, 170);
      delay(18000);

      digitalWrite(dcMotorInterruptPinYellow1, LOW);
      digitalWrite(dcMotorInterruptPinYellow2, LOW);
      analogWrite(dcMotorEnablePinYellow, 0);

      // Here Servo Motor Code for mix
      // Rotate the servo clockwise
      for (int i = 0; i < 50; i++) {
        myservo.write(0);  // rotate servo to 0 degrees
        delay(500);   // wait for 100 milliseconds
        myservo.write(180);  // rotate servo to 180 degrees
        delay(500);    // wait for 100 milliseconds
      }

      // delay(10000);



      digitalWrite(dcMotorInterruptPinYellow1, HIGH);
      digitalWrite(dcMotorInterruptPinYellow2, LOW);
      analogWrite(dcMotorEnablePinYellow, 170);
      delay(18000);
      
      digitalWrite(dcMotorInterruptPinYellow1, LOW);
      digitalWrite(dcMotorInterruptPinYellow2, LOW);
      analogWrite(dcMotorEnablePinYellow, 0);
      flag1++;
      }
      
      if(flag1 != 0){
        Serial.println("Stop Motor");
      }

  } else{
    if(flag == 0){
      Serial.print("Sensor Value: ");
      Serial.println(sensorValue1);  // Print the sensor value to the serial monitor
      // delay(4000);  // Wait for half a second before the next reading
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cup Not detected!");

      // Turn on the DC motor
      digitalWrite(dcMotorInterruptPin1, HIGH);
      digitalWrite(dcMotorInterruptPin2, LOW);
      analogWrite(dcMotorEnablePin, 255);  // Adjust the speed as needed (0-255)
    }
    
  }
  }


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Glass size:");
  lcd.setCursor(0, 1);
  lcd.print("1)330ml 2)250ml");
}


