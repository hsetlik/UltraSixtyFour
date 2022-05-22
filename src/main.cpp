#include <Arduino.h>
#include "Sequencer.h"
#include <Encoder.h>
#include "OLEDLog.h"
#include <memory>
#include <RotaryEncoder.h>


std::unique_ptr<Sequencer> seq(nullptr);


std::deque<std::string> logDeque;
void OLEDLog::println(std::string str)
{
  OLEDLog::printToDisplay(str, seq->getDisplay(), logDeque);
}
AnalogButtonGroup groupA(BUTTONS1, 6);

AnalogButton encAButton(0, 487); //TODO: measure and set all these voltage readings
AnalogButton encBButton(1, 1095);
AnalogButton encCButton(2, 1700);
AnalogButton encDButton(3, 2300);

AnalogButton pgLeft(4, 2895);
AnalogButton pgRight(5, 3555);

AnalogButton* aButtons[] = {&encAButton, &encBButton, &encCButton, &encDButton, &pgLeft, &pgRight};

AnalogButtonGroup groupB(BUTTONS2, 7);

AnalogButton trk1Button(0, 3024);
AnalogButton trk2Button(1, 2275);
AnalogButton trk3Button(2, 1765);
AnalogButton trk4Button(3, 1375);

AnalogButton leftButton(4, 350);
AnalogButton rightButton(5, 690);
AnalogButton playButton(6, 1175);

AnalogButton* bButtons[] = {&trk1Button, &trk2Button, &trk3Button, &trk4Button, &leftButton, &rightButton, &playButton};

void handlePress(uint8_t idx) { seq->buttonPressed(idx); }
void handleHold(uint8_t idx) { seq->buttonHeld(idx); }

//Encoder stuff=======================
long pos[] = {0, 0, 0, 0};

RotaryEncoder encA(ADATA, ACLK, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encB(BDATA, BCLK, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encC(CDATA, CCLK, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encD(DDATA, DCLK, RotaryEncoder::LatchMode::FOUR3);

RotaryEncoder* encoders[] = {&encA, &encB, &encC, &encD};

void handleEncoderMove(uint8_t idx, bool dir)
{
  seq->encoderTurned(idx, dir);
}

void pollEncoders()
{
  for(byte i = 0; i < 4; i++)
  {
    encoders[i]->tick();
    long newPos =  encoders[i]->getPosition();
    if (newPos != pos[i])
    {
      //OLEDLog::println("encoder moved");
      handleEncoderMove(i, newPos > pos[i]);
      pos[i] = newPos;
    }
  }
}
//======Button Stuff============
void initButtons()
{
  for (byte i = 0; i < 6; ++i)
  {
    auto& button = *aButtons[i];
    button.onPress(handlePress)
    .onHold(handleHold, 800);
    groupA.addButton(button);
  }
  for (byte i = 0; i < 7; ++i)
  {
    auto& button = *bButtons[i];
    button.onPress(handlePress)
    .onHold(handleHold, 800);
    groupB.addButton(button);
  }
}
//====================================
const char* ssid = "SD Airport";
const char* password = "plinsky1737";

//AsyncWebServer server(80);

void testOLEDLog()
{
  for(byte i = 0; i < 10; ++i)
  {
    OLEDLog::println("Line number " + std::to_string(i));
  }
}


// button handling
void setup() 
{
  Serial.begin(115200);
  seq.reset(new Sequencer());
  initButtons();
  OLEDLog::println("Sequencer initialized. . .");
}

unsigned long idx = 0;

void loop()
{
  seq->loop();
  pollEncoders();
 
  groupA.update();
  //groupB.update();
}