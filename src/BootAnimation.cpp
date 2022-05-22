#include "BootAnimation.h"
BootAnimation::BootAnimation() : PixelAnimation(16, 72)
{
}

BootAnimation::~BootAnimation()
{
}

void BootAnimation::initFrameBuffer()
{
    auto base = SeqColors::stepColor;
    const float maxValue = 0.8f;
    const float curveFactor = 0.675f;
    for (uint16_t f = 0; f < length; ++f)
    {
        Frame frame = {};
        auto t = (float)f / (float)length;
        uint16_t p = t * numPixels;
        for (uint16_t pixel = 0; pixel < numPixels; ++pixel)
        {
            //figure out distance between pixel and brightest pixel
            uint16_t difference = (uint16_t)abs(p - pixel);
            auto val = pow(curveFactor, (float)difference);
            frame.push_back({base.h, base.s, maxValue * val});
        }
        buffer.push_back(frame);
    }
}