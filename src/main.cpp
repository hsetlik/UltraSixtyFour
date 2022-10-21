#define LED_REFRESH_HZ 20
/*===============================================================
We can divide program into 3 tasks:
1. Poll inputs- as frequently as possible
2. Check sequence advance- speed requirements TBD
3. Update Gates and DACs- as frequently as possible
4. Update LEDs- no faster than some frame rate
5. Update display- no faster than some frame rate

================================================================
    NOTE: Task Scheduler definitions need to go here before the #include
*/
// #define _TASK_TIMECRITICAL      // Enable monitoring scheduling overruns
//#define _TASK_SLEEP_ON_IDLE_RUN // Enable 1 ms SLEEP_IDLE powerdowns between tasks if no callback methods were invoked during the pass
//#define _TASK_STATUS_REQUEST    // Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
// #define _TASK_WDT_IDS           // Compile with support for wdt control points and task ids
// #define _TASK_LTS_POINTER       // Compile with support for local task storage pointer
// #define _TASK_PRIORITY          // Support for layered scheduling priority
// #define _TASK_MICRO_RES         // Support for microsecond resolution
// #define _TASK_STD_FUNCTION      // Support for std::function (ESP8266 and ESP32 ONLY)
// #define _TASK_DEBUG             // Make all methods and variables public for debug purposes
// #define _TASK_INLINE            // Make all methods "inline" - needed to support some multi-tab, multi-file implementations
// #define _TASK_TIMEOUT           // Support for overall task timeout
// #define _TASK_OO_CALLBACKS      // Support for dynamic callback method binding
#include <TaskScheduler.h>
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
#include "OLEDDriver.h"


// Forward declared callbacks
void pollInputsCallback();

void updateDisplayCallback();

void updatePixelsCallback();

void autosaveCallback();

void setupScheduler();

Scheduler scheduler;

Task tUpdatePixels((1000 / LED_REFRESH_HZ) * TASK_MILLISECOND, TASK_FOREVER, &updatePixelsCallback, &scheduler, true);
Task tPollInputs(TASK_IMMEDIATE, TASK_FOREVER, &pollInputsCallback, &scheduler, true);
Task tUpdateDisplay((1000 / OLED_REFRESH_HZ) * TASK_MILLISECOND, TASK_FOREVER, &updateDisplayCallback, &scheduler, true);
Task tAutosave(40 * TASK_SECOND, TASK_FOREVER, &autosaveCallback, &scheduler, true);
//==========================================================
using ace_button::AceButton;
using ace_button::ButtonConfig;
using ace_button::LadderButtonConfig;

std::unique_ptr<Sequencer> seq(nullptr);

std::unique_ptr<OLEDDriver> displayDriver(nullptr);

std::deque<std::string> logDeque;

//=====Button Stuff======================
void handlePress(uint8_t idx) { seq->buttonPressed(idx); }
void handleHold(uint8_t idx) { seq->buttonHeld(idx); }

// Group A
const byte aNumButtons = 7;
static AceButton trk1(nullptr, 0);
static AceButton trk2(nullptr, 1);
static AceButton trk3(nullptr, 2);
static AceButton trk4(nullptr, 3);
static AceButton play(nullptr, 4);
static AceButton menuLeft(nullptr, 5);
static AceButton menuRight(nullptr, 6);

static AceButton *const aButtons[] = {&trk1, &trk2, &trk3, &trk4, &play, &menuLeft, &menuRight};
// measured voltage levels for each button
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

static AceButton *const bButtons[] = {&e1, &e2, &e3, &e4, &lPage, &rPage};
static AceButton *const allButtons[] = {&trk1, &trk2, &trk3, &trk4, &play, &menuLeft, &menuRight, &e1, &e2, &e3, &e4, &lPage, &rPage};
// measured voltage levels for each button
static const uint16_t bLevels[] = {482, 1117, 1748, 2386, 2896, 3553};

static LadderButtonConfig bButtonConfig(BUTTONS2, bNumButtons, bLevels, bNumButtons, bButtons);
// common button stuff
void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState);
// Encoder stuff========================
long pos[] = {0, 0, 0, 0};

RotaryEncoder encA(ADATA, ACLK, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encB(BDATA, BCLK, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encC(CDATA, CCLK, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encD(DDATA, DCLK, RotaryEncoder::LatchMode::FOUR3);

RotaryEncoder *encoders[] = {&encA, &encB, &encC, &encD};
//======================================
void OLEDLog::println(const char *str)
{
    displayDriver->addMessage(str);
}

//==============Server Stuff============
std::string ssid = "SD Airport";
std::string password = "plinsky1737";
AsyncWebServer server(80);
void initWifi();
// to be accessed from main.cpp
void handleEncoderMove(uint8_t idx, bool dir)
{
    seq->encoderTurned(idx, dir);
}
void pollInputsCallback()
{
    for (byte i = 0; i < 4; i++)
    {
        encoders[i]->tick();
        long newPos = encoders[i]->getPosition();
        if (newPos != pos[i])
        {
            // OLEDLog::println("encoder moved");
            handleEncoderMove(i, newPos > pos[i]);
            pos[i] = newPos;
        }
    }
    static uint16_t prev = millis();
    uint16_t now = millis();
    if ((uint16_t)(now - prev) >= 3)
    {
        prev = now;
        bButtonConfig.checkButtons();
        aButtonConfig.checkButtons();
    }
}

void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState)
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
//===================UPDATE PIXELS====================
void updatePixelsCallback()
{
#if USE_NEOPIXELS
    seq->updateLeds();
#endif
}
//==================UPDATE DISPLAY====================
void updateDisplayCallback()
{
    displayDriver->update();
}
//==================AUTOSAVE=============
void autosaveCallback()
{
    seq->autosave();
}
//=======================GENRAL SETUP STUFF============
void initWifi()
{
    WiFi.mode(WIFI_STA);

    auto res = WiFi.begin(ssid.c_str(), password.c_str());
    if (res == WL_CONNECT_FAILED)
    {
        OLEDLog::println("Connection failed");
    }
    else if (res == WL_NO_SSID_AVAIL)
    {
        // OledLog::writeLn("Network " + ssid + " not available");
    }
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
        // Serial.print(".");
    }
    // Serial.println();
    auto ip = WiFi.localIP();
    OLEDLog::println(ip.toString().c_str());
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", "Hi! I am ESP32."); });
    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    server.begin();
    if (res != WL_CONNECT_FAILED)
        OLEDLog::println("Connection succeded");
}

void setupScheduler()
{
    Serial.begin(115200);

    displayDriver.reset(new OLEDDriver());
    seq.reset(new Sequencer());
    // seq->loadAutosaved();
    initWifi();
    // Set up buttons
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
}
void OLEDLog::println(std::string str)
{
    displayDriver->addMessage(str);
}

void setup()
{
    Serial.begin(115200);
    setupScheduler();
    OLEDLog::println("Setting up scheduler...");
    // seq->loadAutosaved();
}
void loop()
{
    scheduler.execute();
    seq->updateDACs();
    seq->updateGates();
    seq->checkAdvance();
}