#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL ///< This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
#else
#define DCSBIOS_DEFAULT_SERIAL ///< This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)  
#endif

#ifdef __AVR__
 #include <avr/power.h> 
#endif

#include <Wire.h>
#include <MT6701.h>
#include <DcsBios.h>  

MT6701 encoder;
bool gearState = true;
bool inMiddlePosition = false;               // Flag to track if the lever is in the middle position
bool emerState = false;                      // Flag to track if the lever is in emerState
unsigned long lastTime = 0;                  // Last time the gear state was checked
unsigned long delayTime = 500;               // Half second delay (you can adjust this for responsiveness)
unsigned long middlePositionStartTime = 0;   // If left in middle position, EMER ROTATE will be triggered after a countdown


// DCS Bios PIN setups except for landing gear handle, which is using direct commands ("sendDcsBiosMessage")
DcsBios::LED landingGearHandleLt(FA_18C_hornet_LANDING_GEAR_HANDLE_LT_AM, 9);
DcsBios::Switch2Pos gearDownlockOverrideBtn("GEAR_DOWNLOCK_OVERRIDE_BTN", A2);
DcsBios::Switch2Pos gearSilenceBtn("GEAR_SILENCE_BTN", A3);

void setup() {
  Wire.begin();
  encoder.initializeI2C();
  DcsBios::setup();  
}

void loop() {
  float angle = encoder.angleRead();

  // Check if enough time has passed since the last action
  if (millis() - lastTime >= delayTime) {
    
    // Gear lever moving up (angle < 30.0)
    if (angle < 30.0 && gearState == 1) {
      if (emerState == 1) {
        // If previously in emergency state, reset it before moving gear
        sendDcsBiosMessage("EMERGENCY_GEAR_ROTATE", "0");  
        emerState = 0;
      }
      sendDcsBiosMessage("GEAR_LEVER", "1"); 
      gearState = 0;
      Serial.print(angle);  // Debug: print I2C output lever angle
      lastTime = millis();  // Update the last time for delay check
      inMiddlePosition = false;  // Reset middle position state
    }

    // Middle position for emergency gear operation (angle between 40.0 and 48.0)
    else if (angle >= 40.0 && angle <= 50.0) {
      if (!inMiddlePosition) {
        // First time entering the middle position, start the timer
        middlePositionStartTime = millis();
        inMiddlePosition = true;
        Serial.println("Entered middle position; countdown started");
      }
      
      // Check if the lever stays in the middle position for more than 2 seconds (3000ms)
      if (inMiddlePosition && millis() - middlePositionStartTime >= 3000) {
        if (emerState == 0) {
          // Trigger emergency gear rotate once 
          sendDcsBiosMessage("EMERGENCY_GEAR_ROTATE", "1");
          emerState = 1;  // Set emergency state to active
          Serial.println("Time elapsed. Emergency gear rotate triggered");
          lastTime = millis();  // Reset lastTime to respect delayTime
        }
      }
    }

    // Gear lever moving down (angle > 50.0)
    else if (angle > 50.0 && gearState == 0) {
      sendDcsBiosMessage("GEAR_LEVER", "0"); 
      gearState = 1;
      Serial.print(angle);  // Debug: print I2C output lever angle
      lastTime = millis();  // Update the last time for delay check
      inMiddlePosition = false;  // Reset middle position state
    }
  }

  DcsBios::loop();  // Update DCS BIOS
}
