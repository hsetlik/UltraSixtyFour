#ifndef SEQUENCEFILESYSTEM_H

#include "Sequence.h"
#define AUTOSAVE_NAME "lastAutosavedSequence"
#define FORMAT_SPIFFS_IF_FAILED true
#include <FS.h>
#include <SPIFFS.h>

class SequenceFilesystem
{
private:
    fs::FS* const fileSys;
    /* data */
    static bool isValidSequence(File& file);
public:
    SequenceFilesystem(fs::FS* fileSys = &SPIFFS);
    ~SequenceFilesystem();
    // Attempts to load the named file into the referenced sequence. Returns success or failure.
    bool loadSequence(String name, Sequence& seq);
    // Attempts to save the referenced sequence with the given name. Returns success or failure.
    bool saveSequence(String name, Sequence& seq);

};
#endif // !SEQUENCEFILESYSTEM_H
