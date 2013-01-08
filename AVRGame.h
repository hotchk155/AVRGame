
/*
#define P_COL0  13
#define P_COL1  12
#define P_COL2  11
#define P_COL3  10
#define P_COL4  14
#define P_COL5  15
#define P_COL6  16
#define P_COL7  17
*/
#define P_COL7  14
#define P_COL6  15
#define P_COL5  16
#define P_COL4  17
#define P_COL3  13
#define P_COL2  12
#define P_COL1  11
#define P_COL0  10

#define P_CLK   3
#define P_OE   9
#define P_DAT   2
#define P_LED   5

#define P_SPK   8
#define P_BUTA  18
#define P_BUTB  19
#define P_BUTC  6
#define P_BUTD  7

#define DISP_OFF    0
#define DISP_RED    1
#define DISP_GREEN  2
#define DISP_YELLOW 3
#define DEBOUNCE_TIME   20
#define NUM_GAMES 4
#define MENU_SIZE (NUM_GAMES + 1)
#define MENU_SOUND NUM_GAMES

#define EEPROM_SOUNDON 100
#define EEPROM_GAMESELECTED 101

enum {
  EV_START    = 1,
  EV_PRESS_A,
  EV_PRESS_B,
  EV_PRESS_C,
  EV_PRESS_D,
  EV_RELEASE_A,
  EV_RELEASE_B,
  EV_RELEASE_C,
  EV_RELEASE_D,
  EV_TIMER_1,
  EV_TIMER_2,
  EV_TIMER_3,
  EV_TIMER_4,
  EV_TIMER_5
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   D I S P L A Y   H A N D L I N G
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Disp8x8Class {
public:  
  byte red[8];
  byte green[8];
  
  Disp8x8Class() {
    cls();
  }
  
  void cls() 
  {
    memset(red,0,8);
    memset(green,0,8);
  }
  
  void set(byte col, byte row, byte colour)
  {
    switch(colour)
    {
      case DISP_OFF:
        red[row%8]&=~(1<<(7-col%8));
        green[row%8]&=~(1<<(7-col%8));
        break;
      case DISP_RED:
        red[row%8]|=(1<<(7-col%8));
        green[row%8]&=~(1<<(7-col%8));
        break;
      case DISP_GREEN:
        red[row%8]&=~(1<<(7-col%8));
        green[row%8]|=(1<<(7-col%8));
        break;
      case DISP_YELLOW:
        red[row%8]|=(1<<(7-col%8));
        green[row%8]|=(1<<(7-col%8));
        break;
    }
  }
  
  byte get(byte col, byte row)
  {
    return (red[row%8]&(1<<(7-col%8))? DISP_RED : 0) |
          (green[row%8]&(1<<(7-col%8))? DISP_GREEN : 0);
  }
  
  void refresh()
  {
    digitalWrite(P_CLK, LOW);
    digitalWrite(P_DAT, HIGH);
    digitalWrite(P_CLK, HIGH);
    digitalWrite(P_DAT, LOW);
  
  //00  RD4
  //01  RD5
  //02  RD6
  //03  GR7
  //04  RD7
  //05  GR6
  //06  GR5
  //07  GR4
  //08  GR0
  //09  GR1
  //10  RD2
  //11  RD3
  //12  GR3
  //13  GR2
  //14  RD1
  //15  RD0
  
    for(int i=0; i<16; ++i)
    {
      digitalWrite(P_CLK, LOW);
      byte d;
      switch(i) 
      {
        case 0: d = red[4]; break;
        case 1: d = red[5]; break;
        case 2: d = red[6]; break;
        case 3: d = green[7]; break;
        case 4: d = red[7]; break;
        case 5: d = green[6]; break;
        case 6: d = green[5]; break;
        case 7: d = green[4]; break;
        case 8: d = green[0]; break;
        case 9: d = green[1]; break;
        case 10: d = red[2]; break;
        case 11: d = red[3]; break;
        case 12: d = green[3]; break;
        case 13: d = green[2]; break;
        case 14: d = red[1]; break;
        case 15: d = red[0]; break;
      }
      digitalWrite(P_COL0, LOW);
      digitalWrite(P_COL1, LOW);
      digitalWrite(P_COL2, LOW);
      digitalWrite(P_COL3, LOW);
      digitalWrite(P_COL4, LOW);
      digitalWrite(P_COL5, LOW);
      digitalWrite(P_COL6, LOW);
      digitalWrite(P_COL7, LOW);
  
  //    CLR_COLS;    
      
      digitalWrite(P_CLK, HIGH);
  
      digitalWrite(P_COL0, !!(d&1));
      digitalWrite(P_COL1, !!(d&2));
      digitalWrite(P_COL2, !!(d&4));
      digitalWrite(P_COL3, !!(d&8));
      digitalWrite(P_COL4, !!(d&16));
      digitalWrite(P_COL5, !!(d&32));
      digitalWrite(P_COL6, !!(d&64));
      digitalWrite(P_COL7, !!(d&128));
      
      delayMicroseconds(100);
    }
  }
};
extern Disp8x8Class Disp8x8;

class CGame 
{
  public:
    virtual void init() = 0;
    virtual void handleEvent(char event) = 0;
};


extern unsigned int Timer1Period;
extern unsigned int Timer2Period;
extern unsigned int Timer3Period;
extern unsigned int Timer4Period;
extern unsigned int Timer5Period;
inline void playSound(int pitch, int dur);
extern byte isSoundOn();
extern void setSoundOn(byte on);
extern void getMenuIcon(int which, byte *dst);
extern void setNextGame(int which);
extern void scrollNumber(unsigned long n, int ofs);


