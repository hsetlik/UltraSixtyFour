#include "Sequence.h"

//=====================STEP==============================
Step::Step() : midiNumber(45), gate(false), length(80)
{
}

void Step::addToJsonArray(JsonArray& arr)
{
    auto obj = arr.createNestedObject();
    obj["midiNumber"] = midiNumber;
    obj["gate"] = gate;
    obj["length"] = length;
}

uint16_t Step::encode() const
{
    /*
    A step can be represented in 2 bytes. The first byte represents the midi number. With a range of 0-255, the midi number needs all 8 bits.
    The gate lentgh has a range of 0-99, which can be represented in only 7 bits (max value 127). The final bit can be use to store the gate status.
    Byte 1:
    00101101 - represents midi number 45
    Byte 2:
    01010000 - the leftmost bit represents the gate status, the remaining seven bits represent the gate length (80 in this case)
    */
    uint16_t value = midiNumber;  // the midi number should only use the rightmost 8 bits, so the leftmost 8 bits should be cleared
    value = value << 8;           // move the midi number to the left byte
    //OLEDLog::println("MIDI: " + std::to_string(value));
    uint16_t len16 = length;      // length again uses only rightmost byte
    if (gate)
        len16 |= 1 << 7;
    else
        len16 &= ~(1 << 7);
    //OLEDLog::println("LENGTH: " + std::to_string(len16));
    // combine the two bits
    OLEDLog::println("ENCODED: " + std::to_string(value | len16));
    return value | len16;
}

Step Step::decode(uint16_t value)
{
    uint16_t midi = value >> 8; //bring value back down to righthand byte
    uint16_t len = value << 8; //shift length byte left to clear righthand bits
    len = len >> 8;
    bool gate = len >> 7 > 0; //shift to onlt the last bit to check the gate
    //ensure that out length is within 7 bit range and disregard the gate bit
    len &= ~(1 << 7);
    Step step;
    step.midiNumber = (uint8_t)midi;
    step.length = (uint8_t)len;
    step.gate = gate;
    return step;
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

std::string Track::encode()
{
    std::string output = "";
    for (auto& step : steps)
    {
        auto chunk = std::to_string(step.encode()) + ':';
        //OLEDLog::println("Step: " + chunk);
        output += chunk; 
    }
    return output;
}

Track Track::decode(std::string str)
{
    Track output;
    std::string curr = "";
    uint8_t idx = 0;
    for (auto& c : str)
    {
        if (c == ':')
        {
            uint16_t stepVal = (uint16_t)std::stoi(curr);
            output.steps[idx] = Step::decode(stepVal);
            ++idx;
            curr = "";
        }
        else
        {
            curr += c;
        }
    }
    return output;
}
//=====================SEQUENCE==============================
Sequence::Sequence() :
currentStep(0),
currentTrack(0),
selectedStep(0),
isPlaying(false),
quantizeMode(false),
lengthMode(false),
pageMode(false),
tempo(200),
periodMicros(0),
microsIntoPeriod(0),
lastMicros(0)
{
    setTempo(tempo);
    //initDummySequence();
}

Sequence Sequence::decode(std::string str)
{
    Sequence seq;
    //split the string up into lines
    std::string lStr = "Decoding from " + std::to_string(str.size()) + " characters";
    OLEDLog::println(lStr);
    delay(1000);
    std::vector<std::string> lines = {};
    std::string curr = "";
    for (auto& c : str)
    {
        if (c == '\n')
        {
            lines.push_back(curr);
            curr = "";
        }
        curr += c;
    }
    OLEDLog::println(std::to_string(lines.size()) + " lines in file");
    delay(300);
    seq.setTempo(std::stoi(lines[0]));
    OLEDLog::println("Tempo: " + lines[0]);
    for (uint8_t trk = 0; trk < 4; ++trk)
    {
        seq.tracks[trk] = Track::decode(lines[trk + 1]);
    }
    return seq;
}

void Sequence::checkAdvance()
{
    auto now = micros();
    microsIntoPeriod += (now - lastMicros);
    if (microsIntoPeriod >= periodMicros)
        advance();
    lastMicros = now;
}

std::string Sequence::encode()
{
    std::string output = std::to_string(tempo) + '\n';
    for(auto& t : tracks)
        output += t.encode() + '\n';
    return output;
}

void Sequence::advance()
{
    microsIntoPeriod -= periodMicros;
    auto max = (pageMode) ? PAGE_LENGTH : SEQ_LENGTH;
    if (isPlaying)
        currentStep = (currentStep + 1) % max;
}
void Sequence::updateGates(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4)
{
    uint8_t gatePins[] = {p1, p2, p3, p4};
    for (uint8_t i = 0; i < NUM_TRACKS; ++i)
    {
        if (!isPlaying)
        {
            digitalWrite(gatePins[i], LOW);
            continue;
        }
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
    if (dirOrLength)
        selectedStep = (selectedStep + 1) % SEQ_LENGTH;
    else
        selectedStep = (selectedStep == 0) ? 63 : selectedStep - 1;
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
    setTempo(tempo);
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
    tracks[currentTrack].quantizer.shiftMode(dirOrLength);
}
void Sequence::shiftQuantRoot(bool dirOrLength)
{
    tracks[currentTrack].quantizer.shiftRoot(dirOrLength);
}

void Sequence::toggleSelectedGate()
{
    auto& step = tracks[currentTrack].steps[selectedStep];
    step.gate = !step.gate;
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

uint32_t Sequence::getStepColor(uint8_t idx)
{
    auto &step = tracks[currentTrack].steps[idx];
    if (!step.gate)
    {
        if (idx == currentStep)
            return SeqColors::stepColor.asRgb();
        else if (idx == selectedStep)
            return SeqColors::selectColor.asRgb();
        else
             return SeqColors::off.asRgb();
    }
    if (idx == currentStep)
    {
        if (!tracks[currentTrack].steps[idx].gate)
            return SeqColors::stepColor.asRgb();
        else
        {
            const float lerpFactor = 0.327f;
            auto noteColor = Hsv::forMidiNote(tracks[currentTrack].steps[currentStep].midiNumber);
            return Hsv::lerp(lerpFactor, SeqColors::stepColor, noteColor).asRgb();
        } 
    }
    auto midiNum = tracks[currentTrack].quantizedMidiAt(idx);
    auto base = SeqColors::pitchColors[midiNum % 12];
    if (idx == selectedStep)
    {
        Hsv out = {base.h, base.s, 1.0f};
        return out.asRgb();
    }
    return base.asRgb();
}

ColorState Sequence::currentStepColors()
{
    ColorState arr = {};
    auto keyStep = (isPlaying) ? currentStep : selectedStep;
    auto page = keyStep / PAGE_LENGTH;
    for(byte idx = 0; idx < PAGE_LENGTH; ++idx)
        arr.push_back(getStepColor(idx + (page * PAGE_LENGTH)));
    return arr;
}

ColorState Sequence::currentPageColors()
{
    ColorState arr = {};
    if (!quantizeMode)
    {
        auto idx = (isPlaying) ? currentStep : selectedStep;
        auto page = idx / 16;
        for (byte i = 0; i < 4; ++i)
        {
            if (i == page)
                arr.push_back(SeqColors::trackColors[i].asRgb());
            else
                arr.push_back(SeqColors::off.asRgb());
            // switch to a solid color if we're in page mode
            if (pageMode)
                arr.back() = SeqColors::aeolianHsv.asRgb();
        }
    }
    else
    {
        auto rootIdx = tracks[currentTrack].quantizer.getRoot();
        arr.push_back(SeqColors::pitchColors[rootIdx].asRgb());
        auto mode = (uint8_t)tracks[currentTrack].quantizer.getMode();
        for (byte i = 0; i < 3; i++)
            arr.push_back(SeqColors::modeColors[mode].asRgb());
    }
    return arr;
}

ColorState Sequence::currentTrackColors()
{
    ColorState arr = {};
    for(byte i = 0; i < 4; ++i)
    {
        if (i == currentTrack)
            arr.push_back(SeqColors::pitchColors[i].asRgb());
        else
            arr.push_back(SeqColors::off.asRgb());
    }
    return arr;
}


void Sequence::applyCurrentPage()
{
    auto idx = isPlaying ? currentStep : selectedStep;
    int page = idx / PAGE_LENGTH;
    auto startIdx = page * PAGE_LENGTH;
    for(byte i = 0; i < SEQ_LENGTH; ++i)
    {
        auto offset = i % PAGE_LENGTH;
        auto toCopy = tracks[currentTrack].steps[startIdx + offset];
        tracks[currentTrack].steps[i] = toCopy;
    }
}

void Sequence::clearCurrentPage()
{
    auto current = pageSteps(currentStep);
    for (auto i = 0; i < PAGE_LENGTH; ++i)
    {
        *current[i] = Step();
    }
}
    
void Sequence::shiftPage(bool dir)
{
    int8_t initial = isPlaying ? currentStep : selectedStep;
    initial = dir ? (initial + PAGE_LENGTH) % SEQ_LENGTH : initial - PAGE_LENGTH;
    if (initial < 0)
        initial += SEQ_LENGTH;
    if (isPlaying)
        currentStep = initial;
    else
        selectedStep = initial;
}

uint8_t Sequence::pageForStep(uint8_t step)
{
    return (uint8_t)floor(step / PAGE_LENGTH);
}

std::array<Step*, PAGE_LENGTH> Sequence::pageSteps(uint8_t step)
{
    std::array<Step*, PAGE_LENGTH> output;
    auto offset = pageForStep(step) * PAGE_LENGTH;
    for(uint8_t i = 0; i < PAGE_LENGTH; ++i)
    {
        output[i] = &tracks[currentTrack].steps[offset + i];
    }
    return output;
}

std::array<Step*, PAGE_LENGTH> Sequence::getPage(uint8_t page)
{
    std::array<Step*, PAGE_LENGTH> output;
    auto offset = page * PAGE_LENGTH;
    for(uint8_t i = 0; i < PAGE_LENGTH; ++i)
    {
        output[i] = &tracks[currentTrack].steps[offset + i];
    }
    return output;
}

void Sequence::initDummySequence()
{
    uint8_t notes[] = {6, 18, 30, 42};
    for (uint8_t step = 0; step < 64; step++)
    {

        if (step % 2 != 0)
            continue;
        auto idx = step / 2;
        auto note = notes[idx % 4];
        for (uint8_t trk = 0; trk < 4; trk++)
        {
            auto& onStep = tracks[trk].steps[step];
            onStep.gate = true;
            onStep.midiNumber = note + trk;
        }
    }
}