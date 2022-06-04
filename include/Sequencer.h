#ifndef SEQUENCER_H
#define SEQUENCER_H
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <MCP_DAC.h>
#include "OLEDLog.h"
#include "BootAnimation.h"
#include "TrackClearAnimation.h"
#include "ApplyPageAnimation.h"
#include "SequenceFilesystem.h"

// Macro to turn the pixels off for serial debugging
#define USE_NEOPIXELS true

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

//DAC pins
#define DAC1_PIN 22
#define DAC2_PIN 12

//I2C pins
#define SDA 17
#define SCL 5

//gate outputs
#define GATE1 15
#define GATE2 4
#define GATE3 2
#define GATE4 16

//NeoPixels data line
#define PIXEL_PIN 1

//Other macros
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C

#define MIN_LOOP_PERIOD 1000


/*
Vout = 10.56 * (value / 4095)
Vnote = (1 / 12) * note
1/12 V = ~83.333mV
10.56 / 4095 = 0.002579
~2.579 mV / DAC integer
83.33333333 / 2.579 = 32.312

*/

#define HALFSTEP_INCREMENT 25.644f

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
    MCP4822 dac1;

    MCP4822 dac2;

    Adafruit_NeoPixel pixels;

    Sequence currentSequence;

    BootAnimation bootAnim;

    TrackClearAnimation trackClearAnim;

    ApplyPageAnimation applyPageAnim;

    SequenceFilesystem fileSystem;

    static uint16_t levelForMidiNote(uint16_t note)
    {
        return ((float)note * HALFSTEP_INCREMENT);
    }
    void setLevelForTrack(uint8_t trk, uint16_t mV);
    void setStepPixel(byte idx, uint32_t color);
    void setPagePixel(byte idx, uint32_t color);
    void setTrackPixel(byte idx, uint32_t color);
    unsigned long lastUpdated = 0;
    unsigned long ledLastUpdated = 0;
    unsigned long autosaves = 0;

    void writeToDac(bool useFirst, bool channel, uint16_t value);
    unsigned long loopIdx;


public:
    Sequencer();
 //loop subroutines
    void checkAdvance() { currentSequence.checkAdvance(); }
    void updateLeds();
    void updateDACs();
    void updateGates();
    void buttonPressed(uint8_t id);
    void buttonHeld(uint8_t id);
    void encoderTurned(uint8_t id, bool dir);
    void autosave();
    void loadAutosaved();
};

#endif // !SEQUENCER_H
