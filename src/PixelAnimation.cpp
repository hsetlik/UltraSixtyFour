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
    if (buffer.size() < 1)
        initFrameBuffer();
    running = true;
    Serial.println("Animation started. . .");
}

void PixelAnimation::finish()
{
    running = false;
    buffer.clear();
}

void PixelAnimation::setFrameRate(uint16_t rate)
{
    frameRate = rate;
    framePeriodMs = 1000 / frameRate;
}

std::vector<uint32_t> PixelAnimation::currentColorVector()
{
  
    std::vector<uint32_t> output = {};
    auto &frame = buffer[currentFrame];
    for(auto& p : frame)
    {
        output.push_back(p.asRgb());
    }

    auto now = millis();
    bool bufferShouldClear = false;
    //advance to the next frame
    if (now - lastStartedAt > framePeriodMs)
    {   
        ++currentFrame;
        if (currentFrame >= length)
        {
            finish();
        }
        lastStartedAt = now;
    }
    return output;
}
