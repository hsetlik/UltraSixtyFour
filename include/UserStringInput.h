#ifndef USERSTRINGINPUT_H
#define USERSTRINGINPUT_H
#include <string>
#include "Adafruit_SSD1306.h"
#define NUM_CHARS_VISIBLE 7

class UserStringInput
{
private:
    int idx;
    /* data */
    std::string current;
    void getVisibleChars(std::string& str);
public:
    UserStringInput(/* args */);
    ~UserStringInput();
    const std::string allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    void next();
    void prev();
    void enterChar();
    void backspace();
    std::string getString();
    void render(Adafruit_SSD1306* disp);
};
#endif