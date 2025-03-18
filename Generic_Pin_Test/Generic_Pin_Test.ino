// Store the previous state of each pin
bool previousState[24];

void setup() {
  // Start serial communication at 9600 baud
  Serial.begin(9600);
  
  // Initialize all digital pins as inputs with internal pull-up resistors
  for (int pin = 0; pin < 24; pin++) {
    pinMode(pin, INPUT_PULLUP);
    previousState[pin] = HIGH;  // Initial state assumes all pins are HIGH (OFF)
  }
  
  delay(1000); // Wait for the serial monitor to be ready
}

void loop() {
  // Loop through all the pins (0-23 for Pro Micro)
  for (int pin = 0; pin < 24; pin++) {
    // Read the current state of the pin
    int currentState = digitalRead(pin);
    
    // Check if the state has changed
    if (currentState != previousState[pin]) {
      if (currentState == LOW) {
        Serial.print("Pin ");
        Serial.print(pin);
        Serial.println(" is ON");
      } else {
        Serial.print("Pin ");
        Serial.print(pin);
        Serial.println(" is OFF");
      }
      
      // Update the previous state to the current state
      previousState[pin] = currentState;
    }
  }
  
  delay(100); // Short delay to avoid flooding the serial monitor
}
