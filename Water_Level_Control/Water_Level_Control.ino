/*
  Arduino IoT Cloud Sketch for Water Level Management System
  - Uses an ultrasonic sensor to measure the water level in a tank.
  - Displays the current level, setpoint, mode, and control status on an LCD.
  - Allows remote control of the pump and valve via the Arduino IoT Cloud.
  - Supports manual or automatic modes for managing water level.

  Automatically generated variables:
    - float level_SV;   // Setpoint level from the cloud
    - float sensor_PV;  // Process variable (current water level)
    - int mode_cloud;   // Mode control from the cloud (manual or auto)
    - bool pump;        // Pump control state
    - bool valve;       // Valve control state
*/

#include "thingProperties.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // LCD library for I2C communication

// Pin definitions
const int trigPin = 5;     // Ultrasonic sensor trigger pin
const int echoPin = 18;    // Ultrasonic sensor echo pin
const int buttonPin1 = 13; // Mode toggle button pin
const int buttonPin2 = 12; // Pump control button pin
const int buttonPin3 = 14; // Valve control button pin
const int pumppin = 15;    // Pump control output pin
const int valvepin = 2;    // Valve control output pin

// Variables for sensor and control logic
long duration;   // Time duration for ultrasonic pulse
int distance;    // Distance measured by ultrasonic sensor
long tanklvl;    // Calculated tank level percentage
int setpoint;    // Desired water level setpoint
int modebit;     // Mode indicator (manual or automatic)
boolean pumpbit; // Pump state in manual mode
boolean valvebit; // Valve state in manual mode

// Button state variables for debouncing
boolean oldval1, newval1, state1 = LOW;  // For button 1 (mode toggle)
boolean oldval2, newval2, state2 = LOW;  // For button 2 (pump control)
boolean oldval3, newval3, state3 = LOW;  // For button 3 (valve control)

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD at I2C address 0x27 with 16x2 display

void setup() {
  // Initialize pins for buttons and outputs
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(pumppin, OUTPUT);
  pinMode(valvepin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize LCD display
  lcd.init();
  lcd.backlight();

  // Initialize control variables
  level_SV = 50;  // Default setpoint level (50%)
  modebit = 1;    // Default mode (automatic)

  // Initialize Arduino IoT Cloud properties and connection
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // Set debug message level for IoT Cloud connection
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update(); // Update IoT Cloud properties and check for changes

  // Sync cloud variables with local variables
  pumpbit = pump;
  valvebit = valve;
  modebit = mode_cloud;
  
  // Display current mode on the LCD
  lcd.setCursor(0, 1);
  lcd.print("mode:");
  if (modebit == 2)
    lcd.print("OFA"); // Manual mode
  else
    lcd.print("ONA"); // Automatic mode

  // Manual mode: control pump and valve using buttons
  if (modebit == 2) {
    pumpbit = toggle2(); // Check pump control button
    valvebit = toggle3(); // Check valve control button
    boolean temp_mode = toggle1(); // Check mode toggle button
    if (temp_mode == LOW) {
      modebit = 0; // Switch to manual mode
    } else if (temp_mode == HIGH) {
      modebit = 1; // Switch to automatic mode
    } 
  }

  // Measure current water level
  int level = measurelvl();
  Serial.print(int(level));
  Serial.println("%");
  sensor_PV = level; // Update process variable with measured level
  setpoint = level_SV; // Set desired setpoint

  // Automatic control logic
  if (modebit == 0 && level > 30 && level < 85) {
    digitalWrite(pumppin, pumpbit); // Control pump state
    pump = pumpbit;
    digitalWrite(valvepin, valvebit); // Control valve state
    valve = valvebit;
  } 
  else if (modebit == 1 || level < 30 || level > 85) {
    modebit = 1; // Default to automatic mode
    if (level < setpoint - 2) {
      digitalWrite(pumppin, HIGH);  // Turn on pump
      pump = HIGH;
      digitalWrite(valvepin, LOW);  // Close valve
      valve = LOW;
    } else if (level > setpoint + 2) {
      digitalWrite(pumppin, LOW);   // Turn off pump
      pump = LOW;
      digitalWrite(valvepin, HIGH); // Open valve
      valve = HIGH;
    } else {
      digitalWrite(pumppin, LOW);   // Keep pump off
      pump = LOW;
      digitalWrite(valvepin, LOW);  // Keep valve closed
      valve = LOW;
    }
  }

  // Update LCD display with current status
  lcd.setCursor(0, 0);
  lcd.print("PV:");
  lcd.print(level);
  lcd.setCursor(10, 0);
  lcd.print("SP:");
  lcd.print(setpoint);
  lcd.setCursor(10, 1);
  lcd.print("ctrl:");
  if (modebit == 0)
    lcd.print("M");
  else
    lcd.print("A");

  delay(200); // Delay for stability
}

// Function to toggle mode using button 1
boolean toggle1() {
  newval1 = digitalRead(buttonPin1);
  if (oldval1 == 0 && newval1 == 1) {
    state1 = !state1; // Toggle state
  }
  oldval1 = newval1;
  delay(10); // Debounce delay
  return state1;
}

// Function to toggle pump control using button 2
boolean toggle2() {
  newval2 = digitalRead(buttonPin2);
  if (oldval2 == 0 && newval2 == 1) {
    state2 = !state2; // Toggle state
  }
  oldval2 = newval2;
  delay(10); // Debounce delay
  return state2;
}

// Function to toggle valve control using button 3
boolean toggle3() {
  newval3 = digitalRead(buttonPin3);
  if (oldval3 == 0 && newval3 == 1) {
    state3 = !state3; // Toggle state
  }
  oldval3 = newval3;
  delay(10); // Debounce delay
  return state3;
}

// Function to measure water level using ultrasonic sensor
int measurelvl() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);  // Measure the duration of the HIGH pulse
  distance = duration * 0.034 / 2;    // Calculate distance based on the pulse duration
  tanklvl = (399 - distance) / 4;     // Convert distance to a level percentage
  if (tanklvl > 99)                   // Ensure tank level does not exceed 99%
    tanklvl = 99;
  else if(tanklvl < 0)                // Ensure tank level does not drop below 0%
    tanklvl = 0;
  return tanklvl;                     // Return the calculated tank level
}
