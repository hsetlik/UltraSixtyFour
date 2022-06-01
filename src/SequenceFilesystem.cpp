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
    if (fileSys->exists(path.c_str()))
        Serial.println("File found");
    else
        return false;
    auto file = fileSys->open(path.c_str(), "r");
    Serial.println("Opened file");
   std::string lStr = "File has " + std::to_string(file.size()) + " bytes"; 
    Serial.println(lStr.c_str());
    //TODO: we need a way of making sure that the file is a valid Sequence JSON
    DynamicJsonDocument doc(SEQ_BYTES);
    deserializeJson(doc, file.readString());
    Serial.println("Deserialized JSON");
    file.close();
    //seq = Sequence(doc);
    return true;
}
bool SequenceFilesystem::saveSequence(String name, Sequence& seq)
{
    String path = "/" + name + ".json";
    if (fileSys->exists(path.c_str()))
        Serial.println("File found"); 
    else
        Serial.println("Creating file...");
    
    auto file = fileSys->open(path.c_str(), FILE_WRITE, !fileSys->exists(path.c_str()));
 
    Serial.println("File opened");
    auto doc = seq.getJsonDocument(name.c_str());
    Serial.println("Created JSON document");
    String data = "";
    serializeJson(doc, data);
    if (file.print(data))
    {
        Serial.println("Serialized JSON sequence");
        std::string lStr = "File " + std::string(name.c_str()) + " has " + std::to_string(file.size()) + " bytes"; 
        Serial.print(data);
        file.close();
        return true;
    }
    file.close();
    return false;
}

bool SequenceFilesystem::isValidSequence(File& file)
{


    return false;
}