////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   M E N U
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CMenu : public CGame 
{
    char sel;
    char offset;
    char count;
    char shift;
  public:  
    static void getGameIcon(byte *dst, byte count)
    {
      if(!isSoundOn())
      {
        dst[ 0] = 0b00000000;
        dst[ 1] = 0b00000010;
        dst[ 2] = 0b00000100;
        dst[ 3] = 0b00001000;
        dst[ 4] = 0b00010000;
        dst[ 5] = 0b00100000;
        dst[ 6] = 0b01000000;
        dst[ 7] = 0b00000000;
      }
      dst[ 8] = 0b00000000;
      dst[ 9] = 0b00000010;
      dst[10] = 0b01110110;
      dst[11] = 0b01011010;
      dst[12] = 0b01011010;
      dst[13] = 0b01110110;
      dst[14] = 0b00000010;
      dst[15] = 0b00000000;
    }
    void init()
    {
      shift = 0;
      sel = EEPROM.read(EEPROM_GAMESELECTED);
      if(sel <= 0 || sel >= numGameFactories)
        sel = 1;
      offset = 0;
      showIcon(0, 0);
      Timer3Period = 100;
    }
   
    void showIcon(int ofs, byte count)
    {
      byte left[16];
      byte right[16];
      getMenuIcon(sel,left,count);
      if(sel < numGameFactories-1)
        getMenuIcon(sel+1, right, count);
      else
        getMenuIcon(0, right, count);      
      for(int i=0;i<8;++i) 
      {
        Disp8x8.red[i] = (left[i]<<ofs) | right[i]>>(8-ofs);
        Disp8x8.green[i] = (left[i+8]<<ofs) | right[i+8]>>(8-ofs);
      }
    }
    
    void handleEvent(char event)
    {
      switch(event)
      {
        case EV_PRESS_A:
          shift=1;
          break;
        case EV_RELEASE_A:
          shift=0;
          break;
        
        case EV_PRESS_C: 
          if(shift)
          {
            // version
            Disp8x8.cls();
            getDigit(VERSION_HI, 0, Disp8x8.green);
            getDigit(VERSION_LO, 1, Disp8x8.green);
            Disp8x8.red[5] = 0b00001000;
            Disp8x8.delayWithRefresh(1000);
          }
          else if(sel == 0)
          {
            // toggle
            setSoundOn(!isSoundOn());
          }
          else
          {
            // start game
            setNextGame(sel);
          }
          break;
        case EV_PRESS_B: // previous item
          if(!Timer1Period && !Timer2Period)
          {
            if(--sel < 0) sel = numGameFactories - 1;
            offset = 7;
            Timer1Period = 50;
          }
          break;
        case EV_PRESS_D: // next item
          if(!Timer1Period && !Timer2Period)
          {
            offset = 0;
            Timer2Period = 50;
          }
          break;
        case EV_TIMER_1:
          if(--offset <= 0)
          {
            Timer1Period = 0;
          }
          break;
        case EV_TIMER_2:
          if(++offset > 7)
          {
            if(++sel > numGameFactories-1) 
              sel = 0;
            offset =0;
            Timer2Period = 0;
          }
          break;
        case EV_TIMER_3:
          count++;
          break;
      }
      showIcon(offset, count);
    }
};
CGameFactoryImpl<CMenu> Menu;
