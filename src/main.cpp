#include <Arduino.h>
#include "Sequencer.h"

Sequencer seq;

AnalogButtonGroup groupA(BUTTONS1, 6);

AnalogButton encAButton(0, 1); //TODO: measure and set all these voltage readings
AnalogButton encBButton(1, 1);
AnalogButton encCButton(2, 1);
AnalogButton encDButton(3, 1);

AnalogButton pgLeft(4, 1);
AnalogButton pgRight(5, 1);

AnalogButton* aButtons[] = {&encAButton, &encBButton, &encCButton, &encDButton, &pgLeft, &pgRight};

AnalogButtonGroup groupB(BUTTONS2, 7);

AnalogButton trk1Button(0, 1);
AnalogButton trk2Button(1, 1);
AnalogButton trk3Button(2, 1);
AnalogButton trk4Button(3, 1);

AnalogButton leftButton(4, 1);
AnalogButton rightButton(5, 1);
AnalogButton playButton(6, 1);

AnalogButton* bButtons[] = {&trk1Button, &trk2Button, &trk3Button, &trk4Button, &leftButton, &rightButton};

void handlePress(uint8_t idx) { seq.buttonPressed(idx); }
void handleHold(uint8_t idx) { seq.buttonHeld(idx); }


// button handling


void setup() 
{
  seq.setup();
  for (uint8_t i = 0; i < 6; ++i)
  {
    aButtons[i]->onPress(handlePress)
    .onHold(handleHold, 500);
    groupA.addButton(*aButtons[i]);
  }

  for (uint8_t i = 0; i < 7; ++i)
  {
    bButtons[i]->onPress(handlePress)
    .onHold(handleHold, 500);
    groupB.addButton(*bButtons[i]);
  }



  // put your setup code here, to run once:
}

void loop() 
{
  seq.loop();
  // put your main code here, to run repeatedly:
}