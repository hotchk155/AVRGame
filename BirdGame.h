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
  byte closeTop;
  byte closeBottom;
  byte gapBetweenWalls;
  byte gapSize;  
  byte gameLevel;
  byte wingToggle;
  
  public:
    static void getGameIcon(byte *dst, byte count)
    {
      switch(count&0x3)
      {
        case 0:
          dst[0] = 0b00000000;          
          dst[1] = 0b00000000;
          dst[2] = 0b00010011;
          dst[3] = 0b00000010;
          dst[4] = 0b00000000;
          dst[5] = 0b00000000;
          dst[6] = 0b00000000;
          dst[7] = 0b00000000;
          
          dst[8] =  0b00111000;          
          dst[9] =  0b01111100;
          dst[10] = 0b01101111;
          dst[11] = 0b11111110;
          dst[12] = 0b01111000;
          dst[13] = 0b01110000;
          dst[14] = 0b01100000;
          dst[15] = 0b00000000;
          break;
      
        case 1:
        case 3:
          dst[0] =  0b00000000;          
          dst[1] =  0b00000000;
          dst[2] =  0b00000000;
          dst[3] =  0b00010011;
          dst[4] =  0b00000010;
          dst[5] =  0b00000000;
          dst[6] =  0b00000000;
          dst[7] =  0b00000000;
          
          dst[8] =  0b00000000;          
          dst[9] =  0b00111000;
          dst[10] = 0b01111100;
          dst[11] = 0b11101111;
          dst[12] = 0b11111110;
          dst[13] = 0b01111000;
          dst[14] = 0b00000000;
          dst[15] = 0b00000000;
          break;

        case 2:
          dst[0] = 0b00000000;          
          dst[1] = 0b00000000;
          dst[2] = 0b00000000;
          dst[3] = 0b00000000;
          dst[4] = 0b00010011;
          dst[5] = 0b00000010;
          dst[6] = 0b00000000;
          dst[7] = 0b00000000;
          
          dst[8] =  0b00000000;          
          dst[9] =  0b01100000;
          dst[10] = 0b11111000;
          dst[11] = 0b01111100;
          dst[12] = 0b01101111;
          dst[13] = 0b11111110;
          dst[14] = 0b00111000;
          dst[15] = 0b00000000;
          break;
      }
    }

    void nextWall(int space)
    {
      distanceToNextWall = space;
      nextGapMin = random(7-gapSize);
      nextGapMax = nextGapMin + gapSize - 1;
    }
    
    void advanceLevel()
    {
      gameLevel++;
      if(!((20+gameLevel)%25) && gapSize>2)
        --gapSize;
      if(!(gameLevel%9) && gapBetweenWalls>5)
        --gapBetweenWalls;
      if(gameLevel > 20)
        closeBottom = 1;
      if(gameLevel > 30)
        closeTop = 1; 
      if(Timer2Period > 50)
       Timer2Period --;
      if(Timer1Period > 50)
       Timer1Period --;
    }
    
    void init()
    {
      memset(walls,0,8);
      gameLevel = 0;
      gapBetweenWalls = 8;
      gapSize = 3;
      Timer1Period = 100; // gravity
      Timer2Period = 300; // forward
      closeTop = 0;
      closeBottom = 0;      
      advanceLevel();
      height = 3;
      velocity = 0;
      wingToggle = 1;
      nextWall(10);
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
          wingToggle = !wingToggle;
          playSound(200+wingToggle * 200, 20);
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
              nextWall(gapBetweenWalls);
            }
        
            for(i=0;i<8;++i)
            {
              if(walls[i] & 0x80)
                score=1;                
              walls[i]<<=1;
              if(wall & (i < nextGapMin || i > nextGapMax))
                walls[i]|=0x01;
            }
            if(closeTop)
              walls[0]|=0x01;
            if(closeBottom)
              walls[7]|=0x01;
            if(score)
            {
              gameScore++;
              if(!(gameScore % 10))
              {
                playSound(400, 200);            
                delay(50);
                playSound(800, 200);            
                delay(50);
                playSound(400, 200);            
                delay(50);
                playSound(800, 200);            
                delay(50);
                advanceLevel();
              }
              else
              {
                playSound(800, 200);            
                delay(10);
              }
            }
            else
            {
              playSound(800-50*height, 10);
            }
            
          }
          break;          
      }
      memcpy(Disp8x8.red,walls,8);
      memset(Disp8x8.green,0,8);
      
      if(Disp8x8.get(1, height))
      {
        Disp8x8.set(1, height, DISP_YELLOW);
        for(i=600; i>100; i-=50)
        {
          playSound(i, 50);
          Disp8x8.delayWithRefresh(10);
          playSound(i+100, 50);
          Disp8x8.delayWithRefresh(10);
        }
        endGame();
      }
      Disp8x8.set(1, height, DISP_GREEN);
    }      
};
CGameFactoryImpl<CBirdGame> BirdGame;
