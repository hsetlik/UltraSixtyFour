#include <Arduino.h>
#include "Sequencer.h"
#include "OLEDLog.h"
#include <memory>
#include <RotaryEncoder.h>
#include <AceButton.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <MCP_DAC.h>
using ace_button::AceButton;
using ace_button::ButtonConfig;
using ace_button::LadderButtonConfig;

#define DAC_UPDATE_MICROS 2500

MCP4822 dac1;
MCP4822 dac2;


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
  
    for (byte i = 0; i < aNumButtons + bNumButtons; ++i)
    {
      if (allButtons[i] == button)
        {
          if (eventType == AceButton::kEventClicked)
            handlePress(i);
          else if (eventType == AceButton::kEventLongPressed)
            handleHold(i);
          return;
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
    bButtonConfig.checkButtons();
    aButtonConfig.checkButtons();
  }
}

//=================DAC stuff===========================================

void initDACs()
{
  dac1.selectVSPI();
  dac1.begin(DAC1_PIN);
  dac1.setGain(2);

  dac2.selectVSPI();
  dac2.begin(DAC2_PIN);
  dac2.setGain(2);
}

void runDACTest()
{
  while(millis() < 10000)
  {
    for (uint16_t i = 0; i < 64; ++i)
    {
      uint16_t level = (4095 / 64) * i;
      dac1.fastWriteA(level);
      dac2.fastWriteB(level);
      dac2.fastWriteA(level);
      dac2.fastWriteB(level);
      delay(5);
    }
  }
}

void updateDACs()
{
  static unsigned long lastUpdated = 0;
  static uint16_t level = 0;
  auto now = micros();
  if (now - lastUpdated > DAC_UPDATE_MICROS)
  {
    lastUpdated = now;
    level = (level + 90) % 4095;
    dac1.analogWrite(level, 0);
    dac1.analogWrite(level, 1);
    dac2.analogWrite(level, 0);
    dac2.analogWrite(level, 1);
    std::string lStr = "Level: " + std::to_string(level);
    Serial.println(lStr.c_str());
  }
}


//=======================================================================

void initButtons()
{
  pinMode(BUTTONS1, INPUT);
  pinMode(BUTTONS2, INPUT);
  aButtonConfig.setEventHandler(handleEvent);
  aButtonConfig.setFeature(ButtonConfig::kFeatureClick);
  aButtonConfig.setFeature(ButtonConfig::kFeatureLongPress);
  aButtonConfig.setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
  aButtonConfig.setLongPressDelay(800);


  bButtonConfig.setEventHandler(handleEvent);
  bButtonConfig.setFeature(ButtonConfig::kFeatureClick);
  bButtonConfig.setFeature(ButtonConfig::kFeatureLongPress);
  bButtonConfig.setFeature(ButtonConfig::kFeatureSuppressAfterClick);
  bButtonConfig.setLongPressDelay(800);
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
//==============Server Stuff============


std::string ssid = "SD Airport";
std::string password = "plinsky1737";
AsyncWebServer server(80);


 void initWifi()
 {
    WiFi.mode(WIFI_STA);
    
  auto res = WiFi.begin(ssid.c_str(), password.c_str());
  if (res == WL_CONNECT_FAILED)
  {
    //OledLog::writeLn("Connection failed");
  } else if (res == WL_NO_SSID_AVAIL)
  {
    //OledLog::writeLn("Network " + ssid + " not available");
  }
  Serial.println(res);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  auto ip = WiFi.localIP();
  std::string logString = "Connected to " +  ssid + " with IP address " + ip.toString().c_str();
  Serial.println(logString.c_str());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });
  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();

 }
//====================================

void setup() 
{
  Serial.begin(115200);
  seq.reset(new Sequencer());
  initButtons();
  initWifi();
  initDACs();
  //runDACTest();
  OLEDLog::println("Wifi Initialized");
}

unsigned long idx = 0;

void loop()
{
  seq->loop();
  updateDACs();
  pollEncoders();
  checkButtons();
}