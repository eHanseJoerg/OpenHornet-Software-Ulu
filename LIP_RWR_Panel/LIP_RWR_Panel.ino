#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

/* paste code snippets from the reference documentation here */

//Edited by Ulukaii - COM Port: 45


DcsBios::ActionButton rwrPowerBtnToggle("RWR_POWER_BTN", "TOGGLE", 2);
DcsBios::Switch2Pos rwrDisplayBtn("RWR_DISPLAY_BTN", 3);
DcsBios::Switch2Pos rwrSpecialBtn("RWR_SPECIAL_BTN", A1);
DcsBios::Switch2Pos rwrOffsetBtn("RWR_OFFSET_BTN", 4);
DcsBios::Switch2Pos rwrBitBtn("RWR_BIT_BTN", A0);


DcsBios::Potentiometer rwrAudioCtrl("RWR_AUDIO_CTRL", A3);
DcsBios::Potentiometer rwrDmrCtrl("RWR_DMR_CTRL", A2);

//DIS Type rotary switch - Non-OH-standard: RWR_DIS_I is connected on cable Pin 1 (Arduino Pin 15)
//  and the other positions are one index higher
const byte rwrDisTypeSwPins[5] = {16, 14, 6, 7, 15};


DcsBios::SwitchMultiPos rwrDisTypeSw("RWR_DIS_TYPE_SW", rwrDisTypeSwPins, 5);



//DcsBios::Switch2Pos auxRelSw("AUX_REL_SW", 9);
//DcsBios::Switch3Pos cmsdDispenseSw("CMSD_DISPENSE_SW", 8, 10);
//DcsBios::Switch2Pos cmsdJetSelBtn("CMSD_JET_SEL_BTN", 15);
//const byte ecmModeSwPins[5] = {4, 6, 14, 7, 16};
//DcsBios::SwitchMultiPos ecmModeSw("ECM_MODE_SW", ecmModeSwPins, 5);


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

