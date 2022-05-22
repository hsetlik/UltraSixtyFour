#ifndef SEQUENCE_H
#define SEQUENCE_H

#define SEQ_LENGTH 64
#define PAGE_LENGTH 16

#define MIDI_MAX 127

#define NUM_TRACKS 4

#define TEMPO_MIN 5
#define TEMPO_MAX 350

#define GATE_MIN 5
#define GATE_MAX 99


#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "Hsv.h"
#include <ArduinoJson.h>
#include <string>
#include <array>

#define SEQ_BYTES 12288
typedef StaticJsonDocument<SEQ_BYTES> SeqJson;

typedef std::array<uint32_t, 16> SeqColorState;

//the microcontroller pins attached to each gate output
const uint8_t gatePins[] = {2, 3, 4, 5};

//One step in a sequence track
struct Step
{
    Step();
    uint8_t midiNumber;
    bool gate;
    //gate length as a percentage of the length of one step 
    int length;
    void addToJsonArray(JsonArray& arr);
};

//One of a seqnence's four tracks
struct Track
{
    Track(): gateHigh(false) {}
    std::array<Step, SEQ_LENGTH> steps;
    bool gateHigh;
    //returns first the step at or before idx which has its gate toggled on
    int lastOnStep(uint8_t idx);
    void addNestedStepsArray(JsonArray& arr);
    
};
//All the data for playback/saving a sequence

class Sequence
{
public:
    Sequence();
    std::array<Track, NUM_TRACKS> tracks;
    uint8_t currentStep;
    uint8_t currentTrack;
    uint8_t selectedStep;
    bool isPlaying; 
    //check whether enough time has elapsed to move to the next step. Call this on every loop before updating hardware
    void checkAdvance();
    //Update the 16-led display and the 4 page LEDs to reflect the current sequence
    //Set the gates
    void updateGates();
    //update the DAC voltages
    void updateMvs();
    //set the tempo and calcualte the period of each step
    void setTempo(int t);
    int getTempo() {return tempo;}
    //shifting functions for rotary encoders
    void shiftSelected(bool dirOrLength);
    void shiftNote(bool dirOrLength);
    void shiftTrack(bool dirOrLength);
    void shiftTempo(bool dirOrLength);
    void shiftGateLength(bool dirOrLength);
    void shiftQuantType(bool dirOrLength);
    void shiftQuantRoot(bool dirOrLength);
    //track/sequence manipulation callbacks
    //Copies the contents of the current 16-step page and pastes in the other 3 pages
    void applyCurrentPage();
    //removes all notes on the current page
    void clearCurrentPage();
    //Get JSON to save sequence file
    SeqJson getJsonDocument(std::string name="sequence name");
    Step& getCurrentStep() { return tracks[currentTrack].steps[currentStep]; }
    //gets the current state of the 16 step LEDs
    SeqColorState currentStepColors();
private:
    uint32_t getStepColor(uint8_t idx);
    int tempo;
    unsigned long periodMicros;
    unsigned long microsIntoPeriod;
    unsigned long lastMicros;
    void advance();
    uint8_t pageForStep(uint8_t step);
    std::array<Step*, PAGE_LENGTH> pageSteps(uint8_t step);
    std::array<Step*, PAGE_LENGTH> getPage(uint8_t page);
};
#endif
