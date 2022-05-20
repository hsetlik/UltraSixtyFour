#ifndef HSV_H
#define HSV_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

struct Hsv
{
    float h;
    float s;
    float v;
    uint32_t asRgb();
    static Hsv forMidiNote(uint8_t note);
};

namespace SeqColors
{
    static Hsv off = {0.0f, 0.0f, 0.0f};
    //12 tones spaced evenly along the color wheel
    static Hsv cHsv = {0.0f, 1.0f, 0.25f};
    static Hsv csHsv = {30.0f, 1.0f, 0.25f};
    static Hsv dHsv = {60.0f, 1.0f, 0.25f};
    static Hsv dsHsv = {90.0f, 1.0f, 0.25f};
    static Hsv eHsv = {120.0f, 1.0f, 0.25f};
    static Hsv fHsv = {150.0f, 1.0f, 0.25f};
    static Hsv fsHsv = {180.0f, 1.0f, 0.25f};
    static Hsv gHsv = {210.0f, 1.0f, 0.25f};
    static Hsv gsHsv = {240.0f, 1.0f, 0.25f};
    static Hsv aHsv = {270.0f, 1.0f, 0.25f};
    static Hsv asHsv = {300.0f, 1.0f, 0.25f};
    static Hsv bHsv = {330.0f, 1.0f, 0.25f};

    Hsv pitchColors[] = {cHsv, csHsv, dHsv, dsHsv, eHsv, fHsv, fsHsv, gHsv, gsHsv, aHsv, asHsv, bHsv};

    Hsv trackColors[] = {dHsv, fHsv, gsHsv, bHsv};

    static Hsv stepColor = {330.0f, 1.0f, 0.25f};
    static Hsv selectColor = {230.0f, 1.0f, 0.25f};

    static Hsv ionianHsv = {0.0f, 1.0f, 0.5f};
    static Hsv dorianHsv = {51.429f, 1.0f, 0.5f};
    static Hsv phrygianHsv = {102.858f, 1.0f, 0.5f};
    static Hsv lydianHsv = {154.287f, 1.0f, 0.5f};
    static Hsv mixolydianHsv = {205.716f, 1.0f, 0.5f};
    static Hsv aeolianHsv = {257.145f, 1.0f, 0.5f};
    static Hsv locrianHsv = {308.574f, 1.0f, 0.5f};

    Hsv modeColors[] = {ionianHsv, dorianHsv, phrygianHsv, lydianHsv, mixolydianHsv, aeolianHsv, locrianHsv};
}

#endif