#ifndef PIXELANIMATION_H
#define PIXELANIMATION_H
#include <vector>
#include "Hsv.h"


typedef std::vector<Hsv> Frame;
typedef std::vector<Frame> FrameBuffer;



//Abstract class for pixel animations 
class PixelAnimation
{
protected:
    bool running;
    unsigned long lastStartedAt;
    unsigned long framePeriodMs;
    const uint16_t length;
    uint16_t currentFrame;
    FrameBuffer buffer;
    //This virtual function gets called in the constructor to initialize the frame data-- all the animation logic happens here
    virtual void initFrameBuffer()=0;
    std::vector<uint32_t> currentColorVector();
    void finish();
public:
    PixelAnimation(uint8_t numPixels=12, uint16_t frames=48);
    virtual ~PixelAnimation();
    std::vector<uint32_t> process(std::vector<uint32_t>& arr)
    {
        return running ? currentColorVector() : arr;
    }
    const uint8_t numPixels;
    bool isRunning() { return running; }
    //Starts the animation
    void start();
    //Call this in the loop function
    //Gets the number of frames in the animation
    uint16_t getLength() { return length; }

};


#endif // !PIXELANIMATION_H