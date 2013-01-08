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
  public:  
    static void getGameIcon(byte *dst)
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
      sel = 0;
      offset = 0;
      showIcon(0);
    }
   
    void showIcon(int ofs)
    {
      byte left[16];
      byte right[16];
      getMenuIcon(sel,left);
      if(sel < MENU_SIZE-1)
        getMenuIcon(sel+1, right);
      else
        getMenuIcon(0, right);      
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
        case EV_PRESS_C: // toggle
          if(sel == MENU_SOUND)
          {
            setSoundOn(!isSoundOn());
          }
          else
          {
            setNextGame(sel);
          }
          break;
        case EV_PRESS_B: // previous item
          if(!Timer1Period)
          {
            if(--sel < 0) sel = MENU_SIZE - 1;
            offset = 7;
            Timer1Period = 50;
          }
          break;
        case EV_PRESS_D: // next item
          if(!Timer2Period)
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
            if(++sel > MENU_SIZE - 1) sel = 0;
            offset =0;
            Timer2Period = 0;
          }
          break;
      }
      showIcon(offset);
    }
};

