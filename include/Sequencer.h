#ifndef SEQUENCER_H
#define SEQUENCER_H
#include <Adafruit_NeoPixel.h>
#include <MCP_DAC.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "Sequence.h"
#include "OLEDLog.h"
#include "BootAnimation.h"
#include "TrackClearAnimation.h"
#include "ApplyPageAnimation.h"
#include <memory>


// Pin definitions based on 30-pin ESP32 Devkit V1 pinout
//Encoder pins
#define ADATA 34
#define ACLK 35
#define BDATA 32
#define BCLK 33
#define CDATA 25
#define CCLK 26
#define DDATA 27
#define DCLK 14

//Analog Button pins
#define BUTTONS1 36
#define BUTTONS2 39

//sequence inputs
#define CV_IN 21
#define TRIG_IN 13

//DAC/SPI pins
#define MOSI 23
#define SCK 18
#define DAC1 22
#define DAC2 19

//I2C pins
#define SDA 17
#define SCL 5

//gate outputs
#define GATE1 16
#define GATE2 4
#define GATE3 2
#define GATE4 15

//NeoPixels data line
#define PIXEL_PIN 18

//Other macros
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C

#define MIN_LOOP_PERIOD 1000

#define MAX_REFRESH_HZ 60

/*
    We have DACs with a max output of 3.3v being fed to an amplifier with a gain of 3.2, so the final range of the CV output is 0-10.56 volts.
    A halfstep corresponds to one twelfth of a volt or 83.33333mV. 
    Since the MCP4822 is a 12-bit DAC, we control it with a number between 0 and 4095 and therefore the increment is ~2.579mV.
    Therefore, a half-step is represented by a difference of about 32.3 in the input integer
*/

#define HALFSTEP_INCREMENT 32.3123f

//Corresponds to index passed by button library in main.cpp
enum ButtonId
{
    MenuL,
    MenuR,
    Play,
    Track1,
    Track2,
    Track3,
    Track4,
    E1,
    E2,
    E3,
    E4,
    PageR,
    PageL
};

class Sequencer
{
private:
    friend class SpiffsEnvironment;
    MCP4822 dac1;

    MCP4822 dac2;

    Adafruit_NeoPixel pixels;

    Adafruit_SSD1306 display;

    std::unique_ptr<Sequence> currentSequence;

    BootAnimation bootAnim;

    TrackClearAnimation trackClearAnim;

    ApplyPageAnimation applyPageAnim;

    static uint16_t levelForMidiNote(uint16_t note)
    {
        return note * HALFSTEP_INCREMENT;
    }
    void setLevelForTrack(uint8_t trk, uint16_t mV);
    void setStepPixel(byte idx, uint32_t color);
    void setPagePixel(byte idx, uint32_t color);
    void setTrackPixel(byte idx, uint32_t color);
    unsigned long lastUpdated = 0;
    unsigned long ledLastUpdated = 0;
//loop subroutines
    void updateLeds();
    void updateDACs();
    void updateGates();
    void updateDisplay();

    static void updateDAC (MCP4822* dac, uint16_t value, uint8_t channel=0);
public:
    Sequencer();
    void loop();
    void buttonPressed(uint8_t id);
    void buttonHeld(uint8_t id);
    void encoderTurned(uint8_t id, bool dir);
    Adafruit_SSD1306* getDisplay() { return &display; }
};

#endif // !SEQUENCER_H
