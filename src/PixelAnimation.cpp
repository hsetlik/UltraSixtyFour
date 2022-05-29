#include "PixelAnimation.h"

PixelAnimation::PixelAnimation(uint8_t n, uint16_t frames) : 
running(false),
lastStartedAt(0),
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
std::vector<uint32_t> PixelAnimation::currentColorVector()
{
  
    std::vector<uint32_t> output = {};
    auto &frame = buffer[currentFrame];
    for(auto& p : frame)
    {
        output.push_back(p.asRgb());
    }

    bool bufferShouldClear = false;
    //advance to the next frame
    ++currentFrame;
    if (currentFrame >= length)
    {
        finish();
    }
    return output;
}
