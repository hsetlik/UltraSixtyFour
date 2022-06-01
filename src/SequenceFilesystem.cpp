#include "SequenceFilesystem.h"

SequenceFilesystem::SequenceFilesystem(fs::FS *sys) : fileSys(sys)
{
    //enable default SPIFFS if that's what we're using
    if (fileSys == &SPIFFS)
    {
        if (SPIFFS.begin(true))
        {
            Serial.println("SPIFFS started");
            auto sizeKb = SPIFFS.totalBytes() / 1000;
            std::string lStr = "SPIFFS has " + std::to_string(sizeKb) + "kb available";
            ///OLEDLog::println(lStr);
        }
        else
            Serial.println("SPIFFS failed");
    }
}

SequenceFilesystem::~SequenceFilesystem()
{
}

bool SequenceFilesystem::loadSequence(String name, Sequence& seq)
{
    String path = "/" + name + ".json";
    auto file = fileSys->open(path.c_str(), "r");

   
    return false;
}
bool SequenceFilesystem::saveSequence(String name, Sequence& seq)
{
    /*
    auto doc = seq.getJsonDocument(name.c_str());
    auto fileName = "/" + name + ".json";
    if (fileSys.saveToFile(fileName.c_str(), doc));
        return true;
    */
    return false;
}