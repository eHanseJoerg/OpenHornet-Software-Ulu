/**************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___/|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *
 *
 *   Project OpenHornet
 *
 *   The OpenHornet Project is a F/A-18C OFP 13C Lot 20 1:1 Replica Simulator,
 *   consisting of a physical structure and electrical/software interfaces to a PC
 *   to be driven by Digital Combat Simulator (DCS).
 *
 *   ---------------------------------------------------------------------------------
 *
 *   This Project is released under the Creative Commons
 *   Atribution - Non Comercal - Share Alike License.
 *
 *   CC BY-NC-SA 3.0
 *
 *   You are free to:
 *   - Share — copy and redistribute the material in any medium or format
 *   - Adapt — remix, transform, and build upon the material
 *   The licensor cannot revoke these freedoms as long as you follow the license terms.
 *
 *   Under the following terms:
 *   - Attribution — You must give appropriate credit, provide a link to the license,
 *     and indicate if changes were made. You may do so in any reasonable manner,
 *     but not in any way that suggests the licensor endorses you or your use.
 *   - NonCommercial — You may not use the material for commercial purposes.
 *   - ShareAlike — If you remix, transform, or build upon the material,
 *     you must distribute your contributions under the same license as the original.
 *
 *   No additional restrictions — You may not apply legal terms or technological
 *   measures that legally restrict others from doing anything the license permits.
 *
 *   More Information about the license can be found under:
 *   https://creativecommons.org/licenses/by-nc-sa/3.0/
 *
 *   ---------------------------------------------------------------------------------
 *
 *   The OpenHornet Software is based on DCS-BIOS
 *   You can find more information here: http://dcs-bios.a10c.de
 *
 *   DCS-BIOS is released under the following terms:
 *   https://github.com/dcs-bios/dcs-bios/blob/develop/DCS-BIOS-License.txt
 * 
 *   Use only the following DCS-BIOS Arduino Library
 *   https://github.com/DCSFlightpanels/dcs-bios-arduino-library
 *
 *   ---------------------------------------------------------------------------------
 *
 *   This Project uses Doxygen as a documentation generator.
 *   Please use Doxigen capable comments.
 *
 **************************************************************************************/

/**
 * @file OH_LIP_BL_controller.ino
 * @author Higgins
 * @date <01.09.2024>
 * @version 0.0.1
 * @warning This code is for HUD PANEL v.2 (56 instead of 50 LEDs), SPIN RCVY PANEL v2.1 (63 LEDs, Spin indicator two non-subsequent LEDs)
 * and ANT SEL v2 (13 LEDs).
 * @brief Backlight controller
 * Not implemented yet: 
 * - Logic function to check whether DCS is running
 * - WARN/CAUT brightness
 * - Rotary encoder on SIM PWR PANEL
 *
 * Due to the code's length the sections are separated by highly visible headers.
 *
 * @details 
 * 
 *  * **Intended Board:**
 * ABSIS BACKLIGHT CONTROLLER
 * 
 *  * ** Processor **
 *  Arduino Mega + Backlight Shield
 */

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface and if on a Sparkfun Pro Micro use Default Serial
 * 
 */
#if defined(__AVR_ATmega328P__) ||  defined(__AVR_ATmega2560__)
  #define DCSBIOS_IRQ_SERIAL
#else
  #define DCSBIOS_DEFAULT_SERIAL
#endif

#ifdef __AVR__
 #include <avr/power.h> 
#endif

/**
 * DCS Bios library include
 */
#include "DcsBios.h"
//#include <CtrlEnc.h>
#include <Adafruit_NeoPixel.h>

/**
 * @brief BL channels are correct, the written number of pixels might exceed the actual number.
 * 
 */
const uint8_t LIP_BL_CH1  =    13;
const uint8_t LIP_BL_CH1_COUNT  =    100;
const uint8_t LIP_BL_CH2  =    12;
const uint8_t LIP_BL_CH2_COUNT  =    120;
const uint8_t UIP_BL_CH1  =    11;
const uint8_t UIP_BL_CH1_COUNT  =    208;
const uint8_t UIP_BL_CH2  =    10;
const uint8_t UIP_BL_CH2_COUNT  =    200;
const uint8_t LC_BL_CH1   =     9;
const uint8_t LC_BL_CH1_COUNT   =     233;
const uint8_t LC_BL_CH2   =     8;
const uint8_t LC_BL_CH2_COUNT   =     250;
const uint8_t RC_BL_CH1   =     7;
const uint8_t RC_BL_CH1_COUNT   =     200;
const uint8_t RC_BL_CH2   =     6;
const uint16_t RC_BL_CH2_COUNT   =     380;
const uint8_t BL_CH9      =     5;
const uint8_t BL_CH10     =     4;

// Define the pins for the brightness encoder on Sim Pwr Panel, create encoder object
const int pinA = 22;  // Pin A of the encoder
const int pinB = 23;  // Pin B of the encoder
const int pinSw1 = 24;  // Pin for the switch



//setup of the BL channels
Adafruit_NeoPixel LIP_1 = Adafruit_NeoPixel(LIP_BL_CH1_COUNT, LIP_BL_CH1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel LIP_2 = Adafruit_NeoPixel(LIP_BL_CH2_COUNT, LIP_BL_CH2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel UIP_1 = Adafruit_NeoPixel(UIP_BL_CH1_COUNT, UIP_BL_CH1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel LC_1 = Adafruit_NeoPixel(LC_BL_CH1_COUNT, LC_BL_CH1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel LC_2 = Adafruit_NeoPixel(LC_BL_CH2_COUNT, LC_BL_CH2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel RC_1 = Adafruit_NeoPixel(RC_BL_CH1_COUNT, RC_BL_CH1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel RC_2 = Adafruit_NeoPixel(RC_BL_CH2_COUNT, RC_BL_CH2, NEO_GRB + NEO_KHZ800);



/********************************************************************************************************************
 * @brief Global variables for the indicator LEDs' colours. 
 * @remark If indicators are not tinted and the colour is too bright reduce the values here.
 ********************************************************************************************************************/
//RGB LEDs
  uint32_t IndYellow = LC_1.Color(128, 128, 0);   //Yellow Indicators
  uint32_t IndRed = LC_1.Color(128, 0, 0);        //Red Indicators
  uint32_t IndGreen = LC_1.Color(0, 128, 0);      //Green Indicators
  uint32_t IndWhite = LC_1.Color(40, 40, 30);     //White for Jett Station Select toggle light
  uint32_t dark = LC_1.Color(0, 0, 0);            //No colour
  uint32_t IndYellow2 = LC_1.Color(32, 32, 0);   //Yellow Indicators
  uint32_t IndRed2 = LC_1.Color(32, 0, 0);        //Red Indicators
  uint32_t IndGreen2 = LC_1.Color(0, 32, 0);      //Green Indicators





/********************************************************************************************************************
 * @brief Instrument Panels Backlight
 * @warning This code is for a HUD PANEL with 56 LEDs (v2) and a SPIN RCVY PANEL with the indicator's LEDs at D31 and D34 (v2.1).
 * 
 ********************************************************************************************************************/
void onInstrIntLtChange(unsigned int newValueInstrIntLt) {          //Instrument brightness according to value in DCS
  uint8_t brightness = map(newValueInstrIntLt, 0, 65535, 0, 255); 
//COLOURS: Change values here for Instrument panels backlight.
  //RGB LEDs
  uint32_t PanelGreen = LC_1.Color(0, 100 * brightness / 255, 0);  //Panel backlight colour
  //GRB LEDs (Rad Alt and Stdby Instruments with Adafruit 1938)
  uint32_t GaugeGreen = LC_1.Color(100 * brightness / 255, 0, 0);    //Gauges GRB backlight colour

// UIP panels backlight
  //MASTER ARM panel backlight
    for (int i = 4; i < 25; i++)  {                
    UIP_1.setPixelColor(i, PanelGreen);      
    }
  //HUD panel backlight
    //for (int i = 59; i < 115; i++)  {              
    //UIP_1.setPixelColor(i, PanelGreen);      
    //}
  //SPIN RECOVERY panel backlight
    //for (int i = 145; i < 174; i++)  {            //SPIN RECOVERY panel backlight 1/3
    //UIP_1.setPixelColor(i, PanelGreen);      
    //  }
    //  for (int i = 175; i < 181; i++)  {          //SPIN RECOVERY panel backlight 2/3
    //  UIP_1.setPixelColor(i, PanelGreen);    
    //  }
    //  for (int i = 182; i < 208; i++)  {          //SPIN RECOVERY panel backlight 3/3
    //  UIP_1.setPixelColor(i, PanelGreen);    
    //}
  //SPIN RECOVERY panel backlight
    for (int i = 89; i < 118; i++)  {            //SPIN RECOVERY panel backlight 1/3
    UIP_1.setPixelColor(i, PanelGreen);      
      }
      for (int i = 119; i < 125; i++)  {          //SPIN RECOVERY panel backlight 2/3
      UIP_1.setPixelColor(i, PanelGreen);    
      }
      for (int i = 126; i < 149; i++)  {          //SPIN RECOVERY panel backlight 3/3
      UIP_1.setPixelColor(i, PanelGreen);    
    }

// LIP panels backlight
  //IFEI backlight
    for (int i = 0; i < 39; i++)  {                                      
    LIP_1.setPixelColor(i, PanelGreen);      
    }
  //VID RECORD backlight
    for (int i = 39; i < 55; i++)  {                                      
    LIP_1.setPixelColor(i, PanelGreen);      
    }
  //JETT SELECT panel backlight
    LIP_1.setPixelColor(56, PanelGreen);          //RO backlight 1/2
    LIP_1.setPixelColor(57, PanelGreen);          //RO backlight 2/2
    LIP_1.setPixelColor(60, PanelGreen);          //RI backlight 1/2
    LIP_1.setPixelColor(61, PanelGreen);          //RI backlight 2/2
    LIP_1.setPixelColor(64, PanelGreen);          //CTR backlight 1/2
    LIP_1.setPixelColor(65, PanelGreen);          //CTR backlight 2/2
    LIP_1.setPixelColor(67, PanelGreen);          //LI backlight 1/2
    LIP_1.setPixelColor(70, PanelGreen);          //LI backlight 2/2
    LIP_1.setPixelColor(71, PanelGreen);          //LO backlight 1/2
    LIP_1.setPixelColor(74, PanelGreen);          //LO backlight 2/2
  //JETT STATION SELECT PLACARD backlight
    for (int i = 87; i < 95; i++)  {                                      
    LIP_1.setPixelColor(i, PanelGreen);      
    }
  //ECM JETT PANEL backlight
    for (int i = 0; i < 74; i++)  {                                       
    LIP_2.setPixelColor(i, PanelGreen);
    }
  //RWR backlight
    for (int i = 78; i < 85; i++)  {         //RWR backlight 1/2
    LIP_2.setPixelColor(i, PanelGreen);
    }
      for (int i = 105; i < 111; i++)  {     //RWR backlight 2/2
      LIP_2.setPixelColor(i, PanelGreen);
    }
    //RWR ALR-67 POWER backlight
      LIP_2.setPixelColor(101, PanelGreen);  //RWR ALR-67 POWER backlight 1/2  
      LIP_2.setPixelColor(104, PanelGreen);  //RWR ALR-67 POWER backlight 1/2 
  //Standby Instruments backlight
    for (int i = 111; i < 117; i++)  {                                    
    LIP_2.setPixelColor(i, GaugeGreen);
    }
// L AUX panels backlight
  //LANDING GEAR panel backlight
    for (int i = 0; i < 23; i++)  {
    LC_1.setPixelColor(i, PanelGreen);
    }
  //SELECT JETT panel backlight
    for (int i = 23; i < 104; i++)  {
    LC_1.setPixelColor(i, PanelGreen);
    }
// R AUX panels backlight
  //LDG CHKLST panel backlight
    for (int i = 0; i < 24; i++)  {                                       
    RC_1.setPixelColor(i, PanelGreen);        
    }
  //RADAR ALTIMETER backlight
    for (int i = 24; i < 26; i++)  {         //@warning: GRB LEDs                             
    RC_1.setPixelColor(i, GaugeGreen);        
    }
  //HYD PRESS gauge backlight
    for (int i = 26; i < 68; i++)  {                                      
    RC_1.setPixelColor(i, PanelGreen);        
    }
  //AV COOL panel backlight
    for (int i = 92; i < 105; i++)  {                                     
    RC_1.setPixelColor(i, PanelGreen);        
    }

LIP_1.show();
LIP_2.show(); 
UIP_1.show();
LC_1.show();
RC_1.show();  
}
DcsBios::IntegerBuffer instrIntLtBuffer(0x7560, 0xffff, 0, onInstrIntLtChange);

/********************************************************************************************************************
 * @brief CONSOLES backlights
 * 
 * 
 ********************************************************************************************************************/
void onConsolesDimmerChange(unsigned int newValueConsolesDimmer) {
  uint8_t brightness = map(newValueConsolesDimmer, 0, 65535, 0, 255); 


//COLOUR: Change values here for Instrument panels backlight.
  uint32_t PanelGreen = LC_1.Color(0, 100 * brightness / 255, 0);  //Panel backlight colour

//LEFT CONSOLE panels backlight
  // FIRE TEST panel backlight
    for (int i = 104; i < 114; i++)  {
    LC_1.setPixelColor(i, PanelGreen);
    }
  //SIM PWR panel backlight
    for (int i = 114; i < 149; i++)  {
    LC_1.setPixelColor(i, PanelGreen);
    }
  //GEN TIE panel backlight
    for (int i = 149; i < 158; i++)  {
    LC_1.setPixelColor(i, PanelGreen);
    }
  //EXT LTS panel backlight
    for (int i = 158; i < 202; i++)  {
    LC_1.setPixelColor(i, PanelGreen);
    }
  //FUEL panel backlight
    for (int i = 202; i < 234; i++)  {
    LC_1.setPixelColor(i, PanelGreen);
    }
  //APU panel backlight
    for (int i = 0; i < 20; i++)  {
    LC_2.setPixelColor(i, PanelGreen);
    }
  //FCS panel backlight
    for (int i = 20; i < 43; i++)  {
    LC_2.setPixelColor(i, PanelGreen);
    }
  //COMM panel backlight
    for (int i = 43; i < 152; i++)  {
    LC_2.setPixelColor(i, PanelGreen);
    }
  //ANT SEL panel backlight
    for (int i = 152; i < 165; i++)  {                   // @warning: (here v2(13 LEDs); rev4 has 17)
    LC_2.setPixelColor(i, PanelGreen);
    }
  //OBOGS panel backlight
    for (int i = 165; i < 181; i++)  {
    LC_2.setPixelColor(i, PanelGreen);
    }
  //MC/HYD ISOL panel backlight
    for (int i = 181; i < 205; i++)  {
    LC_2.setPixelColor(i, PanelGreen);
    }
//RIGHT CONSOLE panels backlight
  //ELEC panel backlight
    for (int i = 105; i < 128; i++)  {
    RC_1.setPixelColor(i, PanelGreen);
    }
  //BATT GAUGE backlight
    for (int i = 128; i < 174; i++)  {
    RC_1.setPixelColor(i, PanelGreen);
    }
  //ECS panel backlight
    for (int i = 0; i < 63; i++)  {
    RC_2.setPixelColor(i, PanelGreen);
    }
  //DEFOG panel backlight
    for (int i = 63; i < 85; i++)  {
    RC_2.setPixelColor(i, PanelGreen);
    }
  //INTR LT panel backlight
    for (int i = 85; i < 150; i++)  {
    RC_2.setPixelColor(i, PanelGreen);
    }
  //SNSR panel backlight
    for (int i = 150; i < 208; i++)  {
    RC_2.setPixelColor(i, PanelGreen);
    }
  //SIM CTRL panel backlight
    for (int i = 208; i < 269; i++)  {
    RC_2.setPixelColor(i, PanelGreen);
    }
  //KY-58 panel backlight
    for (int i = 269; i < 348; i++)  {
    RC_2.setPixelColor(i, PanelGreen);
    }

LC_1.show(); 
LC_2.show(); 
RC_1.show(); 
RC_2.show();
} 
DcsBios::IntegerBuffer consolesDimmerBuffer(0x7544, 0xffff, 0, onConsolesDimmerChange);

/********************************************************************************************************************
 * @brief UIP indicators
 * 
 ********************************************************************************************************************/
//MASTER ARM panel indicators
  //A/A indicator
    void onMasterModeAaLtChange(unsigned int newValueAa) {
    if (newValueAa == 1) {                                
      for (int i = 27; i < 29; i++)  {   
      UIP_1.setPixelColor(i, IndGreen);
      }
      UIP_1.show(); 
    } else {
      for (int i = 27; i < 29; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer masterModeAaLtBuffer(0x740c, 0x0200, 9, onMasterModeAaLtChange);

  //A/G indicator
    void onMasterModeAgLtChange(unsigned int newValueAg) {
    if (newValueAg == 1) {                                
      for (int i = 25; i < 27; i++)  {   
      UIP_1.setPixelColor(i, IndGreen);
      }
      UIP_1.show(); 
    } else {
      for (int i = 25; i < 27; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer masterModeAgLtBuffer(0x740c, 0x0400, 10, onMasterModeAgLtChange);

  //DISCHARGE indicator
    void onMcDischChange(unsigned int newValueMcDisch)  {
    if (newValueMcDisch == 1) {                                
      for (int i = 2; i < 4; i++)  {    
      UIP_1.setPixelColor(i, IndGreen);
      }
      UIP_1.show(); 
    } else {
      for (int i = 2; i < 4; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer mcDischBuffer(0x740c, 0x4000, 14, onMcDischChange);


  //MCREADY indicator
    void onMcReadyChange(unsigned int newValueMcReady)  {
    if (newValueMcReady == 1) {                               
      for (int i = 0; i < 2; i++)  {    
      UIP_1.setPixelColor(i, IndYellow);
      }
      UIP_1.show(); 
    } else {
      for (int i = 0; i < 2; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer mcReadyBuffer(0x740c, 0x8000, 15, onMcReadyChange);

    
//EWI LEFT indicators
  //FIRE LEFT indicator
    void onFireLeftLtChange(unsigned int newValueFireLeftLt)  {
    if (newValueFireLeftLt == 1) {                               
      for (int i = 29; i < 33; i++)  {   
      UIP_1.setPixelColor(i, IndRed);
      }
      UIP_1.show(); 
    } else {
      for (int i = 29; i < 33; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer fireLeftLtBuffer(0x7408, 0x0040, 6, onFireLeftLtChange);

  //MASTER CAUTION indicator
    void onMasterCautionLtChange(unsigned int newValueMasterCautionLt)  {
    if (newValueMasterCautionLt == 1) {                          
      for (int i = 33; i < 37; i++)  { 
      UIP_1.setPixelColor(i, IndYellow);
      }
      UIP_1.show(); 
    } else {
      for (int i = 33; i < 37; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer masterCautionLtBuffer(0x7408, 0x0200, 9, onMasterCautionLtChange);

  //GO indicator
    void onLhAdvGoChange(unsigned int newValueLhAdvGo)  {
    if (newValueLhAdvGo == 1) {                                 
      for (int i = 37; i < 39; i++)  {    
      UIP_1.setPixelColor(i, IndGreen2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 37; i < 39; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer lhAdvGoBuffer(0x740a, 0x0010, 4, onLhAdvGoChange);

  //NO-GO indicator
    void onLhAdvNoGoChange(unsigned int newValueLhAdvNoGo)  {
    if (newValueLhAdvNoGo == 1) {                                     
      for (int i = 39; i < 41; i++)  {    
      UIP_1.setPixelColor(i, IndYellow2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 39; i < 41; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer lhAdvNoGoBuffer(0x740a, 0x0020, 5, onLhAdvNoGoChange);

  //R_BLEED indicator
    void onLhAdvRBleedChange(unsigned int newValueLhAdvRBleed)  {
    if (newValueLhAdvRBleed == 1) {                                 
      for (int i = 41; i < 43; i++)  {    
      UIP_1.setPixelColor(i, IndRed2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 41; i < 43; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer lhAdvRBleedBuffer(0x7408, 0x1000, 12, onLhAdvRBleedChange);

  //L_BLEED indicator
    void onLhAdvLBleedChange(unsigned int newValueLhAdvLBleed)  {
    if (newValueLhAdvLBleed == 1) {                                
      for (int i = 43; i < 45; i++)  {   
      UIP_1.setPixelColor(i, IndRed2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 43; i < 45; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer lhAdvLBleedBuffer(0x7408, 0x0800, 11, onLhAdvLBleedChange);

  //Speed Brake indicator
    void onLhAdvSpdBrkChange(unsigned int newValueLhAdvSpdBrk)  {
    if (newValueLhAdvSpdBrk == 1) {                              
      for (int i = 45; i < 47; i++)  {    
      UIP_1.setPixelColor(i, IndGreen2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 45; i < 47; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer lhAdvSpdBrkBuffer(0x7408, 0x2000, 13, onLhAdvSpdBrkChange);

  //STBY indicator
    void onLhAdvStbyChange(unsigned int newValueLhAdvStby)  {
    if (newValueLhAdvStby == 1) {                               
      for (int i = 47; i < 49; i++)  {    
      UIP_1.setPixelColor(i, IndYellow2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 47; i < 49; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer lhAdvStbyBuffer(0x7408, 0x4000, 14, onLhAdvStbyChange);


  //REC indicator
    void onLhAdvRecChange(unsigned int newValueLhAdvRec)  {
    if (newValueLhAdvRec == 1) {                              
      for (int i = 49; i < 51; i++)  {    
      UIP_1.setPixelColor(i, IndYellow2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 49; i < 51; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer lhAdvRecBuffer(0x740a, 0x0001, 0, onLhAdvRecChange);

  //LBAR red indicator
    void onLhAdvLBarRedChange(unsigned int newValueLhAdvLBarRed)  {
    if (newValueLhAdvLBarRed == 1) {                            
      for (int i = 51; i < 53; i++)  {   
      UIP_1.setPixelColor(i, IndRed2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 51; i < 53; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer lhAdvLBarRedBuffer(0x7408, 0x8000, 15, onLhAdvLBarRedChange);

  //LBAR green indicator
    void onLhAdvLBarGreenChange(unsigned int newValueLhAdvLBarGreen)  {
    if (newValueLhAdvLBarGreen == 1) {                               
      for (int i = 53; i < 55; i++)  {   
      UIP_1.setPixelColor(i, IndRed2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 53; i < 55; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer lhAdvLBarGreenBuffer(0x740a, 0x0002, 1, onLhAdvLBarGreenChange);

  //XMIT indicator
    void onLhAdvXmitChange(unsigned int newValueLhAdvXmit)  {
    if (newValueLhAdvXmit == 1) {                                        
      for (int i = 55; i < 57; i++)  {   
      UIP_1.setPixelColor(i, IndRed2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 55; i < 57; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer lhAdvXmitBuffer(0x740a, 0x0004, 2, onLhAdvXmitChange);

  //ASPJ indicator
    void onLhAdvAspjOhChange(unsigned int newValueLhAdvAspjOh)  {
    if (newValueLhAdvAspjOh == 1) {                                  
      for (int i = 57; i < 59; i++)  {   
      UIP_1.setPixelColor(i, IndRed2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 57; i < 59; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer lhAdvAspjOhBuffer(0x740a, 0x0008, 3, onLhAdvAspjOhChange);


//EWI RIGHT
  //FIRE RIGHT indicator
    void onFireRightLtChange(unsigned int newValueFireRightLt)  {
    if (newValueFireRightLt == 1) {                                   
      for (int i = 59; i < 63; i++)  {    
      UIP_1.setPixelColor(i, IndRed);
      }
      UIP_1.show(); 
    } else {
      for (int i = 59; i < 63; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer fireRightLtBuffer(0x740c, 0x0010, 4, onFireRightLtChange);

  //APU FIRE indicator
    void onFireApuLtChange(unsigned int newValueFireApuLt)  {
    if (newValueFireApuLt == 1) {                                      
      for (int i = 63; i < 67; i++)  {    
      UIP_1.setPixelColor(i, IndRed);
      }
      UIP_1.show(); 
    } else {
      for (int i = 63; i < 67; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer fireApuLtBuffer(0x740c, 0x0004, 2, onFireApuLtChange);

  //DISP indicator
    void onRhAdvDispChange(unsigned int newValueRhAdvDisp)  {
    if (newValueRhAdvDisp == 1) {                                      
      for (int i = 67; i < 69; i++)  {    
      UIP_1.setPixelColor(i, IndGreen);
      }
      UIP_1.show(); 
    } else {
      for (int i = 67; i < 69; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer rhAdvDispBuffer(0x740a, 0x0100, 8, onRhAdvDispChange);

  //RCDR indicator
    void onRhAdvRcdrOnChange(unsigned int newValueRhAdvRcdrOn)  {
    if (newValueRhAdvRcdrOn == 1) {                                        
      for (int i = 69; i < 71; i++)  {    
      UIP_1.setPixelColor(i, IndGreen2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 69; i < 71; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer rhAdvRcdrOnBuffer(0x740a, 0x0080, 7, onRhAdvRcdrOnChange);

  //Spare indicators
    void onRhAdvSpareRh1Change(unsigned int newValueRhAdvSpareRh1)  {
    if (newValueRhAdvSpareRh1 == 1) {         //toggles all spare indicators if the first of them is lit
      for (int i = 71; i < 81; i++)  {    
      UIP_1.setPixelColor(i, IndGreen2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 71; i < 81; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer rhAdvSpareRh1Buffer(0x740a, 0x2000, 13, onRhAdvSpareRh1Change);

  //SAM indicator
    void onRhAdvSamChange(unsigned int newValueRhAdvSam)  {
    if (newValueRhAdvSam == 1) {                                         
      for (int i = 81; i < 83; i++)  {    
      UIP_1.setPixelColor(i, IndRed2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 81; i < 83; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer rhAdvSamBuffer(0x740a, 0x0200, 9, onRhAdvSamChange);

  //AAA indicator
    void onRhAdvAaaChange(unsigned int newValueRhAdvAaa)  {
    if (newValueRhAdvAaa == 1) {                                           
      for (int i = 83; i < 85; i++)  {   
      UIP_1.setPixelColor(i, IndRed2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 83; i < 85; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer rhAdvAaaBuffer(0x740a, 0x0800, 11, onRhAdvAaaChange);

  //AI indicator
    void onRhAdvAiChange(unsigned int newValueRhAdvAi)  {
    if (newValueRhAdvAi == 1) {                                         
      for (int i = 85; i < 87; i++)  {    
      UIP_1.setPixelColor(i, IndRed2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 85; i < 87; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer rhAdvAiBuffer(0x740a, 0x0400, 10, onRhAdvAiChange);

  //CW indicator
    void onRhAdvCwChange(unsigned int newValueRhAdvCw)  {
    if (newValueRhAdvCw == 1) {                                          
      for (int i = 87; i < 89; i++)  {    
      UIP_1.setPixelColor(i, IndRed2);
      }
      UIP_1.show(); 
    } else {
      for (int i = 87; i < 89; i++)  {   
      UIP_1.setPixelColor(i, dark);
      }
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer rhAdvCwBuffer(0x740a, 0x1000, 12, onRhAdvCwChange);


//SPIN RCVY panel indicators
  //SPIN indicator
    //void onSpinLtChange(unsigned int newValueSpinLt)  {
    //if (newValueSpinLt == 1) {                                           
      //UIP_1.setPixelColor(174, IndYellow);
      //UIP_1.setPixelColor(181, IndYellow);
      //UIP_1.show(); 
    //} else {
      //UIP_1.setPixelColor(174, dark);
      //UIP_1.setPixelColor(181, dark);
      //UIP_1.show(); 
      //}
    //}
    //DcsBios::IntegerBuffer spinLtBuffer(0x742a, 0x0800, 11, onSpinLtChange);
    
    void onSpinLtChange(unsigned int newValueSpinLt)  {
    if (newValueSpinLt == 1) {                                           
      UIP_1.setPixelColor(118, IndRed);
      UIP_1.setPixelColor(125, IndRed);
      UIP_1.show();
    } else {
      UIP_1.setPixelColor(118, dark);
      UIP_1.setPixelColor(125, dark);
      UIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer spinLtBuffer(0x742a, 0x0800, 11, onSpinLtChange);



/********************************************************************************************************************
 * @brief LIP indicators
 * Station Jettison Select indicators use a dark white colour to better distinguish toggled state due to lightbleeding from backlight.
 ********************************************************************************************************************/
//STATION JETTISON SELECT panel indicators
  //RO toggle indicator
    void onSjRoLtChange(unsigned int newValueSjRoLt)  {                  
	  if (newValueSjRoLt == 1) {                                            
      LIP_1.setPixelColor(55, IndWhite);
      LIP_1.setPixelColor(58, IndWhite);
      LIP_1.show(); 
      } else {
      LIP_1.setPixelColor(55, dark);
      LIP_1.setPixelColor(58, dark);
      LIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer sjRoLtBuffer(0x7430, 0x0400, 10, onSjRoLtChange);

  //RI toggle indicator
    void onSjRiLtChange(unsigned int newValueSjRiLt)  {                         
	  if (newValueSjRiLt == 1) {                                            
      LIP_1.setPixelColor(59, IndWhite);
      LIP_1.setPixelColor(62, IndWhite);
      LIP_1.show(); 
    } else {
      LIP_1.setPixelColor(59, dark);
      LIP_1.setPixelColor(62, dark);
      LIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer sjRiLtBuffer(0x7430, 0x0200, 9, onSjRiLtChange);

  //CTR toggle indicator
    void onSjCtrLtChange(unsigned int newValueSjCtrLt)  {                         
	  if (newValueSjCtrLt == 1) {                                            
      LIP_1.setPixelColor(63, IndWhite);
      LIP_1.setPixelColor(66, IndWhite);
      LIP_1.show(); 
    } else {
      LIP_1.setPixelColor(63, dark);
      LIP_1.setPixelColor(66, dark);
      LIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer sjCtrLtBuffer(0x742e, 0x4000, 14, onSjCtrLtChange);
  
  //LI toggle indicator
    void onSjLiLtChange(unsigned int newValueSjLiLt)  {                        
	  if (newValueSjLiLt == 1) {                                            
      LIP_1.setPixelColor(68, IndWhite);
      LIP_1.setPixelColor(69, IndWhite);
      LIP_1.show(); 
    } else {
      LIP_1.setPixelColor(68, dark);
      LIP_1.setPixelColor(69, dark);
      LIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer sjLiLtBuffer(0x742e, 0x8000, 15, onSjLiLtChange);

  //LO toggle indicator
    void onSjLoLtChange(unsigned int newValueSjLoLt)  {                         
	  if (newValueSjLoLt == 1) {                                            
      LIP_1.setPixelColor(72, IndWhite);
      LIP_1.setPixelColor(73, IndWhite);
      LIP_1.show(); 
    } else {
      LIP_1.setPixelColor(72, dark);
      LIP_1.setPixelColor(73, dark);
      LIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer sjLoLtBuffer(0x7430, 0x0100, 8, onSjLoLtChange);

//STORES INDICATOR panel
  //NOSE indicator
    void onFlpLgNoseGearLtChange(unsigned int newValueFlpLgNoseGearLt)  {      
	  if (newValueFlpLgNoseGearLt == 1) {                                            
      LIP_1.setPixelColor(75, IndGreen);
      LIP_1.setPixelColor(86, IndGreen);
      LIP_1.show(); 
    } else {
      LIP_1.setPixelColor(75, dark);
      LIP_1.setPixelColor(86, dark);
      LIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer flpLgNoseGearLtBuffer(0x7430, 0x0800, 11, onFlpLgNoseGearLtChange);

  //RIGHT indicator
    void onFlpLgRightGearLtChange(unsigned int newValueFlpLgRightGearLt)  {      
	  if (newValueFlpLgRightGearLt == 1) {                                            
      LIP_1.setPixelColor(76, IndGreen);
      LIP_1.setPixelColor(77, IndGreen);
      LIP_1.show(); 
    } else {
      LIP_1.setPixelColor(76, dark);
      LIP_1.setPixelColor(77, dark);
      LIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer flpLgRightGearLtBuffer(0x7430, 0x2000, 13, onFlpLgRightGearLtChange);

  //LEFT indicator
    void onFlpLgLeftGearLtChange(unsigned int newValueFlpLgLeftGearLt)  {      
	  if (newValueFlpLgLeftGearLt == 1) {                                            
      LIP_1.setPixelColor(78, IndGreen);
      LIP_1.setPixelColor(79, IndGreen);
      LIP_1.show(); 
    } else {
      LIP_1.setPixelColor(78, dark);
      LIP_1.setPixelColor(79, dark);
      LIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer flpLgLeftGearLtBuffer(0x7430, 0x1000, 12, onFlpLgLeftGearLtChange);

  //HALF indicator
    void onFlpLgHalfFlapsLtChange(unsigned int newValueFlpLgHalfFlapsLt)  {      
	  if (newValueFlpLgHalfFlapsLt == 1) {                                            
      LIP_1.setPixelColor(80, IndGreen);
      LIP_1.setPixelColor(81, IndGreen);
      LIP_1.show(); 
    } else {
      LIP_1.setPixelColor(80, dark);
      LIP_1.setPixelColor(81, dark);
      LIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer flpLgHalfFlapsLtBuffer(0x7430, 0x4000, 14, onFlpLgHalfFlapsLtChange);

  //FULL indicator
    void onFlpLgFullFlapsLtChange(unsigned int newValueFlpLgFullFlapsLt)  {      
	  if (newValueFlpLgFullFlapsLt == 1) {                                            
      LIP_1.setPixelColor(82, IndGreen);
      LIP_1.setPixelColor(83, IndGreen);
      LIP_1.show(); 
    } else {
      LIP_1.setPixelColor(82, dark);
      LIP_1.setPixelColor(83, dark);
      LIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer flpLgFullFlapsLtBuffer(0x7430, 0x8000, 15, onFlpLgFullFlapsLtChange);

  //FLAPS indicator
    void onFlpLgFlapsLtChange(unsigned int newValueFlpLgFlapsLt)  {               
	  if (newValueFlpLgFlapsLt == 1) {                                            
      LIP_1.setPixelColor(84, IndYellow);
      LIP_1.setPixelColor(85, IndYellow);
      LIP_1.show(); 
    } else {
      LIP_1.setPixelColor(84, dark);
      LIP_1.setPixelColor(85, dark);
      LIP_1.show(); 
      }
    }
    DcsBios::IntegerBuffer flpLgFlapsLtBuffer(0x7466, 0x0001, 0, onFlpLgFlapsLtChange);

//ECM-DISP panel indicators
  //ECM JETT SEL indicator
    void onCmsdJetSelLChange(unsigned int newValueCmsdJetSelL) {
    if (newValueCmsdJetSelL == 1) {                                            
      LIP_2.setPixelColor(74, IndYellow);
      LIP_2.setPixelColor(78, IndYellow);
      LIP_2.show(); 
    } else {
      LIP_2.setPixelColor(74, dark);
      LIP_2.setPixelColor(78, dark);
      LIP_2.show(); 
      }
    }
    DcsBios::IntegerBuffer cmsdJetSelLBuffer(0x74d4, 0x8000, 15, onCmsdJetSelLChange);

//RWR indicators
  //RWR BIT indicator
  void onRwrBitLtChange(unsigned int newValueRwrBitLt) {
    if (newValueRwrBitLt == 1) {                                            
      LIP_2.setPixelColor(85, IndGreen);
      LIP_2.setPixelColor(88, IndGreen);
      LIP_2.show(); 
    } else {
      LIP_2.setPixelColor(85, dark);
      LIP_2.setPixelColor(88, dark);
      LIP_2.show(); 
      }
    }
    DcsBios::IntegerBuffer rwrBitLtBuffer(0x749c, 0x1000, 12, onRwrBitLtChange);

  //RWR FAIL indicator
    void onRwrFailLtChange(unsigned int newValueRwrFailLt) {
    if (newValueRwrFailLt == 1) {                                            
      LIP_2.setPixelColor(86, IndRed);
      LIP_2.setPixelColor(87, IndRed);
      LIP_2.show(); 
    } else {
      LIP_2.setPixelColor(86, dark);
      LIP_2.setPixelColor(87, dark);
      LIP_2.show(); 
      }
    }
    DcsBios::IntegerBuffer rwrFailLtBuffer(0x749c, 0x0800, 11, onRwrFailLtChange);

  //RWR OFFSET indicator
    void onRwrOffsetLtChange(unsigned int newValueRwrOffsetLt) {
    if (newValueRwrOffsetLt == 1) {                                            
      LIP_2.setPixelColor(89, IndGreen);
      LIP_2.setPixelColor(92, IndGreen);
      LIP_2.show(); 
    } else {
      LIP_2.setPixelColor(89, dark);
      LIP_2.setPixelColor(92, dark);
      LIP_2.show(); 
      }
    }
    DcsBios::IntegerBuffer rwrOffsetLtBuffer(0x749c, 0x0400, 10, onRwrOffsetLtChange);
    
  //RWR OFFSET-ENABLE indicator
    void onRwrEnableLtChange(unsigned int newValueRwrEnableLt) {
    if (newValueRwrEnableLt == 1) {                                            
      LIP_2.setPixelColor(90, IndGreen);
      LIP_2.setPixelColor(91, IndGreen);
      LIP_2.show(); 
    } else {
      LIP_2.setPixelColor(90, dark);
      LIP_2.setPixelColor(91, dark);
      LIP_2.show(); 
      }
    }
    DcsBios::IntegerBuffer rwrEnableLtBuffer(0x749c, 0x0200, 9, onRwrEnableLtChange);
    
  //RWR SPECIAL indicator
    void onRwrSpecialLtChange(unsigned int newValueRwrSpecialLt) {
    if (newValueRwrSpecialLt == 1) {                                            
      LIP_2.setPixelColor(93, IndGreen);
      LIP_2.setPixelColor(96, IndGreen);
      LIP_2.show(); 
    } else {
      LIP_2.setPixelColor(93, dark);
      LIP_2.setPixelColor(96, dark);
      LIP_2.show(); 
      }
    }
    DcsBios::IntegerBuffer rwrSpecialLtBuffer(0x749c, 0x0100, 8, onRwrSpecialLtChange);

  //RWR SPECIAL-ENABLE indicator
    void onRwrSpecialEnLtChange(unsigned int newValueRwrSpecialEnLt) {
    if (newValueRwrSpecialEnLt == 1) {                                            
      LIP_2.setPixelColor(94, IndGreen);
      LIP_2.setPixelColor(95, IndGreen);
      LIP_2.show(); 
    } else {
      LIP_2.setPixelColor(94, dark);
      LIP_2.setPixelColor(95, dark);
      LIP_2.show(); 
      }
    }
    DcsBios::IntegerBuffer rwrSpecialEnLtBuffer(0x7498, 0x8000, 15, onRwrSpecialEnLtChange);

  // RWR DISPLAY indicator
    void onRwrDisplayLtChange(unsigned int newValueRwrDisplayLt) {
    if (newValueRwrDisplayLt == 1) {                                            
      LIP_2.setPixelColor(97, IndGreen);
      LIP_2.setPixelColor(100, IndGreen);
      LIP_2.show(); 
    } else {
      LIP_2.setPixelColor(97, dark);
      LIP_2.setPixelColor(100, dark);
      LIP_2.show(); 
      }
    }
    DcsBios::IntegerBuffer rwrDisplayLtBuffer(0x7498, 0x4000, 14, onRwrDisplayLtChange);

  // RWR LIMIT indicator
    void onRwrLimitLtChange(unsigned int newValueRwrLimitLt) {
    if (newValueRwrLimitLt == 1) {                                            
      LIP_2.setPixelColor(98, IndGreen);
      LIP_2.setPixelColor(99, IndGreen);
      LIP_2.show(); 
    } else {
      LIP_2.setPixelColor(98, dark);
      LIP_2.setPixelColor(99, dark);
      LIP_2.show(); 
      }
    }
    DcsBios::IntegerBuffer rwrLimitLtBuffer(0x7498, 0x2000, 13, onRwrLimitLtChange);

  // RWR POWER ON indicator
    void onRwrLowerLtChange(unsigned int newValueRwrLowerLt) {
    if (newValueRwrLowerLt == 1) {                                            
      LIP_2.setPixelColor(102, IndGreen);
      LIP_2.setPixelColor(103, IndGreen);
      LIP_2.show(); 
    } else {
      LIP_2.setPixelColor(102, dark);
      LIP_2.setPixelColor(103, dark);
      LIP_2.show(); 
      }
    }
    DcsBios::IntegerBuffer rwrLowerLtBuffer(0x7498, 0x1000, 12, onRwrLowerLtChange);


/********************************************************************************************************************
 * @brief RC indicators
 * Ldg checklist: 0-23; Rad Alt: 24,25; Hyd Press: 26-67; Caution: 69-91; AVCool and more: 92-173
 ********************************************************************************************************************/
//Caution Panel
  //CK SEAT indicator
    void onClipCkSeatLtChange(unsigned int newValueCkSeat)  {
    if (newValueCkSeat == 1) {
      for (int i = 68; i < 70; i++)  {    
      RC_1.setPixelColor(i, IndYellow);
      }
      RC_1.show(); 
    } else {
      for (int i = 68; i < 70; i++)  {   
      RC_1.setPixelColor(i, dark);
      }
      RC_1.show(); 
      }
    }
    DcsBios::IntegerBuffer clipCkSeatLtBuffer(0x74a0, 0x8000, 15, onClipCkSeatLtChange);

  //APU ACC indicator
    void onClipApuAccLtChange(unsigned int newValueApuAcc)  {
    if (newValueApuAcc == 1) {
      for (int i = 70; i < 72; i++)  {    
      RC_1.setPixelColor(i, IndYellow);
      }
      RC_1.show(); 
    } else {
      for (int i = 70; i < 72; i++)  {   
      RC_1.setPixelColor(i, dark);
      }
      RC_1.show(); 
      }
    }
    DcsBios::IntegerBuffer clipApuAccLtBuffer(0x74a4, 0x0100, 8, onClipApuAccLtChange);

  //BATT SW indicator
    void onClipBattSwLtChange(unsigned int newValueBattSw)  {
    if (newValueBattSw == 1) {
      for (int i = 72; i < 74; i++)  {    
      RC_1.setPixelColor(i, IndYellow);
      }
      RC_1.show(); 
    } else {
      for (int i = 72; i < 74; i++)  {   
      RC_1.setPixelColor(i, dark);
      }
      RC_1.show(); 
      }
    }
    DcsBios::IntegerBuffer clipBattSwLtBuffer(0x74a4, 0x0200, 9, onClipBattSwLtChange);

  //Spare indicators, 1 triggers all
    void onClipSpareCtn1LtChange(unsigned int newValueSpareCtn1)  {
    if (newValueSpareCtn1 == 1) {            //all spares if Spare indicator 1 is lit
      for (int i = 74; i < 76; i++)  {    
      RC_1.setPixelColor(i, IndYellow);
      }
      for (int i = 84; i < 88; i++)  {    
      RC_1.setPixelColor(i, IndYellow);
      }   
      RC_1.show(); 
    } else {
      for (int i = 74; i < 76; i++)  {    
      RC_1.setPixelColor(i, dark);
      }
      for (int i = 84; i < 88; i++)  {    
      RC_1.setPixelColor(i, dark);
      }   
      RC_1.show(); 
      }
    }
    DcsBios::IntegerBuffer clipSpareCtn1LtBuffer(0x74a4, 0x1000, 12, onClipSpareCtn1LtChange);

  //GEN TIE indicator
    void onClipGenTieLtChange(unsigned int newValueGenTie)  {
    if (newValueGenTie == 1) {
      for (int i = 76; i < 78; i++)  {    
      RC_1.setPixelColor(i, IndYellow);
      }
      RC_1.show(); 
    } else {
      for (int i = 76; i < 78; i++)  {   
      RC_1.setPixelColor(i, dark);
      }
      RC_1.show(); 
      }
    }
    DcsBios::IntegerBuffer clipGenTieLtBuffer(0x74a4, 0x0800, 11, onClipGenTieLtChange);

  //FCS HOT indicator
    void onClipFcsHotLtChange(unsigned int newValueFcsHot)  {
    if (newValueFcsHot == 1) {
      for (int i = 78; i < 80; i++)  {    
      RC_1.setPixelColor(i, IndYellow);
      }
      RC_1.show(); 
    } else {
      for (int i = 78; i < 80; i++)  {   
      RC_1.setPixelColor(i, dark);
      }
      RC_1.show(); 
      }
    }
    DcsBios::IntegerBuffer clipFcsHotLtBuffer(0x74a4, 0x0400, 10, onClipFcsHotLtChange);

  //FUEL LO indicator
    void onClipFuelLoLtChange(unsigned int newValueFuelLo)  {
    if (newValueFuelLo == 1) {
      for (int i = 80; i < 82; i++)  {    
      RC_1.setPixelColor(i, IndYellow);
      }
      RC_1.show(); 
    } else {
      for (int i = 80; i < 82; i++)  {   
      RC_1.setPixelColor(i, dark);
      }
      RC_1.show(); 
      }
    }
    DcsBios::IntegerBuffer clipFuelLoLtBuffer(0x74a4, 0x2000, 13, onClipFuelLoLtChange);

  //FCES indicator
    void onClipFcesLtChange(unsigned int newValueFces)  {
    if (newValueFces == 1) {
      for (int i = 82; i < 84; i++)  {    
      RC_1.setPixelColor(i, IndYellow);
      }
      RC_1.show(); 
    } else {
      for (int i = 82; i < 84; i++)  {   
      RC_1.setPixelColor(i, dark);
      }
      RC_1.show(); 
      }
    }
    DcsBios::IntegerBuffer clipFcesLtBuffer(0x74a4, 0x4000, 14, onClipFcesLtChange);

  //R GEN indicator
    void onClipRGenLtChange(unsigned int newValueRGen)  {
    if (newValueRGen == 1) {
      for (int i = 88; i < 90; i++)  {    
      RC_1.setPixelColor(i, IndYellow);
      }
      RC_1.show(); 
    } else {
      for (int i = 88; i < 90; i++)  {   
      RC_1.setPixelColor(i, dark);
      }
      RC_1.show(); 
      }
    }
    DcsBios::IntegerBuffer clipRGenLtBuffer(0x74a8, 0x0200, 9, onClipRGenLtChange);

  //L GEN indicator
    void onClipLGenLtChange(unsigned int newValueLGen)  {
    if (newValueLGen == 1) {
      for (int i = 90; i < 92; i++)  {    
      RC_1.setPixelColor(i, IndYellow);
      }
      RC_1.show(); 
    } else {
      for (int i = 90; i < 92; i++)  {   
      RC_1.setPixelColor(i, dark);
      }
      RC_1.show(); 
      }
    }
    DcsBios::IntegerBuffer clipLGenLtBuffer(0x74a8, 0x0100, 8, onClipLGenLtChange);



//DcsBios::RotaryEncoder instPnlDimmer("INST_PNL_DIMMER", "DEC", "INC", 22, 23);
// Define an onTurnleft handler.
//void onTurnleft() {
  //long currentPosition = lastPosition -10;
  //sendDcsBiosMessage("INST_PNL_DIMMER", currentPosition);
  //lastPosition = currentPosition;
  // If the position has changed, set brightness to a new level
  //if (currentPosition != lastPosition) {
    //sendDcsBiosMessage("CONSOLES_DIMMER", currentPosition);
    //sendDcsBiosMessage("INST_PNL_DIMMER", currentPosition);
    //lastPosition = currentPosition;
  //}
//}

// Define an onTurnRight handler.
//void onTurnRight() {
  //int currentPosition = lastPosition +10;
  //sendDcsBiosMessage("INST_PNL_DIMMER", currentPosition);
//  lastPosition = currentPosition;
//}

// Create a rotary encoder with the clk signal pin number, dt signal pin number,
// onTurnleft & onTurnRight handler.
//CtrlEnc enc(pinA, pinB, onTurnleft, onTurnRight);

DcsBios::RotaryEncoder instPnlDimmer("INST_PNL_DIMMER", "-3200", "+3200", pinA, pinB);
DcsBios::RotaryEncoder consolesDimmer("CONSOLES_DIMMER", "-3200", "+3200", pinA, pinB);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

    // Set D22 and D23 as input pins
    //pinMode(22, INPUT);
    //pinMode(23, INPUT);
    //pinMode(pinSw1, INPUT_PULLUP);  // Set the switch pin to input with internal pull-up
    //enc.begin(pinA,pinB,18, pinSw1);

  //Run DCS Bios setup function
  DcsBios::setup();

  // INITIALIZE NeoPixel strip object (REQUIRED)
      LIP_1.begin();
      LIP_2.begin();
      UIP_1.begin();
      LC_1.begin();
      LC_2.begin();
      RC_1.begin();
      RC_2.begin();



  // Show the neopixel 
      LIP_1.show();
      LIP_2.show(); 
      UIP_1.show();
      LC_1.show(); 
      LC_2.show(); 
      RC_1.show(); 
      RC_2.show(); 

}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*/
void loop() {

  //Read the value of the encoder and adjust brightness accordingly
  //enc.process();

  
  // If switch is pressed, toggle between game and test mode
  //int sw1state = digitalRead(pinSw1);
  //if (sw1state == LOW) {  // Assuming the switch is active LOW
    //insert code here
    //delay(500);  // Debounce delay
  //}

 //Run DCS Bios loop function
 DcsBios::loop();
  
  }
