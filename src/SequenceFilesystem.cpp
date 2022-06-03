#include "SequenceFilesystem.h"


SequenceFilesystem::SequenceFilesystem(fs::FS* sys) :
fileSystem(sys),
bufferSize(0),
fileBuffer(nullptr)
{
}

SequenceFilesystem::~SequenceFilesystem()
{
}

bool SequenceFilesystem::load(std::string name, Sequence& seq)
{
    std::string path = "/" + name + ".seq";
    if (!fileSystem->exists(path.c_str()))
        return false;
    auto file = fileSystem->open(path.c_str(), FILE_READ);
    std::string fileStr = "";
    // make sure the std::string's allocated memory is enough because we need to write to the underlying char array and need enough room in advance
    fileStr.reserve(file.size());
    // &fileStr[0] returns a raw pointer to the beginning of the string's char array
    auto bytes = file.readBytes(&fileStr[0], file.size());
    file.close();
    //TODO: we need a way to check that we got a valid sequence string
    auto lStr = "File has " + std::to_string(bytes) + "bytes";
    Serial.println(lStr.c_str());
    seq = Sequence::decode(fileStr);
   return true; 
}

bool SequenceFilesystem::save(std::string name, Sequence& seq)
{
    std::string path = "/" + name + ".seq";
    // open in write mode and create the file if it doesn't exist
    auto file = fileSystem->open(path.c_str(), FILE_WRITE, true);
    auto seqStr = seq.encode().c_str();
    auto bytes = file.print(seqStr);
    file.close();
    auto lStr = "Wrote " + std::to_string(bytes) + " bytes to file at " + path;
    Serial.println(lStr.c_str());
    return true;
}