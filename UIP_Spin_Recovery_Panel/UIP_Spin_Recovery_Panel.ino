#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

/* paste code snippets from the reference documentation here */

//Edited by Ulukaii - COM Port: 43 (reminder)
DcsBios::Potentiometer hmdOffBrt("HMD_OFF_BRT", A3);
DcsBios::Switch3Pos irCoolSw("IR_COOL_SW", A2, 2);
DcsBios::SwitchWithCover2Pos spinRecoverySw("SPIN_RECOVERY_SW","SPIN_RECOVERY_COVER", 3);

//add for R EWI
DcsBios::SwitchWithCover2Pos leftFireBtn("RIGHT_FIRE_BTN", "RIGHT_FIRE_BTN_COVER", 8);
DcsBios::Switch2Pos apuFireBtn("APU_FIRE_BTN", 9);


void setup() {
  DcsBios::setup();
  //Serial.begin(250000);
}

void loop() {
  DcsBios::loop();
  //Serial.println("Arduino is working and sending data");
  //delay(1000);
}

