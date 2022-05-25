#ifndef SPIFFSENVIRONMENT_H
#define SPIFFSENVIRONMENT_H

#include <SPIFFS.h>
#include "Sequencer.h"
#define AUTOSAVE_PERIOD_S 90
#define AUTOSAVE_NAME "prev__used___"

class SpiffsEnvironment
{
    friend class Sequencer;
private:
    Sequencer* const seq;
    unsigned long lastAutosavedAt;
    /*constructor subroutines*/
    //loads the last autosaved sequence
    void autosaveSequence();
public:
    // attempts to save the current sequence, returns success or failure.
    bool saveCurrentSequenceAs(std::string name);
    //attempts to locate and load the sequence with a matching name. returns success or failure.
    bool loadSequence(std::string name);
    //call this in the main loop to check if it's time to autosave the current sequence
    void checkAutosave();
    SpiffsEnvironment(Sequencer* seq);
    ~SpiffsEnvironment();
};

#endif // !SPIFFSENVIRONMENT_H