#include "SpiffsEnvironment.h"

SpiffsEnvironment::SpiffsEnvironment(Sequencer* s) : seq(s),
                                                    lastAutosavedAt(0)
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("Failed to initialize SPIFFS!");
    }
    Serial.println("SPIFFS started");
}

SpiffsEnvironment::~SpiffsEnvironment()
{
}
// attempts to save the current sequence, returns success or failure.
bool SpiffsEnvironment::saveCurrentSequenceAs(std::string name)
{
    std::string path = "/" + name + ".json";
    //get the sequence's JSON representation
    auto file = SPIFFS.open(path.c_str(), FILE_WRITE);
    if (!file)
    {
        Serial.println("Could not create file!");
        return false;
    }
    auto jsonDoc = seq->currentSequence->getJsonDocument(name);
    std::string data = "";
    serializeJson(jsonDoc, data);
    bool success = file.print(data.c_str());
    file.close();
    if (success)
    {
        Serial.println("File written");
        return true;
    } else
    {
        Serial.println("Failed to write file!");
        return false;
    }
}
// attempts to locate and load the sequence with a matching name. returns success or failure.
bool SpiffsEnvironment::loadSequence(std::string name)
{
    auto path = "/" + name + ".json";

    auto file = SPIFFS.open(path.c_str(), FILE_READ);
    if (!file)
    {
        Serial.println("Could not open file!");
    }
    //read file
    auto len = file.size();
    auto buffer = new char[len];
    size_t i = 0;
    while (len)
    {
        auto toRead = len;
        if (toRead > 512)
        {
            toRead = 512;
        }
        file.readBytes(buffer, toRead);
        if ((i++ & 0x001F) == 0x001F)
        {
            Serial.print(".");
        }
        len -= toRead;
    }
    Serial.println();
    //put buffer into JSON document
    SeqJson doc;
    deserializeJson(doc, buffer, file.size());
    


}
// call this in the main loop to check if it's time to autosave the current sequence
void SpiffsEnvironment::checkAutosave()
{
}
