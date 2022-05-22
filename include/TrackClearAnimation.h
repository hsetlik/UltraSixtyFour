#ifndef TRACKCLEARANIMATION_H
#define TRACKCLEARANIMATION_H
#include "PixelAnimation.h"

class TrackClearAnimation : public PixelAnimation 
{
private:
    /* data */
public:
    uint8_t track;
    void initFrameBuffer() override;
    TrackClearAnimation(byte track=0);
    ~TrackClearAnimation();
};
#endif // !TRACKCLEARANIMATION_H