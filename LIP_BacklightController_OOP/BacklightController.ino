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

    void setGlobalBacklight(uint32_t color) {
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

// Instantiate panels, and add them to their respective channels. EDIT HERE if your bus layout is different!
// If you don't have a specific panel connected, it is sufficient to comment out the line. If you changed the order
// in which the panels are connected, you need to change the order of the .addPanel() commands, too!

// Example Panel Instantiations
// Panel masterArmPanel(0); // First panel on a bus starting with index 0 and defaulting to 10 LEDs
// Panel spinRecoveryPanel(5); // Second panel, starting after Master Arm panel, defaulting to 10 LEDs

// Add panels to channels
// LIP_BL_CH1.addPanel(masterArmPanel);
// LIP_BL_CH1.addPanel(spinRecoveryPanel);

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


// Loop through all channels and set the backlight to PanelGreen.
    LIP_BL_CH1.setGlobalBacklight(PanelGreen);
    LIP_BL_CH2.setGlobalBacklight(PanelGreen);
    UIP_BL_CH1.setGlobalBacklight(PanelGreen);
    UIP_BL_CH2.setGlobalBacklight(PanelGreen);
    LC_BL_CH1.setGlobalBacklight(PanelGreen);
    LC_BL_CH2.setGlobalBacklight(PanelGreen);
    RC_BL_CH1.setGlobalBacklight(PanelGreen);
    RC_BL_CH2.setGlobalBacklight(PanelGreen);
    BL_CH9.setGlobalBacklight(PanelGreen);
    BL_CH10.setGlobalBacklight(PanelGreen);
  
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
