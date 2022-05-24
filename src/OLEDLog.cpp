#include "OLEDLog.h"

void OLEDLog::printToDisplay(std::string str, Adafruit_SSD1306* display, std::deque<std::string>& log)
{
    /*
    log.push_front(str);
    //remove the oldest logs if we're past the limit
    if (log.size() >= LOG_LENGTH)
        log.pop_back();
    */
    //grab the strings we need to display and clear the screen
    //uint32_t lines = (log.size() < MAX_DISPLAY_LINES) ? log.size() : MAX_DISPLAY_LINES;
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setTextWrap(true);
    /*
    for(uint8_t i = 0; i < lines; ++i)
    {
        auto str = log[i];
        display->setCursor(5, i * 8);
        display->println(F(str.c_str()));
        
    }
    */
   /*
    for (auto i = 0; i < lines; ++i)
    {
        if (log[i].length() * 6 > 128)
            display->startscrollright(0x00, 0x0F);
    }
    */
    display->setCursor(5, 0);
    display->println(str.c_str());
    display->display();
}