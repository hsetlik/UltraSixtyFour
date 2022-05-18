#include "Sequencer.h"

Sequencer::Sequencer() : 
pixels(24, PIXEL_PIN, NEO_RGB + NEO_KHZ800),
dac1(DAC1),
dac2(DAC2),
display(SCREEN_WIDTH, SCREEN_HEIGHT),
groupA(BUTTONS1, 6),
encAButton(0, 1), //NOTE: measure actual mean voltage readings
encBButton(1, 1),
encCButton(2, 1),
encDButton(3, 1),
pgLeft(4, 1),
pgRight(5, 1),
groupB(BUTTONS2, 7),
trk1Button(0, 1),
trk2Button(1, 1),
trk3Button(2, 1),
trk4Button(3, 1),
leftButton(4, 1),
rightButton(5, 1),
playButton(6, 1)
{
    pixels.begin();
    pixels.setBrightness(40);
    //set up DACS
    dac1.init();
    dac1.turnOnChannelA();
    dac1.turnOnChannelB();
    dac1.setGainA(MCP4822::High);
    dac1.setGainB(MCP4822::High);

    dac2.init();
    dac2.turnOnChannelA();
    dac2.turnOnChannelB();
    dac2.setGainA(MCP4822::High);
    dac2.setGainB(MCP4822::High);

    //set up display
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
    {
    for(;;); // Don't proceed, loop forever
    }

    pinMode(GATE1, OUTPUT);
    pinMode(GATE2, OUTPUT);
    pinMode(GATE3, OUTPUT);
    pinMode(GATE4, OUTPUT);

}

void Sequencer::setup()
{
    for(uint8_t i = 0; i < 24; ++i)
    {
        pixels.clear();
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255, 128, 0));
        pixels.show();
        delay(500);
    }
}

void Sequencer::loop()
{
      for(uint8_t i = 0; i < 24; ++i)
    {
        pixels.clear();
        pixels.setPixelColor(i, Adafruit_NeoPixel::Color(255, 128, 0));
        pixels.show();
        delay(500);
    }

}