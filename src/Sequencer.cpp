#include "Sequencer.h"

Sequencer::Sequencer() : pixels(24, PIXEL_PIN, NEO_RGB + NEO_KHZ800),
                         display(SCREEN_WIDTH, SCREEN_HEIGHT)
{
    Serial.println("Creating sequencer");
    
    Wire.begin(SDA, SCL);
    pixels.begin();
    pixels.setBrightness(40);
    Serial.println("Initialized neo pixels");
    // set up DACS

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println("Could not start display!");
        for (;;)
            ; // Don't proceed, loop forever
    } else 
    {
        Serial.println("Display started successfully");
    }
    display.display();
    Serial.println("Display started");
    pinMode(GATE1, OUTPUT);
    pinMode(GATE2, OUTPUT);
    pinMode(GATE3, OUTPUT);
    pinMode(GATE4, OUTPUT);
    bootAnim.start();

}

void Sequencer::loop()
{
    currentSequence.checkAdvance();
    updateLeds();
}

void Sequencer::buttonPressed(uint8_t id)
{
    ButtonId button = (ButtonId)id;
    switch (button)
    {
        case MenuL:
        {
            break;
        }
        case MenuR:
        {
            break;
        }
        case Play:
        {
            currentSequence.isPlaying = !currentSequence.isPlaying;
            break;
        }
        case Track1:
        {
            currentSequence.currentTrack = 0;
            break;
        }
        case Track2:
        {
            currentSequence.currentTrack = 1;
            break;
        }
        case Track3:
        {
            currentSequence.currentTrack = 2;
            break;
        }
        case Track4:
        {
            currentSequence.currentTrack = 3;
            break;
        }
        case E1:
        {
            break;
        }
        case E2:
        {
            break;
        }
        case E3:
        {
            currentSequence.toggleSelectedGate();
            break;
        }
        case E4:
        {
            break;
        }
        case PageL:
        {
            currentSequence.shiftPage(true);
            break;
        }
        case PageR:
        {
            currentSequence.shiftPage(false);
            break;
        }
        default:
            break;
    }
}
void Sequencer::buttonHeld(uint8_t id)
{
    ButtonId button = (ButtonId)id;
    switch (button)
    {
        case MenuL:
        {
            break;
        }
        case MenuR:
        {
            break;
        }
        case Play:
        {
            break;
        }
        case Track1:
        {
            trackClearAnim.track = 0;
            trackClearAnim.start();
            currentSequence.clearTrack(0);
            break;
        }
        case Track2:
        {
            trackClearAnim.track = 1;
            trackClearAnim.start();
            currentSequence.clearTrack(1);
            break;
        }
        case Track3:
        {
            trackClearAnim.track = 2;
            trackClearAnim.start();
            currentSequence.clearTrack(2);
            break;
        }
        case Track4:
        {
            trackClearAnim.track = 2;
            trackClearAnim.start();
            currentSequence.clearTrack(3);
            break;
        }
        case E1:
        {
            break;
        }
        case E2:
        {
            break;
        }
        case E3:
        {
            break;
        }
        case E4:
        {
            break;
        }
        case PageL:
        {
            currentSequence.applyCurrentPage();
            applyPageAnim.start();
            break;
        }
        case PageR:
        {
            break;
        }
        default:
            break;
    }
}

void Sequencer::encoderTurned(uint8_t id, bool dir)
{
    //Serial.print("Encoder ");
    //Serial.print(id);
    //Serial.print(" turned\n");
    switch (id)
    {
        case 0:
        {
            currentSequence.shiftTempo(dir);
            break;
        }
        case 1:
        {
            currentSequence.shiftSelected(dir);
            break;
        }
        case 2:
        {
            currentSequence.shiftNote(dir);
            break;
        }
        case 3:
        {
            break;
        }
        default:
            break;
    }
}

void Sequencer::setStepPixel(byte idx, uint32_t color)
{
    if (idx > 7)
    {
        auto diff = idx - 7;
        idx = 16 - diff;
    }
    pixels.setPixelColor(idx, color);
}

void Sequencer::setPagePixel(byte idx, uint32_t color)
{
    pixels.setPixelColor(16 + idx, color);
}

void Sequencer::setTrackPixel(byte idx, uint32_t color)
{
    pixels.setPixelColor(20 + idx, color);
}

void Sequencer::updateLeds()
{
    /*
    Limit the refresh rate of the pixels
    Since the LEDs must be updated continuously to reflect the sequence, this function gets called 1:1 from the main loop() function.
    On an ESP32, this would mean updating the LEDs far more often than is visible, wasting a good amount of time on all the color calculations
    and serial communication to the pixels.
    In practice, this works by keeping track of when the LEDs and then checking if enough time has elapsed to update again in the next loop
    */
    auto now = millis();
    if (now - ledLastUpdated > 1000 / MAX_REFRESH_HZ)
    {
        //DO PIXEL STUFF HERE
        pixels.clear();
        ledLastUpdated = now;
        //set the step pixel colors
        auto colors = currentSequence.currentStepColors();
        colors = bootAnim.process(colors);
        for(byte i = 0; i < PAGE_LENGTH; ++i)
        {
            setStepPixel(i, colors[i]);
        }
        //get the track and page colors
        auto trackColors = currentSequence.currentTrackColors();
        trackColors = trackClearAnim.process(trackColors);
        auto pageColors = currentSequence.currentPageColors();
        pageColors = applyPageAnim.process(pageColors);
        for(byte i = 0; i < 4; ++i)
        {
            setTrackPixel(i, trackColors[i]);
            setPagePixel(i, pageColors[i]);
        }
        pixels.show();
    }
}
void Sequencer::updateGates()
{

}

void Sequencer::updateDisplay()
{

}

std::array<uint16_t, 4> Sequencer::currentTrackLevels()
{
    std::array<uint16_t, 4> arr;
    for(byte trk = 0; trk < 4; trk++)
    {
        auto& step = currentSequence.tracks[trk].steps[currentSequence.currentStep];
        arr[trk] = levelForMidiNote(step.midiNumber);
    }
    return arr;
}
