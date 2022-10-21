#include "SequenceBrowser.h"

SequenceBrowser::SequenceBrowser(SequenceFilesystem& fileSystem) : fs(&fileSystem), currentIdx(0)
{
    sequences = fs->getSequenceNames();
}

SequenceBrowser::~SequenceBrowser()
{
}

void SequenceBrowser::render(Adafruit_SSD1306* disp)
{
    /* TODO:
        - shoud display as many saved sequences as the MAX_DISPLAYED macro allows
        - current selected should be black on white, others should be white on black
    */

}