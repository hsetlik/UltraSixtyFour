#include "ApplyPageAnimation.h"

ApplyPageAnimation::ApplyPageAnimation(/* args */) : PixelAnimation(4, 48)
{
}

ApplyPageAnimation::~ApplyPageAnimation()
{
}

void ApplyPageAnimation::initFrameBuffer()
{
    buffer = {};
    auto base = SeqColors::selectColor;
    const float curveFactor = 0.35f;
    for(byte i = 0; i < 48; ++i)
    {
        float center = ((float)i / 48.0f) * 4.0f;
        Frame frame = {};
        for(byte page = 0; page < 4; ++page)
        {
            auto difference = abs(center - (float)page);
            auto val = pow(curveFactor, difference);
            frame.push_back({base.h, base.s, val * 0.9f});
        }
        buffer.push_back(frame);
    }
}
