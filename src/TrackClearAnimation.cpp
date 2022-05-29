#include "TrackClearAnimation.h"

TrackClearAnimation::TrackClearAnimation(byte t) : PixelAnimation(4, 72), track(t)
{

}

TrackClearAnimation::~TrackClearAnimation()
{
    
}

void TrackClearAnimation::initFrameBuffer()
{
    buffer = {};
    for (byte f = 0; f < length; ++f)
    {
        uint16_t phaseIdx = f / 15;
        bool blinkPhase = phaseIdx % 2 == 0;
        Frame frame = {};
        for(byte t = 0; t < numPixels; ++t)
        {
            if (t != track)
            {
                auto color = blinkPhase ? SeqColors::csHsv : SeqColors::aeolianHsv;
                frame.push_back(color);
            } 
            else 
                frame.push_back(SeqColors::trackColors[track]);
        }
        buffer.push_back(frame);
    }
}

