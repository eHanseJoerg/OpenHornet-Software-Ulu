#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

/* paste code snippets from the reference documentation here */

//Edited by Ulukaii - COM Port: 51 (temporarily)
DcsBios::Switch2Pos auxRelSw("AUX_REL_SW", 9);
DcsBios::Switch3Pos cmsdDispenseSw("CMSD_DISPENSE_SW", 8, 10);
DcsBios::Switch2Pos cmsdJetSelBtn("CMSD_JET_SEL_BTN", 15);
const byte ecmModeSwPins[5] = {4, 6, 14, 7, 16};
DcsBios::SwitchMultiPos ecmModeSw("ECM_MODE_SW", ecmModeSwPins, 5);


//DcsBios::Potentiometer hmdOffBrt("HMD_OFF_BRT", A3);
//DcsBios::Switch3Pos irCoolSw("IR_COOL_SW", 2, A2);
//DcsBios::Switch2Pos spinRecoverySw("SPIN_RECOVERY_SW", 16);

void setup() {
  DcsBios::setup();
  //Serial.begin(250000);
}

void loop() {
  DcsBios::loop();
  //Serial.println("Arduino is working and sending data");
  //delay(1000);
}

