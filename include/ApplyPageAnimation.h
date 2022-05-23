#ifndef APPLYPAGEANIMATION_H
#define APPLYPAGEANIMATION_H
#include "PixelAnimation.h"

class ApplyPageAnimation : public PixelAnimation
{
private:
    /* data */
    void initFrameBuffer() override;
public:
    ApplyPageAnimation(/* args */);
    ~ApplyPageAnimation();
};


#endif // !APPLYPAG