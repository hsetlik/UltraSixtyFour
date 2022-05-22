#include <Arduino.h>
#include "Sequencer.h"
#include "OLEDLog.h"
#include <memory>
#include <RotaryEncoder.h>
#include <AceButton.h>

using ace_button::AceButton;
using ace_button::ButtonConfig;
using ace_button::LadderButtonConfig;

std::unique_ptr<Sequencer> seq(nullptr);


std::deque<std::string> logDeque;
void OLEDLog::println(std::string str)
{
  OLEDLog::printToDisplay(str, seq->getDisplay(), logDeque);
}
//=====Button Stuff======================
void handlePress(uint8_t idx) { seq->buttonPressed(idx); }
void handleHold(uint8_t idx) { seq->buttonHeld(idx); }

//Group A
const byte aNumButtons = 7;
static AceButton trk1(nullptr, 0);
static AceButton trk2(nullptr, 1);
static AceButton trk3(nullptr, 2);
static AceButton trk4(nullptr, 3);
static AceButton play(nullptr, 4);
static AceButton menuLeft(nullptr, 5);
static AceButton menuRight(nullptr, 6);

static AceButton* const aButtons[] = {&trk1, &trk2, &trk3, &trk4, &play, &menuLeft, &menuRight};
//measured voltage levels for each button
static const uint16_t aLevels[] = {307, 708, 1113, 1519, 1962, 2473, 3136};

static LadderButtonConfig aButtonConfig(BUTTONS1, aNumButtons, aLevels, aNumButtons, aButtons);

// Group B

//Group A
const byte bNumButtons = 6;
static AceButton e1(nullptr, 0);
static AceButton e2(nullptr, 1);
static AceButton e3(nullptr, 2);
static AceButton e4(nullptr, 3);
static AceButton lPage(nullptr, 4);
static AceButton rPage(nullptr, 5);

static AceButton* const bButtons[] = {&e1, &e2, &e3, &e4, &lPage, &rPage};
static AceButton* const allButtons[] = {&trk1, &trk2, &trk3, &trk4, &play, &menuLeft, &menuRight, &e1, &e2, &e3, &e4, &lPage, &rPage};
//measured voltage levels for each button
static const uint16_t bLevels[] = {482, 1117, 1748, 2386, 2896, 3553};

static LadderButtonConfig bButtonConfig(BUTTONS2, bNumButtons, bLevels, bNumButtons, bButtons);

static unsigned long numClicks = 0;
// common button stuff
void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState)
{
  if (eventType == AceButton::kEventClicked)
  {
    // button->getId doesn't work for bc we're using a shared config so we need to check the array
    uint8_t id = 0;
    for (byte i = 0; i < aNumButtons + bNumButtons; ++i)
    {
      if (allButtons[i] == button)
        {
          handlePress(i);
          return;
        }
    }
  } else if (eventType == AceButton::kEventLongPressed)
  {
    for (byte i = 0; i < aNumButtons + bNumButtons; ++i)
    {
      if (allButtons[i] == button)
        {
          handleHold(i);
          return;
        }
    }
  }
}

void checkButtons()
{
  static uint16_t prev = millis();
  uint16_t now = millis();
  if ((uint16_t) (now - prev) >= 3) 
  {
    prev = now;
    aButtonConfig.checkButtons();
    bButtonConfig.checkButtons();
  }
}

void initButtons()
{
  pinMode(BUTTONS1, INPUT);
  pinMode(BUTTONS2, INPUT);
  aButtonConfig.setEventHandler(handleEvent);
  aButtonConfig.setFeature(ButtonConfig::kFeatureClick);
  aButtonConfig.setFeature(ButtonConfig::kFeatureDoubleClick);
  aButtonConfig.setFeature(ButtonConfig::kFeatureLongPress);
  aButtonConfig.setFeature(ButtonConfig::kFeatureRepeatPress);


  bButtonConfig.setEventHandler(handleEvent);
  bButtonConfig.setFeature(ButtonConfig::kFeatureClick);
  bButtonConfig.setFeature(ButtonConfig::kFeatureDoubleClick);
  bButtonConfig.setFeature(ButtonConfig::kFeatureLongPress);
  bButtonConfig.setFeature(ButtonConfig::kFeatureRepeatPress); 
  Serial.println("buttons initialized");
}
//======================================
//Encoder stuff========================
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
  checkButtons();
  //groupA.update();
  //groupB.update();
}