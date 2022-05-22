#include "PixelAnimation.h"

PixelAnimation::PixelAnimation(uint8_t n, uint16_t frames, uint16_t rate) : 
running(false),
lastStartedAt(0),
frameRate(rate),
framePeriodMs(rate / 1000),
length(frames),
currentFrame(0),
numPixels(n)
{
    
}

PixelAnimation::~PixelAnimation()
{
}

void PixelAnimation::start()
{
    currentFrame = 0;
    lastStartedAt = millis();
    running = true;
}

void PixelAnimation::updatePixels(Adafruit_NeoPixel* pixels)
{
    if (!running)
        return;
    auto now = millis();
    //advance to the next frame
    if (now - lastStartedAt > framePeriodMs)
    {
        //check if the animation is over
        if (currentFrame >= length)
        {
            currentFrame = 0;
            running = false;
            return;
        }
        lastStartedAt = now;
        //load the new frame in from the buffer
        auto& frame = buffer[currentFrame];
        pixels->clear();
        for(byte i = 0; i < numPixels; ++i)
        {
            pixels->setPixelColor(i, frame[i].asRgb());
        }
        pixels->show();
        ++currentFrame;
    }
}

void PixelAnimation::setFrameRate(uint16_t rate)
{
    frameRate = rate;
    framePeriodMs = 1000 / frameRate;
}
