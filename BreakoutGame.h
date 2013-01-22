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

    char countdown;    
    char lives;
    char playerX;
    char ballX;
    char ballY;
    char ballDX;
    char ballDY;
    byte blocks[8];
    int ballSpeed;
    int level;
    void init()
    {
      lives = 3;
      level = 0;
      Timer1Period = 200;
      newLevel();
    }
    void newLevel()
    {
      level++;
      memset(blocks,0,8);
      blocks[0]=0xff;
      blocks[1]=0xff;
      blocks[2]=0xff;
      if(level>2)
        blocks[3]=0xff;
      if(level>5)
        blocks[4]=0xff;
      newBall();
    }
    void newBall()
    {
      playerX = 3;              
      ballX = 3;
      ballY = 6;
      ballDX = 0;
      ballDY = 0;
      ballSpeed = 100 - 5*level;
      if(ballSpeed < 30)
        ballSpeed = 30;
      countdown = 3;
      Timer2Period = 400;
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
          Disp8x8.set(ballX, ballY, DISP_OFF);        
          for(;;)
          {
            int nx = ballX + ballDX;
            int ny = ballY + ballDY;
            if(nx<0 || nx>7) 
            {
              ballDX=-ballDX;
              nx = ballX;
            }
            if(ny<0) 
            {
              ballDY=1;
              ny = 0;
            }
            else if(ny == 7)
            {   
              ballDY=-1;
              if(ballX == playerX) // flat hit left side
              {
                
                playSound(400,20);
                if(playerX == 0)
                  ballDX = (ballDX<0)? 0: 1;
                else
                  ballDX = (ballDX>0)? 0: -1;
                continue;
              }
              else if(ballX == playerX+1) // flat hit right side
              {
                playSound(400,20);
                if(playerX == 6)
                  ballDX = (ballDX>0)? 0: -1;
                else
                  ballDX = (ballDX<0)? 0: 1;
                continue;
              }
              else if(nx == playerX) // hit left end
              {
                playSound(800,10);
                ballDX =-1;
                continue;
              }
              else if(nx == playerX+1) // hit right side
              {
                playSound(800,10);
                ballDX =1;
                continue;
              }
              else
              {
                // died
                Disp8x8.set(nx, ny, DISP_YELLOW);
                for(i=10;i>0;--i)
                {
                  playSound(200+50*i, 60);
                  Disp8x8.delayWithRefresh(10);
                }
                if(!lives--)
                {
                  endGame();
                }
                else
                {
                  Disp8x8.delayWithRefresh(500);
                  newBall();
                  break;
                }                            
              }
            }                
            
            ballX=nx;
            ballY=ny;
            Disp8x8.set(nx, ny, DISP_YELLOW);
            if(blocks[ny]&(1<<(7-nx)))
            {
              gameScore += 10 * level;
              playSound(600,50);
              blocks[ny]&=~(1<<(7-nx));
              ballDY*=-1;
              if(!(blocks[0]|blocks[1]|blocks[2]|blocks[3]|blocks[4]|blocks[5]|blocks[6]|blocks[7]))
              {
                for(i=0;i<10;++i)
                {
                  playSound(200+50*i, 60);
                  Disp8x8.delayWithRefresh(10);
                }
                gameScore += 1000;
                Disp8x8.delayWithRefresh(100);
                newLevel();
              }
            }
            break;
          }
          // slower with diagonal movement
          Timer1Period = (!ballDX || !ballDY)? ballSpeed : (ballSpeed*1.7);
          break;        
          
        case EV_TIMER_2: // COUNTDOWN BEFORE PLAY BEGINS
          if(countdown)
          {
            playSound(1000,10);
            countdown--;
          }
          else
          {
            playSound(1000,50);
            Timer2Period = 0;
            ballDY=-1;
          }
          break;
          
          
      }
      
      memcpy(Disp8x8.red,blocks,8);
      memset(Disp8x8.green,0,8);
      Disp8x8.set(playerX, 7, DISP_GREEN);
      Disp8x8.set(playerX+1, 7, DISP_GREEN);
      Disp8x8.set(ballX, ballY, DISP_YELLOW);
    }
};

