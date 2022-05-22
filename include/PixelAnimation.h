#ifndef PIXELANIMATION_H
#define PIXELANIMATION_H
#include <vector>
#include "Hsv.h"

//Abstract class for pixel animations 
class PixelAnimation
{
protected:
    bool running;
    unsigned long lastStartedAt;
    //Pure virtual function where the logic of the animation happens
    virtual std::vector<uint32_t> currentPixelColors()=0;
public:
    PixelAnimation(uint8_t n=1);
    virtual ~PixelAnimation();
    const uint8_t numPixels;
    bool isRunning() { return running; }
    //starts the animation
    void start();
    //Pixels can be passed into this function without checking isRunning()
    void applyIfRunning(Adafruit_NeoPixel* pixels);


};


#endif // !PIXELANIMATION_H