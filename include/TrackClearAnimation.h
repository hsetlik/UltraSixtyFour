#ifndef TRACKCLEARANIMATION_H
#define TRACKCLEARANIMATION_H
#include "PixelAnimation.h"

class TrackClearAnimation : public PixelAnimation 
{
private:
    /* data */
public:
    uint8_t track;
    TrackClearAnimation(byte track=0);
    ~TrackClearAnimation();
private:
    void initFrameBuffer() override;
};
#endif // !TRACKCLEARANIMATION_H