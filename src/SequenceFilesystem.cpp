#include "SequenceFilesystem.h"

SequenceFilesystem::SequenceFilesystem(fs::FS *sys) : fileSystem(sys),
                                                      bufferSize(0),
                                                      fileBuffer(nullptr)
{
    if (fileSystem == &SPIFFS)
    {
        if (!SPIFFS.begin(true))
            OLEDLog::println("Failed to start SPIFFS!");
        else
            OLEDLog::println("SPIFFS started");
    }
}

SequenceFilesystem::~SequenceFilesystem()
{
}

bool SequenceFilesystem::load(std::string name, Sequence& seq)
{
    std::string path = "/" + name + ".seq";
    //OLEDLog::println("Path: " + path);
    if (!fileSystem->exists(path.c_str()))
    {
        OLEDLog::println("File at " + path + " not found!");
        return false;
    }
    auto file = fileSystem->open(path.c_str(), FILE_READ);
    auto lStr = "File has " + std::to_string(file.size()) + " bytes";
    OLEDLog::println(lStr.c_str());
    unsigned char buffer[file.size()];
    file.read(buffer, file.size());
    std::string fileStr = "";
    for(auto i = 0; i < file.size(); i++)
        fileStr += (char)buffer[i];
    file.close();
    OLEDLog::println("String has " + std::to_string(fileStr.length()) + " characters");
    //TODO: we need a way to check that we got a valid sequence string
    seq = Sequence::decode(fileStr);
    //OLEDLog::println("Sequence Decoded!");
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
    auto lStr = "Wrote " + std::to_string(bytes);
    //std::string loc = "At: " + path;
    OLEDLog::println(lStr);
    //OLEDLog::println(loc);
    return true;
}

std::vector<std::string> SequenceFilesystem::getSequenceNames()
{
    std::vector<std::string> output = {};
    auto folder = fileSystem->open("/", FILE_READ);
    if (folder.isDirectory()) {
        auto file = folder.openNextFile(FILE_READ);
    }
    //TODO

    return output;
}