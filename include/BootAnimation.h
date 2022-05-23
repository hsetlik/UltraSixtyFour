#include "PixelAnimation.h"
#include <array>
#define BOOT_ANIM_PERIOD 600


class BootAnimation : public PixelAnimation
{
public:
    BootAnimation();
    ~BootAnimation();
private:
    void initFrameBuffer() override;
};
