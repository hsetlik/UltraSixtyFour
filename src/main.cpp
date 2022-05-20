#include <Arduino.h>
#include "Sequencer.h"
#include <Encoder.h>

Sequencer* pSeq;
Sequencer& seq = *pSeq;
/*
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

void handlePress(uint8_t idx) { seq.buttonPressed(idx); }
void handleHold(uint8_t idx) { seq.buttonHeld(idx); }

//Encoder stuff=======================
static long pos[] = {0, 0, 0, 0};

Encoder encA(ADATA, ACLK);
Encoder encB(BDATA, BCLK);
Encoder encC(CDATA, CCLK);
Encoder encD(DDATA, DCLK);

Encoder* encoders[] = {&encA, &encB, &encC, &encD};

void handleEncoderMove(uint8_t idx, bool dir)
{
  seq.encoderTurned(idx, dir);
}

void pollEncoders()
{
  for(byte i = 0; i < 4; i++)
  {
    long newPos = encoders[i]->read();
    if (newPos != pos[i])
    {
      handleEncoderMove(i, newPos > pos[i]);
      pos[i] = newPos;
    }
  }
}
*/

//====================================
const char* ssid = "SD Airport";
const char* password = "plinsky1737";

//AsyncWebServer server(80);



// button handling
void setup() 
{
  Serial.begin(115200);
  /*
  Serial.println("Starting setup");
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
  */
  Serial.println("buttons set up");
  pSeq = new Sequencer();
  Serial.println("Sequencer created");
}

uint16_t aValues[100] = {0};
uint16_t bValues[100] = {0};
byte idx = 0;

void loop()
{
  //pollEncoders();
  //groupA.update();
  //groupB.update();
  //seq.loop();
  /*
  aValues[idx] = analogRead(BUTTONS1);
  bValues[idx] = analogRead(BUTTONS2);
  ++idx;
  if (idx == 100)
  {
    uint32_t aTotal = 0;
    uint32_t bTotal = 0;
    for (auto i = 0; i < 100; ++i)
    {
      aTotal += aValues[i];
      bTotal += bValues[i];
    }
    auto aMean = aTotal / 100;
    auto bMean = bTotal / 100;

    Serial.print("A mean: ");
    Serial.println(aMean);
    Serial.println("=====================");
    Serial.print("B mean: ");
    Serial.println(bMean);
    Serial.println("=====================");
    idx = 0;
  }
  */
}