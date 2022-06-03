#include "OLEDDriver.h"

OLEDDriver::OLEDDriver(/* args */) :
display(DISP_WIDTH, DISP_HEIGHT),
logMessages({"_unused"}),
needsUpdate(false)
{
    Wire.begin(SDA, SCL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, DISP_ADDRESS))
    {
        for (;;)
            ;
    }
    display.clearDisplay();
    display.setCursor(5, 5);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.println("ULTRA 64");
    display.display();
}

OLEDDriver::~OLEDDriver()
{
}

void  OLEDDriver::shiftForward()
{
    for(byte i = 1; i < MAX_LOG_MESSAGES; i++)
        logMessages[i - 1] = logMessages[i];
}
    
void OLEDDriver::addMessage(std::string str)
{
    //shift the previous newest message forward to make room
    shiftForward();
    logMessages[MAX_LOG_MESSAGES - 1] = str;
    needsUpdate = true;
}

void OLEDDriver::update()
{
    if (!needsUpdate)
        return;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    auto numMessages = numSpacesUsed();
    for (byte i = (MAX_LOG_MESSAGES - numMessages); i < MAX_LOG_MESSAGES; ++i)
    {
        display.setCursor(5, 8 * i);
        display.println(logMessages[i].c_str());
        bool needsScroll = logMessages[i].length() * 6 > DISP_WIDTH;
        if (needsScroll)
            display.startscrollleft(8 * i, (8 * i) + 8);
    }
    display.display();
    needsUpdate = false;
}

uint8_t OLEDDriver::numSpacesUsed()
{
    uint8_t used = MAX_LOG_MESSAGES;
    for (auto& m : logMessages)
    {
        if (m == "_unused")
            --used;
    }
    return used;
}