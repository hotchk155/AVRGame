#include <Arduino.h>
#include <Tone.h>
#include <EEPROM.h>

#include "AVRGame.h"
#include "Menu.h"
#include "InvadersGame.h"
#include "LanderGame.h"
#include "BreakoutGame.h"
#include "PongGame.h"

Disp8x8Class Disp8x8;
Tone speaker;
byte soundOn = 0;
char nextGame = -1;

void getDigit(byte which, byte side, byte *data)
{
    byte d0=0,d1=0,d2=0;
    switch(which)
    {
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

void scrollNumber(unsigned long n, int ofs)
{
  unsigned long div = 10000000;
  byte dest = 0;
  byte side = 0;
  byte m[4][5] = {0};
  while(div>0)
  {
    int z = n/div;
    n=n%div;
    div/=10;
    getDigit(z,side,m[dest]);    
    side=!side;
    if(!side) dest++;
  }
ofs %= 48;
//  for(int i=0; i<48; ++i)
//  {
    Disp8x8.cls();
    for(int j=0;j<5;++j)
    {
      byte b0=0,b1=0;
      if(ofs>7 && ofs<40) b0=m[(ofs-8)/8][j];
      if(ofs<32) b1=m[ofs/8][j];
      byte q=ofs%8;
      if(ofs&1)
        Disp8x8.green[j+1] = (b0<<q)|(b1>>(8-q));
      else
        Disp8x8.red[j+1] = (b0<<q)|(b1>>(8-q));
    }
    //  for(int k=0;k<40;++k)
      //  Disp8x8.refresh();
  //}  
}

CGame *pGame = NULL;
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



void setNextGame(int which)
{
  nextGame = which;
}
void getMenuIcon(int which, byte *dst)
{
  memset(dst,0,16);
  switch(which)
  {
    case 0: CInvadersGame::getGameIcon(dst); break;
    case 1: CLanderGame::getGameIcon(dst); break;
    case 2: CBreakoutGame::getGameIcon(dst); break;
    case 3: CPongGame::getGameIcon(dst); break;
    default: CMenu::getGameIcon(dst); break; // Last item is the sound toggle
  }
}
void startGame(int which)
{
  switch(which)
  {
    case 0: pGame = new CInvadersGame(); break;
    case 1: pGame = new CLanderGame(); break;
    case 2: pGame = new CBreakoutGame(); break;
    case 3: pGame = new CPongGame(); break;
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
  pGame->init();
  pGame->handleEvent(EV_START);
}

void initGames()
{
   byte which = EEPROM.read(EEPROM_GAMESELECTED);
   startGame(which);
}


inline void playSound(int pitch, int dur)
{
  if(soundOn)
    speaker.play(pitch,dur);
}
void setSoundOn(byte on)
{
  soundOn = on;
  EEPROM.write(EEPROM_SOUNDON,soundOn);
}
byte isSoundOn()
{
  return soundOn;
}
void initSound()
{
   speaker.begin(P_SPK);
   soundOn = EEPROM.read(EEPROM_SOUNDON);
}









void setup() 
{  
  pinMode(P_COL0, OUTPUT);
  pinMode(P_COL1, OUTPUT);
  pinMode(P_COL2, OUTPUT);
  pinMode(P_COL3, OUTPUT);
  pinMode(P_COL4, OUTPUT);
  pinMode(P_COL5, OUTPUT);
  pinMode(P_COL6, OUTPUT);
  pinMode(P_COL7, OUTPUT);
  pinMode(P_CLK, OUTPUT);
  pinMode(P_OE, OUTPUT);
  pinMode(P_DAT, OUTPUT);
  pinMode(P_LED, OUTPUT);
  pinMode(P_SPK, OUTPUT);
  pinMode(P_BUTA, INPUT);
  pinMode(P_BUTB, INPUT);
  pinMode(P_BUTC, INPUT);
  pinMode(P_BUTD, INPUT);
  
  digitalWrite(P_OE, LOW);
  digitalWrite(P_BUTA, HIGH);
  digitalWrite(P_BUTB, HIGH);
  digitalWrite(P_BUTC, HIGH);
  digitalWrite(P_BUTD, HIGH);
  
  initSound();
  initGames();
}



byte q = 0;
// the loop routine runs over and over again forever:
unsigned long modePress = 0;
void loop() {
  byte event = 0;
  unsigned long milliseconds = millis();
    
  if(digitalRead(P_BUTA) == LOW && digitalRead(P_BUTC) == LOW) 
  {
    if(!modePress) 
    {
      modePress = milliseconds + 2000;
    }
    else if(milliseconds > modePress)
    {
      speaker.play(1000,20); 
      setNextGame(NUM_GAMES);
    }
  }  
  else
  {
    modePress = 0;
  }
 
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
 
  Disp8x8.refresh();
  
  if(nextGame >= 0)
  {
    delete pGame;
    pGame = NULL;
    startGame(nextGame);
    nextGame = -1;
  }
  
  digitalWrite(P_LED,q&128);
  q++;
}
