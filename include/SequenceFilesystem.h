#ifndef SEQUENCEFILESYSTEM_H

#include "Sequence.h"
#include <SPIFFS.h>
#include <string>

#define AUTOSAVE_FILENAME "autosave_pattern"

class SequenceFilesystem
{
private:
    char* fileBuffer;
    size_t bufferSize;
    /* data */
    fs::FS* const fileSystem;
public:
    SequenceFilesystem(fs::FS* system= &SPIFFS);
    ~SequenceFilesystem();
    // Attempts to load the sequence with the given name into the referenced object. Returns success or failure.
    bool load(std::string name, Sequence& seq);
    // Attempts to save the referenced sequence to a file with the given name. Returns success or failure.
    bool save(std::string name, Sequence& seq);
};

#endif // !SEQUENCEFILESYSTEM_H