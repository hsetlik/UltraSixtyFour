#ifndef SEQUENCEBROWSER_H
#define SEQUENCEBROWSER_H
#include "SequenceFilesystem.h"
#include "Adafruit_SSD1306.h"

#define MAX_DISPLAYED 3 // the max number of sequence names that can be displayed at once



class SequenceBrowser
{
private:
    SequenceFilesystem* const fs;
    /* data */
    uint8_t currentIdx; //index of the highlighted sequence name
    std::vector<std::string> sequences;
public:
    SequenceBrowser(SequenceFilesystem& fileSystem);
    ~SequenceBrowser();
    void render(Adafruit_SSD1306* display);
    void next()
    {
        currentIdx = (currentIdx + 1) % sequences.size();
    }
    void prev()
    {
        currentIdx = (currentIdx > 1) ? currentIdx - 1 : sequences.size() - 1;
    }
    std::string selectedSequence() { return sequences[currentIdx]; }
};




#endif // !SEQUENCEBROWSER_H