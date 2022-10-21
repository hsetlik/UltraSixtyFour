#include "UserStringInput.h"

UserStringInput::UserStringInput() : idx(0),
                                     current("")
{
}

UserStringInput::~UserStringInput()
{
}

void UserStringInput::next()
{
    idx = (idx + 1) % allowed.size();
}

void UserStringInput::prev()
{
    idx = (idx == 0) ? (allowed.size() - 1) : idx - 1;
}

void UserStringInput::enterChar()
{
    current += allowed[idx];
}

void UserStringInput::backspace()
{
    if (current.length() > 0)
        current.erase(current.end() - 1);
}

std::string UserStringInput::getString()
{
    return current;
}

void UserStringInput::getVisibleChars(std::string &str)
{
    str = "";
    int start = idx - 3;
    if (start < 0)
        start += allowed.size();
    int i = 0;
    while (i < NUM_CHARS_VISIBLE)
    {
        start = (start + 1) % allowed.size();
        str += allowed[start];
    }
}

void UserStringInput::render(Adafruit_SSD1306 *disp)
{
    bool cursorVisible = (millis() % 1000) <= 500; //set the cursor to blink every 1/2 second
    disp->clearDisplay();
    disp->setCursor(5, 0);
    disp->setTextSize(1);
    static std::string currentChars = "";
    getVisibleChars(currentChars);
    disp->print(currentChars.c_str());
    if (cursorVisible)
    {
        disp->setCursor(29, 12);
        disp->drawLine(29, 12, 11, 12, 1); // underline the current char
    }
    disp->setCursor(5, 13);
    auto currentStr = "Current: " + current;
    disp->println(currentStr.c_str());
    disp->display();
}