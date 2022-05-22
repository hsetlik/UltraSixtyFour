#include "PixelAnimation.h"

PixelAnimation::PixelAnimation(uint8_t n) : 
running(false),
lastStartedAt(0),
numPixels(n)
{
}

PixelAnimation::~PixelAnimation()
{
}

void PixelAnimation::start()
{
    lastStartedAt = millis();
    running = true;
}

void PixelAnimation::applyIfRunning(Adafruit_NeoPixel* pixels)
{
    if (!running)
        return;
    pixels->clear();
    auto colors = currentPixelColors();
    for(byte i = 0; i < numPixels; ++i)
    {
        pixels->setPixelColor(i, colors[i]);
    }
}
