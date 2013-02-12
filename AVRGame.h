///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
////
//// A V R G A M E    C O R E
//// 
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

// Maximim number of games to be allowed in the menu
#define MAX_GAMES 8

// define the pins for the matrix columns
#define P_COL0  10 //b2
#define P_COL1  11 //b3
#define P_COL2  12 //b4
#define P_COL3  13 //b5
#define P_COL4  17 //c3
#define P_COL5  16 //c2
#define P_COL6  15 //c1
#define P_COL7  14 //c0

// define the pins for the shift registers
#define P_CLK   3 //d3
#define P_OE   9
#define P_DAT   2 //d2
#define P_LED   5 

// define the pins for speaker and buttons
#define P_SPK   8
#define P_BUTA  18
#define P_BUTB  19
#define P_BUTC  6
#define P_BUTD  7

#define SET_COL0(v) if(v) PORTB|=(1<<2); else PORTB&=~(1<<2)
#define SET_COL1(v) if(v) PORTB|=(1<<3); else PORTB&=~(1<<3)
#define SET_COL2(v) if(v) PORTB|=(1<<4); else PORTB&=~(1<<4)
#define SET_COL3(v) if(v) PORTB|=(1<<5); else PORTB&=~(1<<5)
#define SET_COL4(v) if(v) PORTC|=(1<<3); else PORTC&=~(1<<3)
#define SET_COL5(v) if(v) PORTC|=(1<<2); else PORTC&=~(1<<2)
#define SET_COL6(v) if(v) PORTC|=(1<<1); else PORTC&=~(1<<1)
#define SET_COL7(v) if(v) PORTC|=(1<<0); else PORTC&=~(1<<0)
#define SET_CLK(v) if(v) PORTD|=(1<<3); else PORTD&=~(1<<3)


// LED colours
#define DISP_OFF    0
#define DISP_RED    1
#define DISP_GREEN  2
#define DISP_YELLOW 3

// button debounce time (ms)
#define DEBOUNCE_TIME   20

// eeprom location addresses
#define EEPROM_SOUNDON 100
#define EEPROM_GAMESELECTED 101

// events passed to game handers
enum {
  EV_START    = 1,
  EV_DISPLAY,
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
  byte redDuty;  
  byte greenDuty;  
  byte *buffer8;
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
    buffer8 = NULL;
    redDuty = 255;
    greenDuty = 255;
    cls();
  }
  
  
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // cls
  // Clear the display
  void cls() 
  {
    memset(red,0,8);
    memset(green,0,8);
    if(buffer8)
      memset(buffer8, 0, 64);
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
  // setBuffer8
  // Associate a 64 byte colour buffer with the display for 8-bit colour mode
  void setBuffer8(byte *buf)
  {
    buffer8 = buf;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // set8
  // Set 8-bit pixel colour
  void set8(byte col, byte row, byte colour)
  {
    if(buffer8)
      buffer8[(col&0x7f)+((row&0x7f)<<3)] = colour;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // set8
  // Get 8-bit pixel colour
  byte get8(byte col, byte row, byte colour)
  {
    if(buffer8)
      return buffer8[(col&0x7f)|((row&0x7f)<<3)];
    return 0;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // refresh
  // Refresh the display matrix. This function must be called repeatedly for the display to work
  void refresh()
  {
    digitalWrite(P_OE,  LOW);    
    digitalWrite(P_CLK, LOW);
    digitalWrite(P_DAT, HIGH);
    digitalWrite(P_CLK, HIGH);
    digitalWrite(P_DAT, LOW);
  
    for(int i=0; i<16; ++i)
    {      
      // Note that this switch statement maps the shift register bits to 
      // the actual row position and colour. It depends on the specific 
      // wiring of the circuit board 
      byte d0, d1, d2, d3, d4, d5, d6, d7;
      if(buffer8)
      {
        byte leftShift, *p;
        switch(i) 
        {//4 for red
          case 0: p = &buffer8[8*4]; leftShift=4; break;
          case 1: p = &buffer8[8*5]; leftShift=4; break;
          case 2: p = &buffer8[8*6]; leftShift=4; break;
          case 3: p = &buffer8[8*7]; leftShift=0; break;
          case 4: p = &buffer8[8*7]; leftShift=4; break;
          case 5: p = &buffer8[8*6]; leftShift=0; break;
          case 6: p = &buffer8[8*5]; leftShift=0; break;
          case 7: p = &buffer8[8*4]; leftShift=0; break;
          case 8: p = &buffer8[8*0]; leftShift=0; break;
          case 9: p = &buffer8[8*1]; leftShift=0; break;
          case 10: p = &buffer8[8*2]; leftShift=4; break;
          case 11: p = &buffer8[8*3]; leftShift=4; break;
          case 12: p = &buffer8[8*3]; leftShift=0; break;
          case 13: p = &buffer8[8*2]; leftShift=0; break;
          case 14: p = &buffer8[8*1]; leftShift=4; break;
          case 15: p = &buffer8[8*0]; leftShift=4; break;
        }
        
        d0=(p[0]<<leftShift)&0xf0;
        d1=(p[1]<<leftShift)&0xf0;
        d2=(p[2]<<leftShift)&0xf0;
        d3=(p[3]<<leftShift)&0xf0;
        d4=(p[4]<<leftShift)&0xf0;
        d5=(p[5]<<leftShift)&0xf0;
        d6=(p[6]<<leftShift)&0xf0;
        d7=(p[7]<<leftShift)&0xf0;
      }
      else
      {
        byte d,duty;
        switch(i) 
        {
          case 0: d = red[4]; duty=redDuty; break;
          case 1: d = red[5]; duty=redDuty; break;
          case 2: d = red[6]; duty=redDuty; break;
          case 3: d = green[7]; duty=greenDuty; break;
          case 4: d = red[7]; duty=redDuty; break;
          case 5: d = green[6]; duty=greenDuty; break;
          case 6: d = green[5]; duty=greenDuty; break;
          case 7: d = green[4]; duty=greenDuty; break;
          case 8: d = green[0]; duty=greenDuty; break;
          case 9: d = green[1]; duty=greenDuty; break;
          case 10: d = red[2]; duty=redDuty; break;
          case 11: d = red[3]; duty=redDuty; break;
          case 12: d = green[3]; duty=greenDuty; break;
          case 13: d = green[2]; duty=greenDuty; break;
          case 14: d = red[1]; duty=redDuty; break;
          case 15: d = red[0]; duty=redDuty; break;
        }
        d0 = (d&0x01)? duty:0;
        d1 = (d&0x02)? duty:0;
        d2 = (d&0x04)? duty:0;
        d3 = (d&0x08)? duty:0;
        d4 = (d&0x10)? duty:0;
        d5 = (d&0x20)? duty:0;
        d6 = (d&0x40)? duty:0;
        d7 = (d&0x80)? duty:0;
      }
      
      SET_CLK(0);
      
      SET_COL0(0); SET_COL1(0); SET_COL2(0); SET_COL3(0);
      SET_COL4(0); SET_COL5(0); SET_COL6(0); SET_COL7(0);
  
      SET_CLK(1);

      SET_COL0(d0); SET_COL1(d1); SET_COL2(d2); SET_COL3(d3);
      SET_COL4(d4); SET_COL5(d5); SET_COL6(d6); SET_COL7(d7);
      
      byte z=0;
      while(++z) {
//        delayMicroseconds(1);
        if(z==d0) SET_COL0(0);
        if(z==d1) SET_COL1(0);
        if(z==d2) SET_COL2(0);
        if(z==d3) SET_COL3(0);
        if(z==d4) SET_COL4(0);
        if(z==d5) SET_COL5(0);
        if(z==d6) SET_COL6(0);
        if(z==d7) SET_COL7(0);
      }
    }   
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // delayWithRefresh
  // Delay function which keeps display updated
  void delayWithRefresh(int cycles)
  {
    unsigned long t = millis() + cycles;
    while(millis() < t)
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   G A M E   F A C T O R Y
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CGameFactory {
public:  
     virtual void getGameIcon(byte *dst, byte count) = 0;
     virtual CGame *createInstance() = 0;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   G A M E   F A C T O R Y   I M P L 
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern byte numGameFactories;
extern CGameFactory *gameFactory[];
template<class T> class CGameFactoryImpl : public CGameFactory
{
public:
   CGameFactoryImpl() {
     if(numGameFactories < MAX_GAMES)
       gameFactory[numGameFactories++] = this;
   }
   void getGameIcon(byte *dst, byte count) {
     T::getGameIcon(dst, count);
   }
   CGame *createInstance() {
     return new T;
   }   
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
extern void getMenuIcon(int which, byte *dst, byte count);
extern void setNextGame(int which);
extern void showScore(unsigned long n);
extern void endGame();
extern void setTimeOut(byte whichTimer, unsigned long period);

