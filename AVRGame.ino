///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
////
//// A V R G A M E    C O R E
//// 
//// The main sketch file for the AVRGAME project
//// 
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <Tone.h>
#include <EEPROM.h>

#include "AVRGame.h"
#include "Menu.h"

// Include all game headers here
#include "InvadersGame.h"
#include "LanderGame.h"
#include "BreakoutGame.h"
#include "PongGame.h"
//#include "BlocksGame.h"
#include "FourInARowGame.h"

// Global variables
CGame *pGame = NULL;
Disp8x8Class Disp8x8;
Tone speaker;
byte soundOn = 0;
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
void getMenuIcon(int which, byte *dst)
{
  memset(dst,0,16);
  switch(which)
  {
    // BEGIN LIST OF GAMES
    case 0: CInvadersGame::getGameIcon(dst); break;
    case 1: CLanderGame::getGameIcon(dst); break;
    case 2: CBreakoutGame::getGameIcon(dst); break;
    case 3: CPongGame::getGameIcon(dst); break;
    case 4: CFourInARowGame::getGameIcon(dst); break;
    // END LIST OF GAMES
    
    default: CMenu::getGameIcon(dst); break; // Last item is the sound toggle
  }
}

///////////////////////////////////////////////////////////////////////////
// startGame
// Creates an instance of a game class and starts it running. The list of games
// defined here must match the list in the getMenuIcon function
void startGame(int which)
{
  switch(which)
  {
    // BEGIN LIST OF GAMES
    case 0: pGame = new CInvadersGame(); break;
    case 1: pGame = new CLanderGame(); break;
    case 2: pGame = new CBreakoutGame(); break;
    case 3: pGame = new CPongGame(); break;
    case 4: pGame = new CFourInARowGame(); break;
    // END LIST OF GAMES
    
    default: pGame =new CMenu(); which = NUM_GAMES; break;
  }    
  EEPROM.write(EEPROM_GAMESELECTED,which);
  
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
    for(j=0;j<50;++j)
      Disp8x8.refresh();
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
      for(j=0;j<100;++j)
        Disp8x8.refresh();
      delay(200);
    }
    showScore(gameScore);
    memcpy(Disp8x8.red,red,8);
    memcpy(Disp8x8.green,green,8);
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

  // We use the milliseconds until first button press after startup to give us some random
  // number entropy
  if(firstButtonPress)
  {
    if(digitalRead(P_BUTA) == LOW || digitalRead(P_BUTB) == LOW || digitalRead(P_BUTC) == LOW || digitalRead(P_BUTD))
    { 
      randomSeed(milliseconds);
      firstButtonPress = 0;
    }
  }
  
  // Check for buttons A and C being held together for 1 second. This
  // combination returns to the menu
  if(digitalRead(P_BUTA) == LOW && digitalRead(P_BUTC) == LOW) 
  {
    if(!menuSelectKeyPressed) 
    {
      menuSelectKeyPressed = milliseconds + 1000;
    }
    else if(milliseconds > menuSelectKeyPressed)
    {
      speaker.play(1000,20); 
      setNextGame(NUM_GAMES); // select the last game, which is always the menu handler
    }
  }  
  else
  {
    menuSelectKeyPressed = 0;
  }
 
  // Poll and debounce button A
  if(digitalRead(P_BUTA) == LOW) 
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
  if(digitalRead(P_BUTB) == LOW) 
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
  if(digitalRead(P_BUTC) == LOW) 
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
  if(digitalRead(P_BUTD) == LOW) 
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
    delete pGame;
    pGame = NULL;
    startGame(nextGame);
    nextGame = -1;
  }
  
  // Flash the heartbeat LED
  digitalWrite(P_LED,heartbeat&128);
  heartbeat++;
}
