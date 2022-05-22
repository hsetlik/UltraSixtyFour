#ifndef SEQUENCER_H
#define SEQUENCER_H
#include <Adafruit_NeoPixel.h>
#include <MCP48xx.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "Sequence.h"
#include "OLEDLog.h"
#include "BootAnimation.h"
#include "TrackClearAnimation.h"


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
#define CV_IN 12
#define TRIG_IN 13

//DAC/SPI pins
#define MOSI 23
#define SCK 18
#define DAC1 1
#define DAC2 3

//I2C pins
#define SDA 17
#define SCL 5

//gate outputs
#define GATE1 16
#define GATE2 1
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

    Adafruit_SSD1306 display;

    Sequence currentSequence;

    BootAnimation bootAnim;

    TrackClearAnimation trackClearAnim;
    

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
// testing/debugging
    void runPixelTest();
public:
    Sequencer();
    void loop();
    void buttonPressed(uint8_t id);
    void buttonHeld(uint8_t id);
    void encoderTurned(uint8_t id, bool dir);
    Adafruit_SSD1306* getDisplay() { return &display; }
};

#endif // !SEQUENCER_H
