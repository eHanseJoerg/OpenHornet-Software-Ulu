#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

/* paste code snippets from the reference documentation here */

//Code von Ulukaii. Arduino an COM 39
DcsBios::Switch2Pos masterArmSw("MASTER_ARM_SW", 4);
DcsBios::Switch2Pos masterModeAa("MASTER_MODE_AA", 2);
DcsBios::Switch2Pos masterModeAg("MASTER_MODE_AG", 3);
DcsBios::Switch2Pos emerJettBtn("EMER_JETT_BTN", A2);                                       //non-std pinout vs. OH
DcsBios::Switch2Pos fireExtBtn("FIRE_EXT_BTN", A3);                                         //non-std pinout vs. OH
DcsBios::SwitchWithCover2Pos leftFireBtn("LEFT_FIRE_BTN", "LEFT_FIRE_BTN_COVER", 8);       // add pins for L EWI
DcsBios::Switch2Pos masterCautionResetSw("MASTER_CAUTION_RESET_SW", 9);                    // add pins for L EWI


void setup() {
  DcsBios::setup();
  //Serial.begin(250000);
}

void loop() {
  DcsBios::loop();
  //Serial.println("Arduino is working and sending data");
  //delay(1000);
}

