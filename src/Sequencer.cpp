#include "Sequencer.h"

Sequencer::Sequencer() : pixels(24, PIXEL_PIN, NEO_RGB + NEO_KHZ800),
                         dac1(DAC1),
                         dac2(DAC2),
                         display(SCREEN_WIDTH, SCREEN_HEIGHT)
{
    pixels.begin();
    pixels.setBrightness(40);
    // set up DACS
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
/*
    // set up display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        for (;;)
            ; // Don't proceed, loop forever
    }
*/
    pinMode(GATE1, OUTPUT);
    pinMode(GATE2, OUTPUT);
    pinMode(GATE3, OUTPUT);
    pinMode(GATE4, OUTPUT);
}

void Sequencer::setup()
{
  
}

void Sequencer::loop()
{
    updateLeds();
    updateDisplay();
    updateDACs();
    updateGates();

}
void Sequencer::buttonPressed(uint8_t id)
{
   Serial.print("Button ");
    Serial.print(id);
    Serial.print(" turned\n"); 
}
void Sequencer::buttonHeld(uint8_t id)
{
   Serial.print("Button ");
    Serial.print(id);
    Serial.print(" pressed\n"); 

}
void Sequencer::encoderTurned(uint8_t id, bool dir)
{
    Serial.print("Encoder ");
    Serial.print(id);
    Serial.print(" turned\n");
}

void Sequencer::setStepPixel(byte idx, uint32_t color)
{
    if (idx < 8)
        idx = 8 - idx;
    pixels.setPixelColor(idx, color);
}

void Sequencer::setPagePixel(byte idx, uint32_t color)
{
    pixels.setPixelColor(16 + idx, color);
}

void Sequencer::setTrackPixel(byte idx, uint32_t color)
{
    pixels.setPixelColor(20 + idx, color);
}

void Sequencer::updateLeds()
{
    /*
    Limit the refresh rate of the pixels
    Since the LEDs must be updated continuously to reflect the sequence, this function gets called 1:1 from the main loop() function.
    On an ESP32, this would mean updating the LEDs far more often than is visible, wasting a good amount of time on all the color calculations
    and serial communication to the pixels.
    In practice, this works by keeping track of when the LEDs and then checking if enough time has elapsed to update again in the next loop
    */
    auto now = millis();
    if (now - ledLastUpdated > 1000 / MAX_REFRESH_HZ)
    {
        //DO PIXEL STUFF HERE
        ledLastUpdated = now;
    }
}