// Array to store pin states
const int numPins = 18;  // Arduino Micro has 20 digital pins, we use pins 2 to 19
int currentState[numPins];
int prevState[numPins];

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Initialize all digital pins (2 to 19) as input with internal pull-ups
  for (int i = 2; i <= 19; i++) {
    pinMode(i, INPUT_PULLUP);  // Use internal pull-up resistors
    currentState[i - 2] = digitalRead(i);  // Initialize current state
    prevState[i - 2] = currentState[i - 2];  // Initialize previous state
  }
}

void loop() {
  // Loop through all pins (2 to 19)
  for (int i = 2; i <= 19; i++) {
    currentState[i - 2] = digitalRead(i);  // Read the current state of the pin

    // Check if the pin state has changed
    if (currentState[i - 2] != prevState[i - 2]) {
      Serial.print("Pin ");
      Serial.print(i);
      Serial.print(" changed to ");
      Serial.println(currentState[i - 2] == HIGH ? "HIGH" : "LOW");
      prevState[i - 2] = currentState[i - 2];  // Update previous state
    }
  }

  delay(50);  // Small delay to reduce serial flooding
}
