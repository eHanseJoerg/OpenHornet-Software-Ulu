/********************************************************************************************************************
 * @brief Setup code that includes the core libraries and defines the constants to be used.
 *
 ********************************************************************************************************************/

// Check the Arduino board processor and select the proper DCS BIOS interface
#if defined(__AVR_ATmega328P__) ||  defined(__AVR_ATmega2560__)
  #define DCSBIOS_IRQ_SERIAL
#else
  #define DCSBIOS_DEFAULT_SERIAL
#endif

//Make functions controlling power consumption available, if supported by the chipset
#ifdef __AVR__
 #include <avr/power.h>
#endif

//Include core libraries
#include "DcsBios.h"
#include <Adafruit_NeoPixel.h>

//Global constants for the indicator LEDs' colours.
uint32_t IndYellow = Adafruit_NeoPixel::Color(255, 255, 0);   //Yellow
uint32_t IndRed = Adafruit_NeoPixel::Color(255, 0, 0);        //Red
uint32_t IndGreen = Adafruit_NeoPixel::Color(0, 255, 0);      //Green
uint32_t IndWhite = Adafruit_NeoPixel::Color(40, 40, 30);     //White for Jett Station Select toggle light
uint32_t dark = Adafruit_NeoPixel::Color(0, 0, 0);            //No colour

/********************************************************************************************************************
 * @brief Setup code that defines the core classes Panel and Channel, and their methods. This is an object-
 * oriented approach that allows you to add/remove panels with low effort. It
 * also allows you to support differing versions of a panel (e.g., older versions or Mods). You don't need to change
 * anything here.
 *
 ********************************************************************************************************************/

// Define a generic Panel. A panel is a container for many LEDs. LEDs can be of two types: backlight and indicator.
// The specific panels (MasterArm, Spin, Hud, ...) inherit from this class.
class Panel {
public:
    Panel(int startIdx, int count = 10) {
        startIndex = startIdx;
        ledCount = count;
        // Populate LED indices and indicator status array. Default isIndicator = false in this baseClass.
        // This will then be overwritten by the specific panel implementations
        for (int i = 0; i < ledCount; ++i) {
            ledIndices.push_back(startIndex + i);
            isIndicator.push_back(false);  // Default to non-indicator LEDs
        }
    }

    int getStartIndex() const { return startIndex; }
    int getLedCount() const { return ledCount; }

    void adjustBacklight(uint32_t color, Channel& channel) {
        channel.setPanelBacklight(*this, color);
    }

    std::vector<int> getBacklights() const {
        std::vector<int> backlightIndices;
        for (size_t i = 0; i < ledIndices.size(); ++i) {
            if (!isIndicator[i]) {
                backlightIndices.push_back(ledIndices[i]);
            }
        }
        return backlightIndices;
    }

protected:
    int startIndex;
    int ledCount;
    std::vector<int> ledIndices;     // Store LED indices
    std::vector<bool> isIndicator;   // Store whether each LED is an indicator
};

//Define a channel class to serve as a container for panels and interface with AdaFruit methods
class Channel {
public:
    Channel(int pin) : pin(pin) {
        strip = Adafruit_NeoPixel(0, pin, NEO_GRB + NEO_KHZ800); // Initialize the strip here with default values
    }
    
    void addPanel(const Panel& panel) {
        panels.push_back(panel);
        totalLedCount += panel.getLedCount();
        strip.updateLength(totalLedCount);  // Update the length of the NeoPixel strip to match the total LEDs
    }

    void setPanelBacklight(const Panel& panel, uint32_t color) {
        std::vector<int> backlightIndices = panel.getBacklights();
        for (size_t i = 0; i < backlightIndices.size(); ++i) {
            strip.setPixelColor(backlightIndices[i], color);
        }
        strip.show();  
    }  

    void setChannelBacklight(uint32_t color) {
        for (auto& panel : panels) {
            panel.adjustBacklight(color, *this);
        }
    }

    Adafruit_NeoPixel strip;  // Made public to access from the Panel class

private:
    int pin;
    std::vector<Panel> panels;
    int totalLedCount = 0;
};

class MasterArm : public Panel {
public:
    // Constructor: Initialize the Master Arm panel with a specific number of LEDs, starting at a certain index.
    MasterArm(int startIdx, int count = 10) : Panel(startIdx, count) {
        // Specify which LEDs are indicators for Master Arm.
        // Assuming LEDs 0 and 1 are indicators for arming status.
        for (int i = 0; i < ledCount; ++i) {
            if (i == 0 || i == 1) {
                isIndicator[i] = true;  // Set these LEDs as indicators
            }
        }
    }

    // Example method to activate Master Arm status indicator
    void setArmStatus(bool isArmed, Channel& channel) {
        uint32_t color = isArmed ? IndRed : dark;
        std::vector<int> armIndices;
        for (size_t i = 0; i < ledIndices.size(); ++i) {
            if (isIndicator[i]) {
                armIndices.push_back(ledIndices[i]);
            }
        }
        channel.setPanelBacklight(*this, color); // Set color for the Master Arm indicators
    }

private:
    #define READY_START 0 + startIdx
    #define READY_COUNT 2
    #define DISCH_START 2 + startIdx
    #define DISCH_COUNT 2
    #define MM_AG_START 25 + startIdx
    #define MM_AG_COUNT 2
    #define MM_AA_START 27 + startIdx
    #define MM_AA_COUNT 2
    #define BL_START 4 + startIdx
    #define BL_LENGTH 21

    void onMcReadyChange(unsigned int newValue) {
      switch (newValue) {
        case 0:
          uipBLCh1.fill(0, READY_START, READY_COUNT);  ///< Off
          uipBLCh1.show();
          break;
        case 1:
          uipBLCh1.fill(uipBLCh1.Color(255, 255, 0), READY_START, READY_COUNT);  ///< Set light to Yellow
          uipBLCh1.show();
          break;
      }
    }
    DcsBios::IntegerBuffer mcReadyBuffer(0x740c, 0x8000, 15, onMcReadyChange);
    void onMcDischChange(unsigned int newValue) {
      switch (newValue) {
        case 0:
          uipBLCh1.fill(0, DISCH_START, DISCH_COUNT);  ///< Off
          uipBLCh1.show();
          break;
        case 1:
          uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), DISCH_START, DISCH_COUNT);  ///< Set light to Green
          uipBLCh1.show();
          break;
      }
    }
    DcsBios::IntegerBuffer mcDischBuffer(0x740c, 0x4000, 14, onMcDischChange);
    void onMasterModeAgLtChange(unsigned int newValue) {
      switch (newValue) {
        case 0:
          uipBLCh1.fill(0, MM_AG_START, MM_AG_COUNT);  ///< Off
          uipBLCh1.show();
          break;
        case 1:
          uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), MM_AG_START, MM_AG_COUNT);  ///< Set light to Green
          uipBLCh1.show();
          break;
      }
    }
    DcsBios::IntegerBuffer masterModeAgLtBuffer(0x740c, 0x0400, 10, onMasterModeAgLtChange);
    void onMasterModeAaLtChange(unsigned int newValue) {
      switch (newValue) {
        case 0:
          uipBLCh1.fill(0, MM_AA_START, MM_AA_COUNT);  ///< Off
          uipBLCh1.show();
          break;
        case 1:
          uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), MM_AA_START, MM_AA_COUNT);  ///< Set light to Green
          uipBLCh1.show();
          break;
      }
    }
    DcsBios::IntegerBuffer masterModeAaLtBuffer(0x740c, 0x0200, 9, onMasterModeAaLtChange);
    void onInstPnlDimmerChange(unsigned int newValue) {
      switch (newValue) {
        case 0:
          uipBLCh1.fill(0, MASTER_ARM_LED_START_INDEX + BL_START, BL_LENGTH);  ///< Off
          uipBLCh1.show();
          break;
        default:
          uipBLCh1.fill(uipBLCh1.Color(0, map(newValue, 0, 65535, 0, 255), 0), MASTER_ARM_LED_START_INDEX + BL_START, BL_LENGTH);  ///< Set light to Green
          uipBLCh1.show();
          break;
      }
    }
    DcsBios::IntegerBuffer instPnlDimmerBuffer(0x7546, 0xffff, 0, onInstPnlDimmerChange);


};








/********************************************************************************************************************
 * @brief Runtime code starts here.
 *
 ********************************************************************************************************************/

// Instantiate channels, respecting the Pinout according OH interconnect
Channel LIP_BL_CH1(13);
Channel LIP_BL_CH2(12);
Channel UIP_BL_CH1(11);
Channel UIP_BL_CH2(10);
Channel LC_BL_CH1(9);
Channel LC_BL_CH2(8);
Channel RC_BL_CH1(7);
Channel RC_BL_CH2(6);
Channel BL_CH9(5);
Channel BL_CH10(4);

// EDIT HERE
// The next lines instantiate panel objects. Comment out those lines that you have not implemented.



// Example Panel Instantiations
// Panel masterArmPanel(0); // First panel on a bus starting with index 0 and defaulting to 10 LEDs
// Panel spinRecoveryPanel(5); // Second panel, starting after Master Arm panel, defaulting to 10 LEDs



// Add panels to channels
// LIP_BL_CH1.addPanel(masterArmPanel);
// LIP_BL_CH1.addPanel(spinRecoveryPanel);



//Global function to control instrument panel backlighting
void onInstrIntLtChange(unsigned int newValueInstrIntLt) {          
  uint8_t brightness = map(newValueInstrIntLt, 0, 65535, 0, 255);  //Set panel BL brightness according to value in DCS
  uint32_t PanelGreen = LC_1.Color(0, 100 * brightness / 255, 0);  //Set panel BL color: RGB LEDs
  //uint32_t GaugeGreen = LC_1.Color(100 * brightness / 255, 0, 0);  //Set gauges BL color: GRB LEDs (Adafruit 1938)

// Loop through all channel instances and set the backlight to PanelGreen.
    LIP_BL_CH1.setChannelBacklight(PanelGreen);
    LIP_BL_CH2.setChannelBacklight(PanelGreen);
    UIP_BL_CH1.setChannelBacklight(PanelGreen);
    UIP_BL_CH2.setChannelBacklight(PanelGreen);
    LC_BL_CH1.setChannelBacklight(PanelGreen);
    LC_BL_CH2.setChannelBacklight(PanelGreen);
    RC_BL_CH1.setChannelBacklight(PanelGreen);
    RC_BL_CH2.setChannelBacklight(PanelGreen);
    BL_CH9.setChannelBacklight(PanelGreen);
    BL_CH10.setChannelBacklight(PanelGreen);
  
}
DcsBios::IntegerBuffer instrIntLtBuffer(0x7560, 0xffff, 0, onInstrIntLtChange);


void setup() {
    // Initialize DCS BIOS
    DcsBios::setup();

    // Initialize channels
    LIP_BL_CH1.strip.begin();
    LIP_BL_CH2.strip.begin();
    UIP_BL_CH1.strip.begin();
    UIP_BL_CH2.strip.begin();
    LC_BL_CH1.strip.begin();
    LC_BL_CH2.strip.begin();
    RC_BL_CH1.strip.begin();
    RC_BL_CH2.strip.begin();
    BL_CH9.strip.begin();
    BL_CH10.strip.begin();
    
    // Set an initial brightness level
    LIP_BL_CH1.setGlobalBacklight(80);
    LIP_BL_CH2.setGlobalBacklight(80);
    UIP_BL_CH1.setGlobalBacklight(80);
    UIP_BL_CH2.setGlobalBacklight(80);
    LC_BL_CH1.setGlobalBacklight(80);
    LC_BL_CH2.setGlobalBacklight(80);
    RC_BL_CH1.setGlobalBacklight(80);
    RC_BL_CH2.setGlobalBacklight(80);
    BL_CH9.setGlobalBacklight(80);
    BL_CH10.setGlobalBacklight(80);  

  
}

void loop() {
    // Handle DCS BIOS updates
    DcsBios::loop();
    
    // Additional logic for LED updates, rotary encoder inputs, etc.
}
