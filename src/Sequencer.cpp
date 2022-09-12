#include "Sequencer.h"

Sequencer::Sequencer() : pixels(24, PIXEL_PIN, NEO_RGB + NEO_KHZ800)
{
    OLEDLog::println("Creating sequencer");
    digitalWrite(PIXEL_PIN, LOW);
    pixels.setBrightness(40);
    pixels.begin();
    #if !USE_NEOPIXELS
        pixels.clear(); 
        pixels.show();
    #endif
    OLEDLog::println("Initialized neo pixels");
    // set up DACS


    Serial.println("Display started");
    pinMode(GATE1, OUTPUT);
    pinMode(GATE2, OUTPUT);
    pinMode(GATE3, OUTPUT);
    pinMode(GATE4, OUTPUT);

    digitalWrite(GATE1, LOW);
    digitalWrite(GATE2, LOW);
    digitalWrite(GATE3, LOW);
    digitalWrite(GATE4, LOW);

    dac2.selectVSPI();
    dac2.setGain(2);
    dac2.begin(DAC2_PIN);
    dac2.analogWrite(3000, 0);
    dac2.analogWrite(4000, 1);
    Serial.println("DAC 2 initialized");

    dac1.selectVSPI();
    dac1.setGain(2);
    dac1.begin(DAC1_PIN);
    dac1.analogWrite(1000, 0);
    dac1.analogWrite(4000, 1);
    Serial.println("DAC 1 initialized");
    Serial.println("DAC outputs set");
    std::string maxStr = "Max value is: " + std::to_string(dac1.maxValue());
    OLEDLog::println(maxStr.c_str());
    bootAnim.start();
    loadAutosaved();
}

void Sequencer::buttonPressed(uint8_t id)
{
    ButtonId button = (ButtonId)id;
    switch (button)
    {
        case MenuL:
        {
            //loadAutosaved();
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
            OLEDLog::println("Track 1");
            currentSequence.currentTrack = 0;
            break;
        }
        case Track2:
        {
            OLEDLog::println("Track 2");
            currentSequence.currentTrack = 1;
            break;
        }
        case Track3:
        {
            OLEDLog::println("Track 3");
            currentSequence.currentTrack = 2;
            break;
        }
        case Track4:
        {
            OLEDLog::println("Track 4");
            currentSequence.currentTrack = 3;
            break;
        }
        case E1:
        {
            //OLEDLog::println("Loading autosave");
            break;
        }
        case E2:
        {
            currentSequence.lengthMode = !currentSequence.lengthMode;
            break;
        }
        case E3:
        {
            currentSequence.toggleSelectedGate();
            break;
        }
        case E4:
        {
            currentSequence.quantizeMode = !currentSequence.quantizeMode;
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
            currentSequence.pageMode = !currentSequence.pageMode;
            currentSequence.currentStep = currentSequence.currentStep % PAGE_LENGTH;
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
            if (currentSequence.quantizeMode)
                currentSequence.shiftQuantType(dir);
            else
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
            if (currentSequence.quantizeMode)
                currentSequence.shiftQuantRoot(dir);
            else if (currentSequence.lengthMode)
                currentSequence.shiftGateLength(dir);
            else
                currentSequence.shiftNote(!dir);
            break;
        }
        case 3:
        {
            //OLEDLog::println("Encoder 3 turned");
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
    pixels.setPixelColor(23 - (3 - idx), color);
}

void Sequencer::updateLeds()
{
  
    // DO PIXEL STUFF HERE
    pixels.clear();
    // set the step pixel colors
    auto colors = currentSequence.currentStepColors();
    colors = bootAnim.process(colors);
    for (byte i = 0; i < PAGE_LENGTH; ++i)
    {
        setStepPixel(i, colors[i]);
    }
    // get the track and page colors
    auto trackColors = currentSequence.currentTrackColors();
    trackColors = trackClearAnim.process(trackColors);
    auto pageColors = currentSequence.currentPageColors();
    pageColors = applyPageAnim.process(pageColors);
    for (byte i = 0; i < 4; ++i)
    {
        setTrackPixel(i, trackColors[i]);
        setPagePixel(i, pageColors[i]);
    }
    pixels.show();
    ledLastUpdated = micros();
}

void Sequencer::updateDACs()
{
    for (byte i = 0; i < 4; i++)
    {
        auto midi = currentSequence.tracks[i].quantizedMidiAt(currentSequence.currentStep);
        if (currentSequence.tracks[i].steps[currentSequence.currentStep].gate)
            setLevelForTrack(i, levelForMidiNote(midi));
    }
}

void Sequencer::updateGates()
{
    currentSequence.updateGates(GATE1, GATE2, GATE3, GATE4);
}
void Sequencer::writeToDac(bool useFirst, bool channel, uint16_t value)
{
    auto *dacToUse = useFirst ? &dac1 : &dac2;
    dacToUse->analogWrite(value, channel);
}

void Sequencer::setLevelForTrack(uint8_t trk, uint16_t mV)
{

    switch (trk)
    {
    case 0:
    {
        writeToDac(false, true, mV); 
        break;
    }
    case 1:
    {
        writeToDac(false, false, mV);
        break;
    }
    case 2:
    {
        writeToDac(true, false, mV);
        break;
    }
    case 3:
    {
        writeToDac(true, true, mV);
        break;
    }
    default:
        break;
    }
}

void Sequencer::autosave()
{
    fileSystem.save(AUTOSAVE_FILENAME, currentSequence);
    //OLEDLog::println("Autosave " + std::to_string(autosaves) + " finished");
    autosaves += 1;
}

void Sequencer::loadAutosaved()
{
    if (!currentSequence.isPlaying)
    {
        fileSystem.load(AUTOSAVE_FILENAME, currentSequence);
    }
}