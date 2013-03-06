////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   M E M O R Y    G A M E
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MEMORYGAME_TIMER2PERIOD 200
class CMemoryGame : public CGame 
{
  public:
    static void getGameIcon(byte *dst, byte count)
    {
      if(count & 1)
      {
        dst[ 0] = 0b00000000;          
        dst[ 1] = 0b00001110;          
        dst[ 2] = 0b00001010;          
        dst[ 3] = 0b00000010;          
        dst[ 4] = 0b00000100;          
        dst[ 5] = 0b00000000;          
        dst[ 6] = 0b01010100;          
        dst[ 7] = 0b00000000;          
      }
      else
      {
        dst[ 6] = 0b01010000;          
      }
      
      dst[ 8] = 0b00000000;          
      dst[ 9] = 0b00001110;          
      dst[10] = 0b00001010;          
      dst[11] = 0b00000010;          
      dst[12] = 0b00000100;          
      dst[13] = 0b00000000;          
      dst[14] = 0b00000100;          
      dst[15] = 0b00000000;          
      
    }    


    enum {
      SYMBOL_A,
      SYMBOL_B,
      SYMBOL_C,
      SYMBOL_D,
      SYMBOL_MAX
    };

    void playTone(byte symbol)
    {
      switch(symbol)
      {
      case SYMBOL_A:
        playSound(523, 200);
        break;
      case SYMBOL_B:
        playSound(659, 200);
        break;
      case SYMBOL_C:
        playSound(587, 200);
        break;
      case SYMBOL_D:
        playSound(698, 200);
        break;
      }      
    }
    
    void getSymbol(byte symbol, byte *dest)
    {      
      switch(symbol)
      {
        case SYMBOL_A:
          dest[0]|=0b01000000;
          dest[1]|=0b10100000;
          dest[2]|=0b11100000;
          dest[3]|=0b10100000;
          dest[4]|=0b10100000;
          break;
        case SYMBOL_B:
          dest[3]|=0b11000000;
          dest[4]|=0b10100000;
          dest[5]|=0b11000000;
          dest[6]|=0b10100000;
          dest[7]|=0b11000000;
          break;
        case SYMBOL_C:
          dest[0]|=0b00000010;
          dest[1]|=0b00000101;
          dest[2]|=0b00000100;
          dest[3]|=0b00000101;
          dest[4]|=0b00000010;
          break;
        case SYMBOL_D:
          dest[3]|=0b0000110;
          dest[4]|=0b0000101;
          dest[5]|=0b0000101;
          dest[6]|=0b0000101;
          dest[7]|=0b0000110;
          break;
      }
    }

#define MAX_SEQUENCE 50
    byte sequence[MAX_SEQUENCE];
    byte sequenceLen;
    byte sequencePos;
    byte showToggle;
    byte counter;
    byte started;
    byte lives;
    int playbackRate;
    void init()
    {
      lives = 3;
      sequencePos = 0;
      sequenceLen = 0;
      playbackRate = 500;
      started = 0;
      counter = 0;
      Timer2Period = MEMORYGAME_TIMER2PERIOD;
    }
    void addNewItem()
    {
      if(sequenceLen >= MAX_SEQUENCE-1)//seriously? 
        sequenceLen = 4;
      sequence[sequenceLen++] = random(SYMBOL_MAX);  
      sequencePos = 0;
      if(playbackRate>100)
        playbackRate -= 10;
    }
    void startPlayback() 
    {
      Timer1Period = playbackRate;            
      showToggle = 1;
      sequencePos = 0;
    }
    void keyPress(byte which)
    {
      int i;
      Timer2Period = 0; // cancel the prompt
      if(!started)
      {
        started = 1;        
        addNewItem();
        addNewItem();
        addNewItem();
        addNewItem();
        startPlayback();
      }
      else
      {
        playTone(which);
        Disp8x8.cls();
        getSymbol(which, Disp8x8.green);        
        getSymbol(which, Disp8x8.red);        
        if(which!= sequence[sequencePos])
        {
            Disp8x8.cls();
            Disp8x8.red[1] = 0b01000010;     
            Disp8x8.red[2] = 0b00100100;     
            Disp8x8.red[3] = 0b00011000;     
            Disp8x8.red[4] = 0b00011000;     
            Disp8x8.red[5] = 0b00100100;     
            Disp8x8.red[6] = 0b01000010;     
            for(i=10;i>0;--i)
            {
              playSound(200+50*i, 60);
              Disp8x8.delayWithRefresh(10);
            }
            if(!--lives)
              endGame();
            Disp8x8.delayWithRefresh(800);            
            startPlayback();
        }          
        else
        {
          if(++sequencePos>=sequenceLen)
          {
            gameScore+=100;
            Disp8x8.delayWithRefresh(500);
            addNewItem();
            Disp8x8.cls();
            Disp8x8.green[2] = 0b00000001;     
            Disp8x8.green[3] = 0b00000010;     
            Disp8x8.green[4] = 0b01000100;     
            Disp8x8.green[5] = 0b00101000;     
            Disp8x8.green[6] = 0b00010000;     
            playSound(1000, 50);
            Disp8x8.delayWithRefresh(50);
            playSound(1200, 50);
            Disp8x8.delayWithRefresh(50);
            playSound(1400, 50);
            Disp8x8.delayWithRefresh(800);
            startPlayback();
          }
          else
          {
            gameScore+=10;
          }
        }
      }
    }
  
    void handleEvent(char event)
    {
      int i;
      if(Timer1Period) // Timer 1 during pattern playback
      {
        if(event == EV_TIMER_1)
        {
          if(showToggle)
          {
            Disp8x8.cls();
            Timer1Period = playbackRate/2;
            showToggle = 0;
          }
          else if(sequencePos < sequenceLen)
          {
            getSymbol(sequence[sequencePos], Disp8x8.green);
            getSymbol(sequence[sequencePos], Disp8x8.red);
            playTone(sequence[sequencePos]);
            sequencePos++;
            Timer1Period = playbackRate;
            showToggle = 1;
          }
          else
          {
            Timer1Period = 0;
            Timer2Period = MEMORYGAME_TIMER2PERIOD;
            sequencePos = 0;
          }
        }
      }
      else
      switch(event)
      {
        case EV_PRESS_A:
           keyPress(SYMBOL_A);
           break;
        case EV_PRESS_B:
           keyPress(SYMBOL_B);
          break;
        case EV_PRESS_C:
           keyPress(SYMBOL_C);
          break;
        case EV_PRESS_D:
           keyPress(SYMBOL_D);
          break;
          
        case EV_TIMER_2: // TIMER 2 FLASHES '?' WAITING FOR INPUT
          Disp8x8.cls();
          if(++counter & 3)
          {
            Disp8x8.red[0] = 0b00011100;
            Disp8x8.red[1] = 0b00100010;
            Disp8x8.red[2] = 0b00000010;
            Disp8x8.red[3] = 0b00000100;
            Disp8x8.red[4] = 0b00001000;
            Disp8x8.red[6] = 0b00001000;
          }
      }      
    }
};
CGameFactoryImpl<CMemoryGame> MemoryGame;
