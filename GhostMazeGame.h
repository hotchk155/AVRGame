////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   G H O S T   M A Z E   G A M E
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define GM_NUM_ROWS  19
#define GM_COL_WALL 0xF0
#define GM_COL_FOOD 0x13
#define GM_COL_PLAYER 0xFF
#define GM_NUM_GHOSTS 4

typedef struct {
  char x;
  char y;
  char dir;
} GM_GHOST;

class CGhostMazeGame : public CGame 
{
    char playerX;
    char playerY;
    char originX;
    char originY;
    char ghostDelay;    
    byte livesRemaining;
    byte powerColour;
    byte ghostColour;
    byte remainingFood;
    int powerCount;
    char powerX[4];    
    char powerY[4];    
    byte buffer8[64];
    int ghostPace;
    unsigned int maze[GM_NUM_ROWS];
    unsigned int food[GM_NUM_ROWS];
    GM_GHOST ghost[GM_NUM_GHOSTS];
    
    enum {
        SOUND_EATGHOST,
        SOUND_POWERUP,
        SOUND_DIE,
        SOUND_COMPLETELEVEL
    };
    
  public:
  
    static void getGameIcon(byte *dst, byte count)
    {
      dst[ 0] = 0b00000000;          
      dst[ 1] = 0b00011100;          
      dst[ 2] = 0b00111110;          
      dst[ 3] = 0b01101100;          
      dst[ 4] = 0b01111000;          
      dst[ 5] = 0b01111100;          
      dst[ 6] = 0b00111110;          
      dst[ 7] = 0b00011100;          
      
      if((count & 0b11) == 0b00)
      {      
        dst[ 0] = 0b00000000;          
        dst[ 1] = 0b00011000;          
        dst[ 2] = 0b00111100;          
        dst[ 3] = 0b01101110;          
        dst[ 4] = 0b01111110;          
        dst[ 5] = 0b01111110;          
        dst[ 6] = 0b00111100;          
        dst[ 7] = 0b00011000;          
      }
      memcpy(dst+8, dst,8);
    }    
    
    // START GAME
    void init()
    {
      ghostPace = 500;
      livesRemaining = 3;
      Disp8x8.setBuffer8(buffer8);
      Disp8x8.cls();
      maze[ 0] = 0b1111111111111111;
      maze[ 1] = 0b1000000010000000;
      maze[ 2] = 0b1011011010110110;
      maze[ 3] = 0b1000000000000000;
      maze[ 4] = 0b1011010111010110;
      maze[ 5] = 0b1000010010010000;
      maze[ 6] = 0b1111011010110111;
      maze[ 7] = 0b1111010000010111;
      maze[ 8] = 0b0000000111000000;
      maze[ 9] = 0b1111010000010111;
      maze[10] = 0b1111010111010111;
      maze[11] = 0b1000000010000000;
      maze[12] = 0b1011011010110110;
      maze[13] = 0b1001000000000100;
      maze[14] = 0b1101010111010101;
      maze[15] = 0b1000010010010000;
      maze[16] = 0b1011111010111110;
      maze[17] = 0b1000000000000000;
      maze[18] = 0b1111111111111111;     
      startLevel();      
    }

    // START LEVEL
    void startLevel()
    {
      powerX[0] = 1;
      powerY[0] = 2;
      powerX[1] = 15;
      powerY[1] = 2;
      powerX[2] = 1;
      powerY[2] = 12;
      powerX[3] = 15;
      powerY[3] = 12;

      food[ 0] = 0b0000000000000000;
      food[ 1] = 0b0111111101111111;//14
      food[ 2] = 0b0000100101001000;//4
      food[ 3] = 0b0111111111111111;//15
      food[ 4] = 0b0100101000101001;//6
      food[ 5] = 0b0111101101101111;//12
      food[ 6] = 0b0000100101001000;//4
      food[ 7] = 0b0000100000001000;//2
      food[ 8] = 0b0000100000001000;//2
      food[ 9] = 0b0000100000001000;//2
      food[10] = 0b0000101000101000;//4
      food[11] = 0b0111111101111111;//14
      food[12] = 0b0000100101001000;//4
      food[13] = 0b0110111101111011;//12
      food[14] = 0b0010101000101010;//6
      food[15] = 0b0111101101101111;//12
      food[16] = 0b0100000101000001;//4
      food[17] = 0b0111111111111111;//15
      food[18] = 0b0000000000000000;     
      remainingFood = 14 + 4 + 15 + 6 + 12 + 4 + 2 + 2 + 2 + 4 + 14 + 4 + 12 + 6 + 12 + 4 + 15;
      resetLevel();
    }

    // RESET PLAYER AND GHOST POSITIONS
    void resetLevel()
    {
      powerColour = 0xF8;
      ghostColour = 0x0F;
      powerCount = 0;
      ghostDelay = 25;
          
      for(int i=0; i<GM_NUM_GHOSTS; ++i)
      {
        ghost[i].x=8;
        ghost[i].y=8;
        ghost[i].dir=0;
      }

      originX = 4;
      originY = 9;
      playerX = 8;
      playerY = 13;

      Timer1Period = 0;
      Timer2Period = 0; // ghosts only start moving after initial delay
      Timer3Period = 100;
    }

    // SOUNDS
    void soundEffect(byte which)
    {
      int i;
      switch(which)
      {
        case SOUND_COMPLETELEVEL:
          for(i=200; i<=1400; i+=200)
          {
            playSound(i-100, 100);
            Disp8x8.delayWithRefresh(100);
            playSound(i, 50);
            Disp8x8.delayWithRefresh(100);
            playSound(i+100, 100);
            Disp8x8.delayWithRefresh(100);
          }
          break;
        case SOUND_DIE:
          for(i=600; i>100; i-=100)
          {
            playSound(i, 50);
            Disp8x8.delayWithRefresh(50);
            playSound(i+100, 50);
            Disp8x8.delayWithRefresh(200);
          }
          break;
        case SOUND_EATGHOST:
          for(i=600; i<1000; i+=50)
          {
            playSound(i, 20);
            Disp8x8.delayWithRefresh(20);
          }
          break;
        case SOUND_POWERUP:
          for(i=0; i<3; i++)
          {
            playSound(1500, 30);
            Disp8x8.delayWithRefresh(30);
            playSound(1000, 30);
            Disp8x8.delayWithRefresh(30);
          }
          break;
      }
    }
    
    // DISPLAY UPDATE
    void updateDisplay()
    {
      int row, col, index;
      unsigned int mask;;
      Disp8x8.cls();
      for(row=0; row<8; ++row)
      {
        index = row + originY;
        if(index >= GM_NUM_ROWS)
          break;
        mask = 1<<(15-originX);
        for(col=0; col<8;++col)
        {
          if(!mask)
          {
            Disp8x8.set8(col,row,(index==8)?0:GM_COL_WALL); // row 8 contains the tunnel :)
          }
          else if(!!(maze[index] & mask))
          {
            Disp8x8.set8(col,row,GM_COL_WALL);
          }
          else if(!!(food[index] & mask))
          {
            Disp8x8.set8(col,row,GM_COL_FOOD);
          }
          mask>>=1;
        }
      }

      // power ups
      for(index = 0; index <4; ++index)
      {
        if(!powerX[index])
          continue;
        col = powerX[index]-originX;
        row = powerY[index]-originY;
        if(col>=0 && col<8 && row>=0 && row<8)
          Disp8x8.set8(col,row,powerColour);                    
      }
      
      // player (timer1 is used to "flicker" when eating food)
      if(!Timer1Period)
      {
        col = playerX-originX;
        row = playerY-originY;
        if(col>=0 && col<8 && row>=0 && row<8)
          Disp8x8.set8(col,row,GM_COL_PLAYER);            
      }
      
      // ghosts
      for(index = 0; index < GM_NUM_GHOSTS; ++index)
      {
        col = ghost[index].x-originX;
        row = ghost[index].y-originY;
        if(col>=0 && col<8 && row>=0 && row<8)
          Disp8x8.set8(col,row,ghostColour);                    
      }
    }
    
    // CHECK FOR WALLS/FOOD IN BITMAP
    byte checkMap(int x, int y, unsigned int *m)
    {      
      if(x>=0 && x<16 && y>=0 && y<GM_NUM_ROWS)
        return !!(m[y]&(1<<(15-x)));
      if(y==8 && x==16)
        return 0;
      return 1;
    }

    // TRY TO MOVE PLAYER 
    byte  movePlayer(int dx, int dy)
    {
      // Tunnel
      if(playerY == 8 && playerX == 1 && dx < 0)
      {
        originX = 9;
        playerX = 15;
      }
      else if(playerY == 8 && playerX == 15 && dx > 0)
      {
        originX = 0;
        playerX = 1;
      }
      else
      {        
        // Check for walls
        if(dx<0 && checkMap(playerX-1, playerY, maze))
          return 0;
        if(dx>0 && checkMap(playerX+1, playerY, maze))
          return 0;
        if(dy<0 && checkMap(playerX, playerY-1, maze))
          return 0;
        if(dy>0 && checkMap(playerX, playerY+1, maze))
          return 0;
        playerX+=dx;
        playerY+=dy;
        
        // scroll the window
        if(playerX>originX+5 && originX<11)
          originX++;
        if(playerX<originX+3 && originX>0)
          originX--;
        if(playerY>originY+5 && originY<(GM_NUM_ROWS-8))
          originY++;
        if(playerY<originY+3 && originY>0)
          originY--;
      }

      // collision check with powerup
      for(int i=0;i<4;++i)
      {
        if(powerX[i] == playerX && powerY[i] == playerY)
        {
          gameScore += 50;
          soundEffect(SOUND_POWERUP);
          powerX[i] = 0;
          powerCount = 80;
          break;
        }
      }      
      
      // collision check with food
      if(checkMap(playerX, playerY, food))
      {
        gameScore += 10;
        food[playerY]&=~(1<<(15-playerX));
        if(--remainingFood == 0)
        {
          gameScore += 250;
          if(ghostPace > 100)
            ghostPace-=50;
          updateDisplay();
          soundEffect(SOUND_COMPLETELEVEL);
          startLevel();          
        }
        else
        {
          setTimeOut(1, 80); // makes player flash when eating
          playSound(200,50);
        }
      }
      
      // collision check with ghosts
      collisionDetection(); 
    }


    // MOVE A GHOST
    byte moveGhost(GM_GHOST *pG, char targetX, char targetY)
    {
            
      byte up_wall = checkMap(pG->x, pG->y-1, maze);
      byte down_wall = checkMap(pG->x, pG->y+1, maze);
      byte left_wall = checkMap(pG->x-1, pG->y, maze);
      byte right_wall = checkMap(pG->x+1, pG->y, maze);
      switch(pG->dir)
      {
        case 0: // NOT STARTED
          if(!random(20))
              pG->dir = 1;
          break;
        case 1://UP
            if(targetX < pG->x && !left_wall)
              pG->dir=4;
            else if(targetX > pG->x && !right_wall)
              pG->dir=2;
            else if(!left_wall && !random(2))
              pG->dir=4;
            else if(!right_wall && !random(2))
              pG->dir=2;
            else if(up_wall)
              pG->dir=3;              
            break;
        case 2://RIGHT
            if(targetY < pG->y && !up_wall)
              pG->dir=1;
            else if(targetY > pG->y && !down_wall)
              pG->dir=3;
            else if(!up_wall && !random(2))
              pG->dir=1;
            else if(!down_wall && !random(2))
              pG->dir=3;
            else if(right_wall)
              pG->dir=4;              
            break;
        case 3://DOWN
            if(targetX < pG->x && !left_wall)
              pG->dir=4;
            else if(targetX > pG->x && !right_wall)
              pG->dir=2;
            else if(!left_wall && !random(2))
              pG->dir=4;
            else if(!right_wall && !random(2))
              pG->dir=2;
            else if(down_wall)
              pG->dir=1;              
            break;
        case 4://LEFT
            if(targetY < pG->y && !up_wall)
              pG->dir=1;
            else if(targetY > pG->y && !down_wall)
              pG->dir=3;
            else if(!up_wall && !random(2))
              pG->dir=1;
            else if(!down_wall && !random(2))
              pG->dir=3;
            else if(left_wall)
              pG->dir=2;              
            break;
      }            
      pG->x += (pG->dir == 2) - (pG->dir == 4);
      pG->y += (pG->dir == 3) - (pG->dir == 1);
    }

    // CHECK FOR COLLISION BETWEEN GHOST AND PLAYER
    void collisionDetection()
    {
      for(int i = 0; i < GM_NUM_GHOSTS; ++i)
      {
         if(ghost[i].x == playerX && ghost[i].y == playerY)
         {
           if(powerCount)
           {
              // player eats ghost
              gameScore += 100;
              soundEffect(SOUND_EATGHOST);
              ghost[i].x=8;
              ghost[i].y=8;
              ghost[i].dir=0;
           }
           else
           {
              // ghost eats player
              updateDisplay();
              soundEffect(SOUND_DIE);
              if(!--livesRemaining)
                endGame();
               resetLevel();
           }
         }
      }      
    }
    
    // EVENT HANDLER
    void handleEvent(char event)
    {
      int i;
      switch(event)
      {
        
        // BUTTONS
        case EV_PRESS_A:
          movePlayer(0,-1);
          break;
        case EV_PRESS_B:
          movePlayer(-1,0);
          break;
        case EV_PRESS_C:
          movePlayer(0,1);
          break;
        case EV_PRESS_D:
          movePlayer(1,0);
          break;
          
        // Timer1 is used to control the 2-tone "gobble" sound and flicker of player
        case EV_TIMER_1:  
          playSound(100,50);
          Timer1Period = 0;
          break;
          
        // Timer2 moves the ghosts
        case EV_TIMER_2: 
          {  
              char targetX = playerX;
              char targetY = playerY;
              if(powerCount)
              {
                // if the player is powered up the ghosts are 
                // sent to the opposite corner of the maze
                targetX = (playerX < 8) ? 18 : 0;
                targetY = (playerY < 8) ? 16 : 0;
              }

              for(i=0;i<GM_NUM_GHOSTS;++i)
                moveGhost(&ghost[i], targetX, targetY);
              collisionDetection();
          }
          break;
          
        // Timer 3 used for general colour animation and ticking sounds
        case EV_TIMER_3:
        
          // ghosts flicker when player is powered up
          if(powerCount)
          {
            switch(ghostColour)
            {
              case 0xF0: ghostColour = 0x0F; break;
              default: ghostColour = 0xF0; break;
            }
            
            // ticking timer during powerup
            if(powerCount > 20)
            {
              // slow tick
              if(!(powerCount%5))
                playSound(1200,10);
            }
            else 
            {
              // fast tick
              if(!(powerCount%2))
                playSound(1400,10);
            }
            powerCount--;
          }
          else
          {
            // normal ghost colour
            ghostColour = 0x0F;
          }
          
          // Short delay at start of level where
          // ghosts stay at home
          if(ghostDelay)
          {
            // ticking
            if(!(ghostDelay % 5))
                playSound(1000,10);
            ghostDelay--;
          }
          else
          {
              // start ghost ai
              Timer2Period = ghostPace;
          }
          
          // flickering powerups
          switch(powerColour)
          {
            case 0x11: powerColour = 0x33; break;
            case 0x33: powerColour = 0x77; break;
            case 0x77: powerColour = 0xFF; break;
            default: powerColour = 0x11; break;
          }
          break;
      }

      // update display with each event
      updateDisplay();
    }
};
CGameFactoryImpl<CGhostMazeGame> GhostMazeGame;



