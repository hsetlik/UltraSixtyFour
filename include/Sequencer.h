#ifndef SEQUENCER_H
#define SEQUENCER_H
#include <Adafruit_NeoPixel.h>
#include <MCP48xx.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "AnalogButtonGroup.h"

// Pin definitions based on 30-pin ESP32 Devkit V1 pinout
//Encoder pins
#define ADATA 36
#define ACLK 39
#define BDATA 34
#define BCLK 35
#define CDATA 32
#define CCLK 33
#define DDATA 25
#define DCLK 26



//Analog Button pins
#define BUTTONS1 27
#define BUTTONS2 14

//sequence inputs
#define CV_IN 12
#define TRIG_IN 13

//DAC/SPI pins
#define MOSI 23
#define SCK 18
#define DAC1 1
#define DAC2 3

//I2C pins
#define SDA 21
#define SCL 22

//gate outputs
#define GATE1 16
#define GATE2 4
#define GATE3 2
#define GATE4 15
//NeoPixels data line
#define PIXEL_PIN 19

//Other macros
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     5
#define SCREEN_ADDRESS 0x3C

class Sequencer
{
private:
    MCP4822 dac1;

    MCP4822 dac2;

    Adafruit_NeoPixel pixels;

    Adafruit_SSD1306 display;

    AnalogButtonGroup groupA;

    AnalogButton encAButton;
    AnalogButton encBButton;
    AnalogButton encCButton;
    AnalogButton encDButton;

    AnalogButton pgLeft;
    AnalogButton pgRight;

    AnalogButtonGroup groupB;

    AnalogButton trk1Button;
    AnalogButton trk2Button;
    AnalogButton trk3Button;
    AnalogButton trk4Button;

    AnalogButton leftButton;
    AnalogButton rightButton;
    AnalogButton playButton;

public:
    Sequencer();
    void setup();
    void loop();

};

#endif // !SEQUENCER_H
