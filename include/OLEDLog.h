#ifndef OLEDLOG_H
#define OLEDLOG_H
#include <string>
#include <deque>
#include <Adafruit_SSD1306.h>

#define LOG_LENGTH 500
#define MAX_DISPLAY_LINES 4

struct OLEDLog
{
public:
    //NOTE: this needs to be implemented in main.cpp with the actual display object
    static void println(std::string str);
    static void println(const char* str)
    {
        println(std::string(str));
    }
    static void printToDisplay(std::string str, Adafruit_SSD1306* display, std::deque<std::string>& log);
private:
};

#endif // !OLEDLOG_H


