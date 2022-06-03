#ifndef OLEDDRIVER_H
#define OLEDDRIVER_H
#include <Adafruit_SSD1306.h>
#include <string>
#include <array>
//I2C pins
#define SDA 17
#define SCL 5

#define DISP_WIDTH 128
#define DISP_HEIGHT 32
#define DISP_ADDRESS 0x3C

#define MAX_LOG_MESSAGES 4

#define OLED_REFRESH_HZ 30

class OLEDDriver
{
private:
    /* data */
    Adafruit_SSD1306 display;
    std::array<std::string, MAX_LOG_MESSAGES> logMessages;
    bool needsUpdate;

    void shiftForward();
    uint8_t numSpacesUsed();
public:
    OLEDDriver(/* args */);
    ~OLEDDriver();
    void addMessage(std::string str);
    void update();

};
#endif // !OLEDDRIVER_H