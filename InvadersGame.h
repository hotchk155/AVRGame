////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   I N V A D E R S     G A M E 
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CInvadersGame : public CGame 
{
  char playerX;
  char invaders[3][3];
  char invadersX;
  char invadersY;
  char invadersDir;
  char maxIX;
  char minIX;
  char playerCaught;
  char invaderCount;
  char toggle;
  char bulletX;
  char bulletY;
  char bulletStop;
  char bombX[8];
  char mothershipX;
  byte buildings[2];
  unsigned long nextShotMove;
  int invaderPeriod;
  public:
    static void getGameIcon(byte *dst, byte count)
    {
      if(count & 0x3f)//sinister blinking
        dst[ 2] = 0b00100100;          
      dst[ 9] = 0b00111100;
      dst[10] = 0b01011010;
      dst[11] = 0b01111110;
      dst[12] = 0b00011000;
      dst[13] = 0b00100100;
      dst[14] = 0b01000010;
    }
    
    void init()
    {
      invaderPeriod = 600;
      playerX = 4;
      newScreen();
    }
    void newScreen()
    {
      playerCaught = 0;
      mothershipX = -1;
      bulletY = -1;
      Disp8x8.cls();
      Disp8x8.green[5] = 0b01011010;
      Disp8x8.green[6] = 0b01011010;
      Disp8x8.set(playerX,7,DISP_RED);
      memset(invaders,1,sizeof invaders);
      invadersX = 1;
      invadersY = 1;
      invadersDir = 1;
      toggle = 1;
      Timer1Period = invaderPeriod;
      Timer3Period = 400;
      Timer4Period = 400;
      memset(bombX,-1,sizeof bombX);
      invaderPeriod -= 50;
    }
    void renderInvaders(char colour)
    {
      maxIX = -1;
      minIX = 8;
      invaderCount = 0;
      for(int iy = 0; iy < 3; ++iy) 
        for(int ix = 0; ix < 3; ++ix) 
          if(invaders[ix][iy])
          {
            invaderCount++;
            int col = invadersX + 2*ix;
            int row = invadersY + iy;
            Disp8x8.set(col, row, colour);
            if(col < minIX) minIX = col;
            if(col > maxIX) maxIX = col;
            if(row == 7 && col == playerX)       
              playerCaught = 1;
          }
    }
    byte checkForHit()
    {
      for(int iy = 0; iy < 3; ++iy) 
      {
        for(int ix = 0; ix < 3; ++ix) 
        {
          if(invaders[ix][iy])
          {
            int col = invadersX + 2*ix;
            int row = invadersY + iy;
            if(bulletX == col && bulletY == row)
            {
              invaders[ix][iy] = 0;
              return 1;
            }
          }
       }
      }
      return 0;
    }
    void dropBomb()
    {
      byte which = random(invaderCount);
      for(int iy = 0; iy < 3; ++iy) 
      {
        for(int ix = 0; ix < 3; ++ix) 
        {          
          if(invaders[ix][iy])
          {
            if(!which)
            {
              int col = invadersX + 2*ix;
              int row = invadersY + iy;
              if(bombX[row] < 0)
                bombX[row] = col;
              return;
            }
            --which;
          }
        }
      }
    }
    void handleEvent(char event)
    {
      int i;
      switch(event)
      {
        case EV_PRESS_B:
          if(playerX > 0)
          {
            Disp8x8.set(playerX,7,DISP_OFF);
            playerX--;
          }
          break;
        case EV_PRESS_D:
          if(playerX < 7)
          {
            Disp8x8.set(playerX,7,DISP_OFF);
            playerX++;
          }      
          break;
        case EV_PRESS_A:
        case EV_PRESS_C:
          if(!Timer2Period)
          {
            bulletY = 7;
            bulletX = playerX;
            bulletStop = 0;
            Timer2Period = 80;
            playSound(1000,20);
          }      
          break;

            
        // TIMER1 USED FOR MOVEMENT OF INVADERS            
        case EV_TIMER_1:          
          if(toggle)
          {
            playSound(400,100);
            renderInvaders(DISP_OFF);
            if(maxIX >= 7 && invadersDir > 0)
            {
              invadersDir = -1;
            }
            else if(minIX <= 0 && invadersDir < 0)
            {
              invadersDir = 1;
              invadersY++;
            }
            else
            {
              invadersX += invadersDir;
            }          
          }
          else
          {
           playSound(200,100);
          }
          toggle = !toggle;
          break;      
          
        // TIMER2 USED FOR PLAYER BULLET
        case EV_TIMER_2:   
            Disp8x8.set(bulletX,bulletY,DISP_OFF);
            bulletY--;
            if(bulletStop || bulletY<0)
            {
              Timer2Period = 0;
            }
            else if(checkForHit())
            {
              gameScore += 25; // hit invader
              playSound(800,100);
              if(Timer1Period > 100)
                Timer1Period -= 60;
              bulletStop = 1;
            }
            else if(bulletY==0 && bulletX==mothershipX)
            {
              mothershipX=-1;
              playSound(2000,300);
              bulletStop = 1;
              gameScore += 250; // mothership bonus
            }
            else if(Disp8x8.get(bulletX,bulletY))
            {
              bulletStop = 1;
            }
            break;
            
        // TIMER3 USED FOR ALIEN BOMBS
        case EV_TIMER_3:                  
            for(i=7; i>0; --i)
            {
              if(bombX[i] > 0)
                Disp8x8.set(bombX[i],i,DISP_OFF);
              if(bombX[i] > 7)
                bombX[i] = -1;
            }
            for(i=7; i>0; --i)
            {
              bombX[i] = bombX[i-1];
              if(Disp8x8.get(bombX[i],i) == DISP_GREEN)
                bombX[i]+=8;
            }

            bombX[0] = -1;
            if(!random(5))
              dropBomb();
            break;
            
        // TIMER4 runs mothership at top of screen
        case EV_TIMER_4:          
          if(mothershipX >= 0)
          {
            Disp8x8.set(mothershipX,0,DISP_OFF);
            if(++mothershipX > 7)
              mothershipX = -1;
          }
          else if(!random(30))
          {
            mothershipX = 0;
          }          
          break;
      }
      renderInvaders(DISP_RED);
      if(!invaderCount)
      {
         for(char i=0;i<32;++i)
         {
           playSound(500 + i*20,20);
           delay(20);
         }
         gameScore += 200; // level clearing bonus
         newScreen();
      }
      Disp8x8.set(playerX,7,DISP_RED);
      for(int i=0; i<8; ++i)        
        if(bombX[i] > 0)
          Disp8x8.set(bombX[i]%8,i,DISP_YELLOW);
      if(Timer2Period)
        Disp8x8.set(bulletX,bulletY,DISP_YELLOW);
      if(mothershipX >= 0)
        Disp8x8.set(mothershipX,0,DISP_GREEN);
      if((bombX[7] == playerX) || playerCaught)
      {
         for(char i=32;i>0;--i)
         {
           playSound(500 + i*20,20);
           delay(20);
         }
         endGame();
      }     
    }
};
CGameFactoryImpl<CInvadersGame> InvadersGame;
