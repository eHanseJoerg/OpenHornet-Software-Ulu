int pin = 3;  // Pin you want to control

void setup() {
  pinMode(pin, OUTPUT);  // Set the pin to output mode
  Serial.begin(9600);    // Start serial communication
  Serial.println("Enter '1' to set the pin HIGH, '0' to set the pin LOW:");
}

void loop() {
  if (Serial.available() > 0) {
    char input = Serial.read();  // Read the incoming byte
    while (Serial.available() > 0) { // Clear any extra characters in the buffer
      Serial.read();
    }

    if (input == '1') {
      digitalWrite(pin, HIGH);  // Set the pin HIGH
      Serial.println("Pin is set HIGH");
    } 
    else if (input == '0') {
      digitalWrite(pin, LOW);  // Set the pin LOW
      Serial.println("Pin is set LOW");
    } 
    else {
      Serial.println("Invalid input! Enter '1' or '0'.");
    }
  }
}
