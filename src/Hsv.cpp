#include "Hsv.h"

uint32_t Hsv::asRgb() const
{
 return Adafruit_NeoPixel::ColorHSV(
        (uint16_t)((h / 360.0f) * 65535), 
        (uint8_t)(s * 255.0f), 
        (uint8_t)(v * 255.0f));
}

Hsv Hsv::forMidiNote(uint8_t note)
{
    return SeqColors::pitchColors[note % 12];
}

float fLerp(float t, float a, float b)
{
    return a + ((b - a) * t);
}

Hsv Hsv::lerp(float t, Hsv a, Hsv b)
{
    return 
    {
        fLerp(t, a.h, b.h),
        fLerp(t, a.s, b.s),
        fLerp(t, a.v, b.v)
    };
}