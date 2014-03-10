///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
////
//// A V R G A M E    C O R E
//// 
//// The main sketch file for the AVRGAME project
//// 
//// FIRMWARE RELEASES
//// 
//// Rev 1.0 27jan13 Initial release
//// Rev 1.1 10feb13 Bug fixes
//// Rev 2.0 06mar13 Display dimming, Display invert, 8-bits per pixel mode
//// Rev 2.1 19mar13 8-bits per pixel mode bug fix
//// Rev 2.2 14feb14 minor cosmetic tweaks
//// Rev 2.3 23feb14 adding Stephen Cropp's Snake Game to main build
//// Rev 2.4 10mar14 crappy bird!
////
#define VERSION_HI 2
#define VERSION_LO 4
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <Tone.h>
#include <EEPROM.h>

#include "AVRGame.h"
#include "Menu.h"

///////////////////////////////////////////////////////////////////////////
//
// include games here
//
#include "InvadersGame.h"
#include "GhostMazeGame.h"
#include "BreakoutGame.h"
#include "FourInARowGame.h"
#include "MemoryGame.h"
#include "SnakeGame.h"
#include "BirdGame.h"

//
///////////////////////////////////////////////////////////////////////////

// Global variables
Disp8x8Class Disp8x8;
CGameFactory *gameFactory[MAX_GAMES] = {0};
byte numGameFactories = 0;
CGame *pGame = NULL;
Tone speaker;
byte soundOn = 0;
char thisGame = -1;
char nextGame = -1;
byte heartbeat = 0;
unsigned long gameScore;
char firstButtonPress = 1;
unsigned long menuSelectKeyPressed = 0;
unsigned long timeButtonAPress = 0;
unsigned long timeButtonBPress = 0;
unsigned long timeButtonCPress = 0;
unsigned long timeButtonDPress = 0;
unsigned long nextTimer1Event;
unsigned long nextTimer2Event;
unsigned long nextTimer3Event;
unsigned long nextTimer4Event;
unsigned long nextTimer5Event;
unsigned int Timer1Period;
unsigned int Timer2Period;
unsigned int Timer3Period;
unsigned int Timer4Period;
unsigned int Timer5Period;

///////////////////////////////////////////////////////////////////////////
// getMenuIcon
// Defines the content of the game menu. Any new games need to be hooked into
// the menu via this function
void getMenuIcon(int which, byte *dst, byte count)
{
  memset(dst,0,16);
  if(which>=numGameFactories)
    which=0;
  gameFactory[which]->getGameIcon(dst, count);
}

///////////////////////////////////////////////////////////////////////////
// startGame
// Creates an instance of a game class and starts it running
void startGame(byte which)
{
  if(which>=numGameFactories)
    which=0;
  thisGame = which;
  EEPROM.write(EEPROM_GAMESELECTED,thisGame);
  pGame = gameFactory[thisGame]->createInstance();
  
  nextTimer1Event = 0;
  nextTimer2Event = 0;
  nextTimer3Event = 0;
  nextTimer4Event = 0;
  nextTimer5Event = 0;
  Timer1Period = 0;
  Timer2Period = 0;
  Timer3Period = 0;
  Timer4Period = 0;
  Timer5Period = 0;
  gameScore = 0;
  pGame->init();
  pGame->handleEvent(EV_START);
}

///////////////////////////////////////////////////////////////////////////
// setNextGame
// Called from the Menu to select a game to start up
void setNextGame(int which)
{
  nextGame = which;
}

///////////////////////////////////////////////////////////////////////////
// initSound
// Simple wrapper around the Tone library to first check if sound is 
// enabled by the user
void initSound()
{
   speaker.begin(P_SPK);
   soundOn = EEPROM.read(EEPROM_SOUNDON);
}

///////////////////////////////////////////////////////////////////////////
// playSound
// Simple wrapper around the Tone library to first check if sound is 
// enabled by the user
inline void playSound(int pitch, int dur)
{
  if(soundOn)
    speaker.play(pitch,dur);
}

///////////////////////////////////////////////////////////////////////////
// setSoundOn
void setSoundOn(byte on)
{  
  soundOn = on;
  // remember the setting in EEPROM
  EEPROM.write(EEPROM_SOUNDON,soundOn);
}

///////////////////////////////////////////////////////////////////////////
// isSoundOn
byte isSoundOn()
{
  return soundOn;
}

///////////////////////////////////////////////////////////////////////////
// getDigit
// insert a digit 0-9 (defined in a 5x4 grid) into a byte buffer as 
// either the left or right 4 columns
void getDigit(byte which, byte side, byte *data)
{
    byte d0=0,d1=0,d2=0;
    switch(which)
    {
        // set up the character design for the digit (3 bytes contain the 5 nybbles 
        // that define the 5x4 charater)
        case 0: d0=0b01110101; d1=0b01010101; d2=0b01110000; break;
        case 1: d0=0b00100110; d1=0b00100010; d2=0b01110000; break;
        case 2: d0=0b01110001; d1=0b01110100; d2=0b01110000; break;
        case 3: d0=0b01110001; d1=0b00110001; d2=0b01110000; break;
        case 4: d0=0b01010101; d1=0b01110001; d2=0b00010000; break;
        case 5: d0=0b01110100; d1=0b01110001; d2=0b01110000; break;
        case 6: d0=0b01110100; d1=0b01110101; d2=0b01110000; break;
        case 7: d0=0b01110001; d1=0b00100100; d2=0b01000000; break;
        case 8: d0=0b01110101; d1=0b01110101; d2=0b01110000; break;
        case 9: d0=0b01110101; d1=0b01110001; d2=0b00010000; break;
    }
    
    // extract and shift byte values as needed
    byte o0=(d0>>4)&0x0f;
    byte o1=(d0)&0x0f;
    byte o2=(d1>>4)&0x0f;
    byte o3=(d1)&0x0f;
    byte o4=(d2>>4)&0x0f;
    if(!side) {
      data[0] |= (o0<<=4);
      data[1] |= (o1<<=4);
      data[2] |= (o2<<=4);
      data[3] |= (o3<<=4);
      data[4] |= (o4<<=4);
    }
    else
    {
      data[0] |= o0;
      data[1] |= o1;
      data[2] |= o2;
      data[3] |= o3;
      data[4] |= o4;
    }
}

///////////////////////////////////////////////////////////////////////////
// showScore
// This function displays a number on the display, scrolling it to allow
// all digits to be seen
void showScore(unsigned long n)
{  
  unsigned long div = 10000000;
  byte dest = 0;
  byte side = 0;
  byte m[4][5] = {0};
  int j;
  
  while(div>0)
  {
    int z = n/div;
    n=n%div;
    div/=10;
    getDigit(z,side,m[dest]);    
    side=!side;
    if(!side) dest++;
  }
  for(int ofs=0; ofs<41; ++ofs)
  {
    Disp8x8.cls();
    for(j=0;j<5;++j)
    {
      byte b0=0,b1=0;
      if(ofs>7 && ofs<40) b0=m[(ofs-8)/8][j];
      if(ofs<32) b1=m[ofs/8][j];
      byte q=ofs%8;
      if(ofs&8)
        Disp8x8.green[j+1] = (b0<<q)|(b1>>(8-q));
      else
        Disp8x8.red[j+1] = (b0<<q)|(b1>>(8-q));
    }
    Disp8x8.delayWithRefresh(100);
  }  
}

///////////////////////////////////////////////////////////////////////////
// endGame
// Standard "Game Over" routine alternately flashes the last display frame 
// of the game and scrolls the content of the gameScore variable. The function
// does not return (user must reset the console)
void endGame() 
{
  byte red[8];
  byte green[8];
  int j,k;
  memcpy(red,Disp8x8.red,8);
  memcpy(green,Disp8x8.green,8);  
  for(;;)
  {
    for(k=0; k<5; ++k)
    {
      Disp8x8.delayWithRefresh(300);
      delay(300);
    }
    byte *buffer8 = Disp8x8.setBuffer8(NULL);
    showScore(gameScore);
    Disp8x8.setBuffer8(buffer8);
    memcpy(Disp8x8.red,red,8);
    memcpy(Disp8x8.green,green,8);
  }
}


///////////////////////////////////////////////////////////////////////////
// setTimeOut
// Schedule a "one off" timer event based on current time. Remember to clear
// timer period afterwards
void setTimeOut(byte whichTimer, unsigned long period)
{
  switch(whichTimer)
  {
    case 1: Timer1Period = 3600000; nextTimer1Event = millis() + period; break;
    case 2: Timer2Period = 3600000; nextTimer2Event = millis() + period; break;
    case 3: Timer3Period = 3600000; nextTimer3Event = millis() + period; break;
    case 4: Timer4Period = 3600000; nextTimer4Event = millis() + period; break;
    case 5: Timer5Period = 3600000; nextTimer5Event = millis() + period; break;
  }
}

///////////////////////////////////////////////////////////////////////////
// setup
// Arduino "setup" entry point called when the sketch starts
void setup() 
{    
  // configure IO pins
  pinMode(P_LED, OUTPUT);
  pinMode(P_SPK, OUTPUT);
  pinMode(P_BUTA, INPUT);
  pinMode(P_BUTB, INPUT);
  pinMode(P_BUTC, INPUT);
  pinMode(P_BUTD, INPUT);
  
  // set internal pull up resistors on button inputs
  digitalWrite(P_BUTA, HIGH);
  digitalWrite(P_BUTB, HIGH);
  digitalWrite(P_BUTC, HIGH);
  digitalWrite(P_BUTD, HIGH);
  digitalWrite(P_LED, HIGH);

//  IRLink.init();
//  Disp8x8.invert = IRLink.enabled;
  
  // initialise sound handling
  initSound();
  
  // the last selected game is started (or menu if not valid)
  byte which = EEPROM.read(EEPROM_GAMESELECTED);
  startGame(which);
  
}

///////////////////////////////////////////////////////////////////////////
// loop
// Arduino "loop" entry point called repeatedly
void loop() 
{
  byte event = 0;
  unsigned long milliseconds = millis();

  byte buttonA, buttonB, buttonC, buttonD;
  if(!Disp8x8.invert)
  {
    buttonA = !digitalRead(P_BUTA);
    buttonB = !digitalRead(P_BUTB);
    buttonC = !digitalRead(P_BUTC);
    buttonD = !digitalRead(P_BUTD);
  }
  else
  {
    buttonA = !digitalRead(P_BUTD);
    buttonB = !digitalRead(P_BUTC);
    buttonC = !digitalRead(P_BUTB);
    buttonD = !digitalRead(P_BUTA);
  }

  // We use the milliseconds until first button press after startup to give us some random
  // number entropy
  if(firstButtonPress)
  {
    if(buttonA || buttonB || buttonC || buttonD)
    { 
      randomSeed(milliseconds);
      firstButtonPress = 0;
    }
  }
  
  // Check for buttons A and C being held together for 1 second. This
  // combination returns to the menu
  if(buttonA && buttonC && thisGame > 0) 
  {
    if(!menuSelectKeyPressed) 
    {
      menuSelectKeyPressed = milliseconds + 1000;
    }
    else if(milliseconds > menuSelectKeyPressed)
    {
      setNextGame(0); // select the menu handler
    }
  }  
  else
  {
    menuSelectKeyPressed = 0;
  }
 
  // Poll and debounce button A
  if(buttonA) 
  {
    if(!timeButtonAPress)
    {      
      timeButtonAPress = milliseconds + DEBOUNCE_TIME;
      pGame->handleEvent(EV_PRESS_A);
    }
  }
  else if(timeButtonAPress && timeButtonAPress < milliseconds)
  {
      timeButtonAPress = 0;
      pGame->handleEvent(EV_RELEASE_A);
  }
  
  // Poll and debounce button B
  if(buttonB) 
  {
    if(!timeButtonBPress)
    {
      timeButtonBPress = milliseconds + DEBOUNCE_TIME;
      pGame->handleEvent(EV_PRESS_B);
    }
  }
  else if(timeButtonBPress && timeButtonBPress < milliseconds)
  {
      timeButtonBPress = 0;
      pGame->handleEvent(EV_RELEASE_B);
  }

  // Poll and debounce button C
  if(buttonC) 
  {
    if(!timeButtonCPress)
    {
      timeButtonCPress = milliseconds + DEBOUNCE_TIME;
      pGame->handleEvent(EV_PRESS_C);
    }
  }
  else if(timeButtonCPress && timeButtonCPress < milliseconds)
  {
      timeButtonCPress = 0;
      pGame->handleEvent(EV_RELEASE_C);
  }

  // Poll and debounce button D
  if(buttonD) 
  {
    if(!timeButtonDPress)
    {
      timeButtonDPress = milliseconds + DEBOUNCE_TIME;
      pGame->handleEvent(EV_PRESS_D);
    }
  }
  else if(timeButtonDPress && timeButtonDPress < milliseconds)
  {
      timeButtonDPress = 0;
      pGame->handleEvent(EV_RELEASE_D);
  }
  
  // Check whether timer events 1..5 are due
  if(Timer1Period && nextTimer1Event < milliseconds)
  {
      pGame->handleEvent(EV_TIMER_1);
      nextTimer1Event = milliseconds + Timer1Period;
  }
  if(Timer2Period && nextTimer2Event < milliseconds)
  {
      pGame->handleEvent(EV_TIMER_2);
      nextTimer2Event = milliseconds + Timer2Period;
  }
  if(Timer3Period && nextTimer3Event < milliseconds)
  {
      pGame->handleEvent(EV_TIMER_3);
      nextTimer3Event = milliseconds + Timer3Period;
  }
  if(Timer4Period && nextTimer4Event < milliseconds)
  {
      pGame->handleEvent(EV_TIMER_4);
      nextTimer4Event = milliseconds + Timer4Period;
  }
  if(Timer5Period && nextTimer5Event < milliseconds)
  {
      pGame->handleEvent(EV_TIMER_5);
      nextTimer5Event = milliseconds + Timer5Period;
  }

  // refresh the display 
  Disp8x8.refresh();
  
  // check if a change of game has been requested by the 
  // currently running game. This is used by the menu
  if(nextGame >= 0)
  {
    Disp8x8.setBuffer8(NULL);
    delete pGame;
    pGame = NULL;
    startGame(nextGame);
    nextGame = -1;
  }
  
  // Flash the heartbeat LED
  digitalWrite(P_LED,!heartbeat);
  heartbeat++;
}
