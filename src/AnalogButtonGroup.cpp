#include "AnalogButtonGroup.h"

AnalogButton::AnalogButton(uint8_t id, uint16_t v) : 
index(id), 
voltage(v),
isPressed(false),
pressedAt(0),
holdLength(300),
fPress(0),
fHold(0)
{

}
AnalogButton::AnalogButton() : 
index(0), 
voltage(0),
isPressed(false),
pressedAt(0),
holdLength(300),
fPress(0),
fHold(0)
{

}
//===============================================================    
AnalogButtonGroup::AnalogButtonGroup(uint8_t p, uint8_t n) :
pin(p),
numButtons(n),
lastButton(nullptr)
{
    buttons = new AnalogButton[numButtons];
    now = millis();
}

void AnalogButtonGroup::update()
{
    if (millis() - now >= MIN_INTERVAL)
    {
        now = millis();
        auto reading = analogRead(pin);
        auto idx = idxForVoltage(reading);
        AnalogButton* button = (idx == -1) ? nullptr : &buttons[idx];
        //if a button has been pressed or released
        if (lastButton != button)
        {
            //lastButton has been released
            
            if (lastButton != nullptr)
            {
                lastButton->isPressed = false;
                if (now - lastButton->pressedAt < button->holdLength)
                {
                    lastButton->fPress(lastButton->index);
                }
            }
            if (button != nullptr)
            {
                /*
                Serial.print("Button ");
                Serial.print(button->index);
                Serial.print(" pressed\n");
                */
                button->pressedAt = millis();
                button->isPressed = true;
            }
            lastButton = button;
        }
        //if the same button is held, check if we need to run the hold callback
        else if (button != nullptr)
        {
            if (millis() - button->pressedAt >= button->holdLength)
            {
                button->fHold(button->index);
                button->pressedAt = millis();
            }
        }
    }
}
    

int8_t AnalogButtonGroup::idxForVoltage(uint16_t voltage)
{
    for (int8_t i = 0; i < numButtons; ++i)
    {
        if (buttons[i].voltage + MV_MARGIN > voltage && buttons[i].voltage - MV_MARGIN <= voltage)
            return i;
    }
    return -1;
}
void AnalogButtonGroup::addButton(AnalogButton& button)
{
    buttons[button.index] = button;
    addedButtons[button.index] = true;
}