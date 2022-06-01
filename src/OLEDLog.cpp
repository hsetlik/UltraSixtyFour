#include "OLEDLog.h"

void OLEDLog::printToDisplay(std::string str, Adafruit_SSD1306* display)
{
    log.push_back(str);
    
}