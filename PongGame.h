////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   P O N G     G A M E 
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CPongGame : public CGame 
{
  byte clrBuf[64];
  int count;
  public:
    static void getGameIcon(byte *dst, byte count)
    {
      dst[ 0] = 0b00000000;          
      dst[ 1] = 0b01000000;          
      dst[ 2] = 0b01000000;          
      dst[ 3] = 0b01000000;          
      dst[ 4] = 0b00000010;          
      dst[ 5] = 0b00000010;          
      dst[ 6] = 0b00001010;          
      dst[ 7] = 0b00000000;          
      
      dst[ 8] = 0b00000000;          
      dst[ 9] = 0b00000000;          
      dst[10] = 0b00000000;          
      dst[11] = 0b00010000;          
      dst[12] = 0b00001000;          
      dst[13] = 0b00000100;          
      dst[14] = 0b00001000;          
      dst[15] = 0b00000000;          
      
    }    
    void init()
    {
      count=0;
      Disp8x8.setBuffer8(clrBuf);
      for(byte b=0;b<64;++b)
      {
        byte r=(b&0x7)<<1;
        byte g=((b>>3)&0x7)<<1;
        clrBuf[b] = COL8(r,g);
      }
          Timer1Period = 100;
          Timer2Period = 50;
          IRLink.mode = IRLINK_SEND;
    }
    void handleEvent(char event)
    {
      switch(event)
      {
        /*
        case EV_PRESS_D:
          endGame();
          break;
        case EV_PRESS_B:
          Disp8x8.cls();
          break;
        case EV_PRESS_C:
          break;*/
        case EV_TIMER_1:
          for(int i=0;i<64;++i){
            byte a = clrBuf[i]>>4;
            byte b = clrBuf[i]&0xf;
            if(a) --a;
            if(b) --b;
            clrBuf[i] = b+8*a;
          }
          break;
        case EV_TIMER_2:
          clrBuf[random(64)] = random(256);
          break;
      
      case EV_PRESS_A: IRLink.sendEvent('A'); break;
      case EV_PRESS_B: IRLink.sendEvent('B'); break;
      case EV_PRESS_C: IRLink.sendEvent('C'); break;
      case EV_PRESS_D: IRLink.sendEvent('D'); break;
      case EV_RELEASE_A: IRLink.sendEvent('a'); break;
      case EV_RELEASE_B: IRLink.sendEvent('b'); break;
      case EV_RELEASE_C: IRLink.sendEvent('c'); break;
      case EV_RELEASE_D: IRLink.sendEvent('d'); break;
    }
  }
};
CGameFactoryImpl<CPongGame> PongGame;

