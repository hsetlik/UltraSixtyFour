#include "Sequence.h"

//=====================STEP==============================
Step::Step() : midiNumber(69), gate(false), length(80)
{
}

void Step::addToJsonArray(JsonArray& arr)
{
    auto obj = arr.createNestedObject();
    obj["midiNumber"] = midiNumber;
    obj["gate"] = gate;
    obj["length"] = length;
}
//=====================TRACK==============================
int Track::lastOnStep(uint8_t idx)
{
    uint8_t stepsChecked = 0;
    while (stepsChecked < SEQ_LENGTH)
    {
        if (steps[idx].gate)
            return idx;
        idx = (idx > 0) ? idx - 1 : SEQ_LENGTH - 1;
        ++stepsChecked;
    }
    return -1;
}
void Track::addNestedStepsArray(JsonArray& arr)
{
    auto jsonSteps = arr.createNestedArray();
    for(uint8_t s = 0; s < SEQ_LENGTH; ++s)
    {
        steps[s].addToJsonArray(jsonSteps);
    }
}
//=====================SEQUENCE==============================
Sequence::Sequence() :
currentStep(0),
currentTrack(0),
selectedStep(0),
isPlaying(false),
tempo(120),
periodMicros(0),
microsIntoPeriod(0),
lastMicros(0)
{
    setTempo(tempo);
}

void Sequence::checkAdvance()
{
    //TODO
    auto now = micros();
    microsIntoPeriod += (now - lastMicros);
    if (microsIntoPeriod >= periodMicros)
        advance();
    lastMicros = now;
}

void Sequence::setSequenceLeds(Adafruit_NeoPixel* stepLeds, Adafruit_NeoPixel* pageLeds)
{
    //clear pixels
    stepLeds->clear();
    pageLeds->clear();
    //first: figure out what page we're on
    uint8_t page = floor(currentStep / PAGE_LENGTH);  
    if (page < 0 || page > 3)
    {
        //page is out of range
    }
    //set the page LEDs
    for(uint8_t p = 0; p < (SEQ_LENGTH / PAGE_LENGTH); ++p)
    {
        if (p == page)
        {
            auto col = SeqColors::selectColor;
            pageLeds->setPixelColor(p, col.asRgb());
        }
        else
        {
            Hsv blank = {0.0f, 0.0f, 0.0f};
            pageLeds->setPixelColor(p, blank.asRgb());
        }
    }
    //set colors for all the steps
    auto firstStepIdx = page * PAGE_LENGTH;
    for (int i = 0; i < PAGE_LENGTH; ++i)
    {
        auto& step = tracks[currentTrack].steps[firstStepIdx + i];
        if (!step.gate && (firstStepIdx + i) != currentStep)
        {
            Hsv col = {0.0f, 0.0f, 0.0f};
            stepLeds->setPixelColor(i, col.asRgb());
        }
        else if (!step.gate)
        {
            auto color = SeqColors::stepColor;
            stepLeds->setPixelColor(i, color.asRgb());
        } else if (step.gate && selectedStep == (firstStepIdx + i))
        {
            auto base = SeqColors::pitchColors[step.midiNumber % 12];
            Hsv col = {base.h, base.s, 1.0f};
            stepLeds->setPixelColor(i, col.asRgb());
        } else if (step.gate)
        {
            auto col = SeqColors::pitchColors[step.midiNumber % 12];
            stepLeds->setPixelColor(i, col.asRgb());
        } else if (selectedStep == (firstStepIdx + i))
        {
            auto col = SeqColors::selectColor;
            stepLeds->setPixelColor(i, col.asRgb());
        }
    }
    //Set the LED for the current step if playing
    if (isPlaying)
    {
        auto s = currentStep % PAGE_LENGTH;
        auto col = SeqColors::stepColor;
        stepLeds->setPixelColor(s, col.asRgb());
    }
    //send to hardware
    stepLeds->show();
    pageLeds->show();
}

void Sequence::advance()
{
    microsIntoPeriod -= periodMicros;
    currentStep = (currentStep + 1) % SEQ_LENGTH;
}

void Sequence::updateGates()
{
    //TODO
    for (uint8_t i = 0; i < NUM_TRACKS; ++i)
    {
        //get the last step which was triggered
        auto& trk = tracks[i];
        auto lastOn = trk.lastOnStep(currentStep);
        auto now = micros();
        //Do nothing if the track is empty
        if (lastOn == -1)
            continue;
        auto stepStart = now - microsIntoPeriod;
        //if this is a gate from a previous step we need to offset by period * num. steps apart
        if (lastOn != currentStep)
        {
            auto offset = (unsigned long)abs(currentStep - lastOn) * periodMicros;
            stepStart -= offset;
        }
        //length of the gate in microseconds
        auto length = (unsigned long)(periodMicros * (float)(trk.steps[lastOn].length / 100.0f));
        auto gateOver = now >= stepStart + length;
        //Turn on the gate as needed
        if (lastOn == currentStep && !gateOver)
        {
            trk.gateHigh = true;
            digitalWrite(gatePins[i], HIGH);
        }
        else if (trk.gateHigh && gateOver) //turn the gate off
        {
            trk.gateHigh = false;
            digitalWrite(gatePins[i], LOW);
        }
    }
}

//Rotary encoder handlers
void Sequence::shiftSelected(bool dirOrLength)
{
    selectedStep = (dirOrLength) ? (selectedStep + 1) % SEQ_LENGTH : selectedStep - 1;
    if (selectedStep < 0)
        selectedStep += SEQ_LENGTH;
}
void Sequence::shiftNote(bool dirOrLength)
{
    auto& note = tracks[currentTrack].steps[selectedStep].midiNumber;
    note = (dirOrLength) ? (note + 1) % MIDI_MAX : note - 1;
    if (note < 0)
        note = 0;
}
void Sequence::shiftTrack(bool dirOrLength)
{
    currentTrack = dirOrLength ? (currentTrack + 1) % NUM_TRACKS : currentTrack - 1;
    if (currentTrack < 0)
        currentTrack += NUM_TRACKS;
}
void Sequence::shiftTempo(bool dirOrLength)
{
    tempo = dirOrLength ? tempo + 1 : tempo - 1;
    if (tempo > TEMPO_MAX)
    {
        tempo = TEMPO_MAX;
    }
    else if (tempo < TEMPO_MIN)
        tempo = TEMPO_MIN;
}
void Sequence::shiftGateLength(bool dirOrLength)
{
    auto& length = tracks[currentTrack].steps[selectedStep].length;
    length = dirOrLength ? length + 5 : length - 5;
    if (length < GATE_MIN)
        length = GATE_MIN;
    else if (length > GATE_MAX)
        length = GATE_MAX;

}
void Sequence::shiftQuantType(bool dirOrLength)
{

}
void Sequence::shiftQuantRoot(bool dirOrLength)
{

}

void Sequence::updateMvs()
{
    //TODO
}
void Sequence::setTempo(int t)
{
    tempo = t;
    periodMicros = (unsigned long)(60000000.0f / (float)tempo);
}
SeqJson Sequence::getJsonDocument(std::string name)
{
    SeqJson doc;
    doc["name"] = name.c_str();
    doc["tempo"] = tempo;
    auto jsonTracks = doc.createNestedArray("tracks");
    for(uint8_t t = 0; t < NUM_TRACKS; ++t)
    {
        tracks[t].addNestedStepsArray(jsonTracks);
    }
    return doc;
}
