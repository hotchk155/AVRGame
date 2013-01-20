///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
////
//// A V R G A M E    C O R E
//// 
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#define NUM_GAMES 5 // change this number to have move games in the menu

// define the pins for the matrix columns
#define P_COL7  14
#define P_COL6  15
#define P_COL5  16
#define P_COL4  17
#define P_COL3  13
#define P_COL2  12
#define P_COL1  11
#define P_COL0  10

// define the pins for the shift registers
#define P_CLK   3
#define P_OE   9
#define P_DAT   2
#define P_LED   5

// define the pins for speaker and buttons
#define P_SPK   8
#define P_BUTA  18
#define P_BUTB  19
#define P_BUTC  6
#define P_BUTD  7

// LED colours
#define DISP_OFF    0
#define DISP_RED    1
#define DISP_GREEN  2
#define DISP_YELLOW 3

// button debounce time (ms)
#define DEBOUNCE_TIME   20

// menu size
#define MENU_SIZE (NUM_GAMES + 1)
#define MENU_SOUND NUM_GAMES

// eeprom location addresses
#define EEPROM_SOUNDON 100
#define EEPROM_GAMESELECTED 101

// events passed to game handers
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
    digitalWrite(P_OE, LOW);    
    cls();
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // cls
  // Clear the display
  void cls() 
  {
    memset(red,0,8);
    memset(green,0,8);
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // set
  // Set a pixel colour
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
  void set(byte pos, byte colour)
  {
    set(pos%8,pos/8,colour);
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // get
  // Fetch a pixel colour
  byte get(byte col, byte row)
  {
    return (red[row%8]&(1<<(7-col%8))? DISP_RED : 0) |
          (green[row%8]&(1<<(7-col%8))? DISP_GREEN : 0);
  }
  byte get(byte pos)
  {
    byte row = (pos/8)%8;
    return (red[row]&(1<<(7-pos%8))? DISP_RED : 0) |
          (green[row]&(1<<(7-pos%8))? DISP_GREEN : 0);
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // refresh
  // Refresh the display matrix. This function must be called repeatedly for the display to work
  void refresh()
  {
    digitalWrite(P_CLK, LOW);
    digitalWrite(P_DAT, HIGH);
    digitalWrite(P_CLK, HIGH);
    digitalWrite(P_DAT, LOW);
  
    for(int i=0; i<16; ++i)
    {
      digitalWrite(P_CLK, LOW);
      byte d;
      
      // Note that this switch statement maps the shift register bits to 
      // the actual row position and colour. It depends on the specific 
      // wiring of the circuit board 
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

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // delayWithRefresh
  // Delay function which keeps display updated
  void delayWithRefresh(int cycles)
  {
    while(cycles-- >0)
      refresh();
  }

};
extern Disp8x8Class Disp8x8;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   G A M E   C L A S S   P R O T O T Y P E
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CGame 
{
  public:
    virtual void init() = 0;
    virtual void handleEvent(char event) = 0;
};


// External variables
extern unsigned int Timer1Period;
extern unsigned int Timer2Period;
extern unsigned int Timer3Period;
extern unsigned int Timer4Period;
extern unsigned int Timer5Period;
extern unsigned long gameScore;
inline void playSound(int pitch, int dur);
extern byte isSoundOn();
extern void setSoundOn(byte on);
extern void getMenuIcon(int which, byte *dst);
extern void setNextGame(int which);
extern void showScore(unsigned long n);
extern void endGame();

