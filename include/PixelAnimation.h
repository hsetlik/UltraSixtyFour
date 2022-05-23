#ifndef PIXELANIMATION_H
#define PIXELANIMATION_H
#include <vector>
#include "Hsv.h"
#define DEFAULT_FRAME_RATE 24


typedef std::vector<Hsv> Frame;
typedef std::vector<Frame> FrameBuffer;



//Abstract class for pixel animations 
class PixelAnimation
{
protected:
    bool running;
    unsigned long lastStartedAt;
    uint16_t frameRate;
    unsigned long framePeriodMs;
    const uint16_t length;
    uint16_t currentFrame;
    FrameBuffer buffer;
    //This virtual function gets called in the constructor to initialize the frame data-- all the animation logic happens here
    virtual void initFrameBuffer()=0;
    std::vector<uint32_t> currentColorVector();
    void finish();
public:
    PixelAnimation(uint8_t numPixels=12, uint16_t frames=48, uint16_t rate=DEFAULT_FRAME_RATE);
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
    //Gets the current frame rate
    uint16_t getFrameRate() {return frameRate;}

    void setFrameRate(uint16_t rate);
};


#endif // !PIXELANIMATION_H