////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   B I R D   G A M E 
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBirdGame : public CGame 
{
  byte walls[8];
  int distanceToNextWall;  
  byte nextGapMin;
  byte nextGapMax;  
  float height;
  float velocity;  
  public:
    static void getGameIcon(byte *dst, byte count)
    {
      dst[0] = 0b11111111;          
      dst[1] = 0b00000000;
      dst[2] = 0b00000000;
      dst[3] = 0b00000000;
      dst[4] = 0b00000000;
      dst[5] = 0b00000000;
      dst[6] = 0b00000000;
      dst[7] = 0b11111111;
    }
    
    void init()
    {
      distanceToNextWall = 10;
      nextGapMin = 2;
      nextGapMax = 4;
      memset(walls,0,8);
      Timer1Period = 100; 
      Timer2Period = 300; // forward
      height = 3;
      velocity = 0;

    }
    void handleEvent(char event)
    {
      int i;
      switch(event)
      {
        case EV_PRESS_A:
        case EV_PRESS_B:
        case EV_PRESS_C:
        case EV_PRESS_D:
          if(velocity > -0.5)
            velocity -= 0.1;
          break;

            
        case EV_TIMER_1: // gravity
          height += velocity;
          if(height < 0)
          {
            height = 0;
            if(velocity < 0)
              velocity = 0;
          }
          else if(height >= 7)
          {
            height = 7;
            velocity = 0;
          }
          else if(velocity < 0.5)
          {
            velocity += 0.05;
          }
          break;        
        case EV_TIMER_2: // forward movement
          {
            byte wall = 0;
            byte score = 0;
            if(--distanceToNextWall<0)
            {
              wall=1;
              distanceToNextWall = 8;
              nextGapMin = random(6);
              nextGapMax = nextGapMin + 2;
            }
        
            for(i=0;i<8;++i)
            {
              if(walls[i] & 0x80)
                score=1;                
              walls[i]<<=1;
              if(wall & (i < nextGapMin || i > nextGapMax))
                walls[i]|=0x01;
            }
            gameScore+=score;
          }
          break;          
      }
      memcpy(Disp8x8.red,walls,8);
      memset(Disp8x8.green,0,8);
      
      if(Disp8x8.get(1, height))
      {
        Disp8x8.set(1, height, DISP_YELLOW);
        endGame();
      }
      Disp8x8.set(1, height, DISP_GREEN);
    }      
};
CGameFactoryImpl<CBirdGame> BirdGame;
