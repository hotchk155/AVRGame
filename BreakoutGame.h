////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   B R E A K O U T     G A M E 
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_BALLS 2
class CBreakoutGame : public CGame 
{
  public:
    static void getGameIcon(byte *dst)
    {
      dst[ 0] = 0b00000000;          
      dst[ 1] = 0b01111110;          
      dst[ 2] = 0b00000000;          
      dst[ 3] = 0b00000000;          
      dst[ 4] = 0b00000100;          
      dst[ 5] = 0b00000000;          
      dst[ 6] = 0b00001110;          
      dst[ 7] = 0b00000000;          
      
      dst[ 8] = 0b00000000;          
      dst[ 9] = 0b01111110;          
      dst[10] = 0b00010000;          
      dst[11] = 0b00101000;          
      dst[12] = 0b01000100;          
      dst[13] = 0b00000000;          
      dst[14] = 0b00000000;          
      dst[15] = 0b00000000;          
      
    }    
    
    char playerX;
    char ballX[MAX_BALLS];
    char ballY[MAX_BALLS];
    char ballDX[MAX_BALLS];
    char ballDY[MAX_BALLS];
    char numBalls;
    byte blocks[8];
    void init()
    {
      startLevel();
      Timer1Period = 200;
    }
    void startLevel()
    {
      playerX = 3;              
      memset(blocks,0,8);
      blocks[0]=0xff;
      blocks[1]=0xff;
      blocks[2]=0xff;
      numBalls = 0;
      newBall();
    }
    void newBall()
    {
      if(numBalls<MAX_BALLS)
      {
        ballX[numBalls] = 3;
        ballY[numBalls] = 6;
        ballDX[numBalls] = 0;
        ballDY[numBalls] = -1;
        numBalls++;        
      }
    }
    void handleEvent(char event)
    {
      int i;
      switch(event)
      {
        case EV_PRESS_B:
          if(playerX>0) 
            playerX--;
          break;
        case EV_PRESS_D:
          if(playerX<6) 
            playerX++;
          break;
    // 012
    // 7 3 
    // 654 
        case EV_TIMER_1:
          for(i=0;i<numBalls;++i)
          {
            for(;;)
            {
              int nx = ballX[i] + ballDX[i];
              int ny = ballY[i] + ballDY[i];
              if(nx<0 || nx>7) 
              {
                ballDX[i]=-ballDX[i];
                nx = ballX[i];
              }
              if(ny<0) 
              {
                ballDY[i]=1;
                ny = 0;
              }
              else if(ny == 7)
              {   
                ballDY[i]=-1;
                if(ballX[i] == playerX) // flat hit left side
                {
                  if(playerX == 0)
                    ballDX[i] = (ballDX[i]<0)? 0: 1;
                  else
                    ballDX[i] = (ballDX[i]>0)? 0: -1;
                  continue;
                }
                else if(ballX[i] == playerX+1) // flat hit right side
                {
                  if(playerX == 6)
                    ballDX[i] = (ballDX[i]>0)? 0: -1;
                  else
                    ballDX[i] = (ballDX[i]<0)? 0: 1;
                  continue;
                }
                else if(nx == playerX) // hit left end
                {
                  ballDX[i] =-1;
                  continue;
                }
                else if(nx == playerX+1) // hit right side
                {
                  ballDX[i] =1;
                  continue;
                }
                else
                {
                  //death
                  ballDX[i] =0;
                  ballDY[i] =0;
                }
              }                
              ballX[i]=nx;
              ballY[i]=ny;
              if(blocks[ny]&(1<<(7-nx)))
              {
                blocks[ny]&=~(1<<(7-nx));
                ballDY[i]*=-1;
              }
              break;
            }
            Timer1Period = (!ballDX[i] || !ballDY[i])? 100 : 150;
          }
          break;        
      }
      
      memcpy(Disp8x8.red,blocks,8);
      memset(Disp8x8.green,0,8);
      Disp8x8.set(playerX, 7, DISP_GREEN);
      Disp8x8.set(playerX+1, 7, DISP_GREEN);
      Disp8x8.set(ballX[0], ballY[0], DISP_GREEN);
      for(i=0;i<numBalls;++i)
        Disp8x8.set(ballX[i], ballY[i], DISP_YELLOW);
    }
};

