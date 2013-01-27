////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   F O U R   I N   A   R O W   G A M E 
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TIMER1PERIOD 200 // cursor flash rate
#define TIMER2PERIOD 50 // counter drop speed
class CFourInARowGame : public CGame 
{
  public:
    static void getGameIcon(byte *dst, byte count)
    {
      dst[ 0] = 0b00000000;          
      dst[ 1] = 0b00000100;          
      dst[ 2] = 0b00001100;          
      dst[ 3] = 0b00100000;          
      dst[ 4] = 0b01001000;          
      dst[ 5] = 0b01100110;          
      dst[ 6] = 0b00000100;          
      dst[ 7] = 0b00000000;          
      
      dst[ 8] = 0b00000000;          
      dst[ 9] = 0b00001000;          
      dst[10] = 0b00010000;          
      dst[11] = 0b00001100;          
      dst[12] = 0b00100100;          
      dst[13] = 0b00011000;          
      dst[14] = 0b00001000;          
      dst[15] = 0b00000000;          
    }    

    char dropRow;
    char dropCol;
    char dropType;
    byte playerMap[8];
    byte cpuMap[8];
    byte pathMap[8];
    char playerPos;
    char playerBlink;
    
    void init() 
    {
      playerBlink = 0;
      playerPos = 0;
      memset(playerMap, 0, 8);
      memset(cpuMap, 0, 8);
      memset(pathMap, 0, 8);
      Timer1Period = TIMER1PERIOD;
      Timer2Period = 0;      
    }      

      /////////////////////////////////////////////////////////////////////////////////////
      // Populate the "path map" for one of 16 paths (each 4 positions long) placed at
      // any position on the grid. Return 0 if cannot fit the path at this position
      byte loadPathMap(byte col, byte row, byte whichPath)
      {            
        memset(pathMap, 0, 8);
        byte m[8];
        byte minX, maxX, minY, maxY;
#define INIT_MAP(a,b,c,d,e,f,g,x0,xx,y0,yy) { m[0]=a;  m[1]=b;  m[2]=c;  m[3]=d;  m[4]=e;  m[5]=f;  m[6]=g; minX=x0; maxX=xx; minY=y0; maxY=yy; }
        switch(whichPath)
        {
          case  0: INIT_MAP(0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0, 4, 0, 7); break;
          case  1: INIT_MAP(0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 1, 5, 0, 7); break;
          case  2: INIT_MAP(0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 2, 6, 0, 7); break;
          case  3: INIT_MAP(0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 3, 7, 0, 7); break;                
          case  4: INIT_MAP(0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0, 7, 0, 4); break;
          case  5: INIT_MAP(0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x00, 0, 7, 1, 5); break;
          case  6: INIT_MAP(0x00, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0, 7, 2, 6); break;
          case  7: INIT_MAP(0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0, 7, 3, 7); break;                    
          case  8: INIT_MAP(0x00, 0x00, 0x00, 0x08, 0x04, 0x02, 0x01, 0, 4, 0, 4); break;
          case  9: INIT_MAP(0x00, 0x00, 0x10, 0x08, 0x04, 0x02, 0x00, 1, 5, 1, 5); break;
          case 10: INIT_MAP(0x00, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00, 2, 6, 2, 6); break;
          case 11: INIT_MAP(0x40, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 3, 7, 3, 7); break;                              
          case 12: INIT_MAP(0x00, 0x00, 0x00, 0x08, 0x10, 0x20, 0x40, 3, 7, 0, 4); break;
          case 13: INIT_MAP(0x00, 0x00, 0x04, 0x08, 0x10, 0x20, 0x00, 2, 6, 1, 5); break;
          case 14: INIT_MAP(0x00, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00, 1, 5, 2, 6); break;
          case 15: INIT_MAP(0x01, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0, 4, 3, 7); break;          
          default: return 0; // invalid
        }
        if(col<minX||col>maxX||row<minY||row>maxY)
          return 0;  // cannot be placed here
        
        byte shiftRight = (col > 4) ? (col-4) : 0;
        byte shiftLeft =  (col < 4) ? (4-col) : 0;
        int destRow = row-3;
        for(int i=0; i<7; ++i)
        {
          if(destRow >= 0 && destRow < 8)
            pathMap[destRow] = (m[i]<<shiftLeft)>>shiftRight;
          ++destRow;
        }        
        return 1;             
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Count the number of bits in the intersection between a grid and a path
    inline int countBits(byte a)
    {  
      return !!((a)&0x01)+!!(a&0x02)+!!(a&0x04)+!!(a&0x08)+!!(a&0x10)+!!(a&0x20)+!!(a&0x40)+!!(a&0x80);
    }          
    int intersect(byte *grid, byte *path)
    {
      return 
        countBits(grid[0] & path[0]) +
        countBits(grid[1] & path[1]) +
        countBits(grid[2] & path[2]) +
        countBits(grid[3] & path[3]) +
        countBits(grid[4] & path[4]) +
        countBits(grid[5] & path[5]) +
        countBits(grid[6] & path[6]) +
        countBits(grid[7] & path[7]);
    }
    
    /////////////////////////////////////////////////////////////////////////////////////
    // Get the weighted score for placing a counter in a given column, depending if 
    // playing as player or as CPU
    char getScoreForColumn(byte col, byte row, byte isCPU)
    {                  
      // try each path
      int maxScore = 0;
      byte *thisGrid = isCPU ? cpuMap : playerMap;
      byte *otherGrid = isCPU ? playerMap : cpuMap;
      for(int path=0;path<16;++path)
      {
        if(!loadPathMap(col,row,path))
          continue; // path does not fit here
          
        // check path against player counters and cpu counters
        int mine = intersect(thisGrid, pathMap);
        int yours = intersect(otherGrid, pathMap);
        if(mine >= 3) // would win
          return 200;
        if(yours >= 3) // blocking other player from a win
          return 100;
        if(mine > maxScore && yours == 0) // worth going here?
          maxScore = mine;
      }
      return maxScore;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // The AI for the CPU to make a move    
    char selectCPUColumn()
    {
      int col;
      char score[8];      
      for(col=0; col<8; ++col)
      {        
        // check if column is full
        byte m = 1<<(7-col);
        if(!!(playerMap[0]&m)||!!(cpuMap[0]&m))
        {
          score[col]=-2;
          continue;
        }
        
        // get row position where counter would fall
        int row;
        for(row=0;row<7;++row)
          if(!!(playerMap[row+1]&m) || !!(cpuMap[row+1]&m))
            break;
        
        // how much would we like to go here?
        char forPlace = getScoreForColumn(col, row, true);         
        if(forPlace > 3)
        {
          // winning or blocking a win always trumps other scores
          score[col] = forPlace;
        }
        else
        {
          // can other player win my putting a counter on top of ours?
          if(row>0 && getScoreForColumn(col, row-1, false) == 200)
          {
            // we do not want to go here!
            score[col] = -1; 
          }
          else
          {                    
            // score position based on how much it benefits us and
            // how much it disadvantages the other player
            char forBlock = getScoreForColumn(col, row, false);
            score[col] = forBlock + forPlace;                         
          }
        }
      }
      
      // whats the max score?
      int maxScore = -1;
      for(col=0;col<8;++col)
        maxScore = max(score[col], maxScore);
        
      // select a column with max score at random
      col = random(8);
      for(int i=0;i<8;++i)
      {
        if(score[col] == maxScore)
          break;
        col = (col+1)%8;
      }
      return col;
    }          
    
    /////////////////////////////////////////////////////////////////////////////////////
    // check if game is won or grid is full. Display winning line (method does not
    // return if game is over)
    void checkForWin(byte col, byte row, byte isCpu)
    {
      int i;
      byte *grid = isCpu? cpuMap : playerMap;
      
      // try each path
      for(int path=0; path<16; ++path)
      {
        // load up the path if it fits
        if(!loadPathMap(col, row, path))
          continue;
          
        // check whether pathmap is filled with counters
        byte win = 1;
        for(i=0; i<8; ++i)
        {          
          if((pathMap[i] & grid[i]) != pathMap[i])
          {
            win=0;
            break;
          }
        }
        
        // did we find a win?
        if(win)
        {
          if(isCpu)
          {
            // CPU won.. boo!
            for(i=10;i>0;--i)
            {
              playSound(200+50*i, 60);
              Disp8x8.delayWithRefresh(10);
            }
          }
          else
          {
            // player won.. woop!
            for(i=0;i<10;++i)
            {
              playSound(200+50*i, 60);
              Disp8x8.delayWithRefresh(10);
            }
          }
          
          // Infinite loop displays the winning path
          byte toggle = 0;
          for(;;)
          {
            int j;
            for(j=0; j<8; ++j)
            {
              Disp8x8.green[j]= playerMap[j];
              Disp8x8.red[j]= cpuMap[j];
              if(toggle)
              {
                Disp8x8.green[j]|=pathMap[j];
                Disp8x8.red[j]|=pathMap[j];
              }
            }
            for(j=0;j<100;++j)
              Disp8x8.refresh();
            toggle=!toggle;
          }
        }
      } 
      
      // if the grid is full we go into
      // and infinite loop
      if((playerMap[0]|cpuMap[0])==0xff)
      {
        playSound(300, 500);
        for(;;) Disp8x8.refresh();
      }
    }
    
    /////////////////////////////////////////////////////////////////////////////////////
    // Event handling
    void handleEvent(char event)
    {
  
      // load grids to display      
      memcpy(Disp8x8.green, playerMap, 8);
      memcpy(Disp8x8.red, cpuMap, 8);
      
      int i;
      if(Timer2Period) // when drop is in progress all other controls are disabled
      {
        if(event == EV_TIMER_2)
        {
          playSound(400+ dropRow * 200, 2);
          Disp8x8.set(dropCol,dropRow,dropType? DISP_RED : DISP_GREEN);
          int nextRow = dropRow + 1;
          
          // did we get to end of drop?
          if(nextRow > 7 || ((playerMap[nextRow] & (1<<(7-dropCol))) || (cpuMap[nextRow] & (1<<(7-dropCol)))))
          {
            // place counter in grid
            if(dropType) 
              cpuMap[dropRow] |= (1<<(7-dropCol));
            else
              playerMap[dropRow] |= (1<<(7-dropCol));
              
            // see if game is over
            checkForWin(dropCol, dropRow, dropType);

            // whose goes next?
            if(dropType)
            {
              // back to player
              Timer2Period = 0;
              dropType = 0; 
            }
            else
            {
              // over to CPU
              dropCol = selectCPUColumn();
              dropRow = 0;
              dropType = 1;
            }
          }
          else
          {
            // still dropping
            dropRow = nextRow;
          }
        }
      }
      else
      {
        switch(event)
        {
            case EV_PRESS_B: // CURSOR LEFT
              if(--playerPos < 0) playerPos=7;
               break;
            case EV_PRESS_D: // CURSOR RIGHT
              if(++playerPos > 7) playerPos=0;
               break;
               
            case EV_PRESS_C: // DROP
               if((playerMap[0] & (1<<(7-playerPos))) || (cpuMap[0] & (1<<(7-playerPos))))
               {
                 playSound(20,250);
                 break; // cant drop if the column is full!
               }                 
               dropRow = 0;
               dropCol = playerPos;
               dropType = 0;
               Timer2Period = TIMER2PERIOD;
               break;
  
            // TIMER 1 - BLINK THE CURSOR             
            case EV_TIMER_1:
              playerBlink = !playerBlink;        
        }               
        
        // blink cursor
        if(playerBlink)
          Disp8x8.set(playerPos,0,DISP_YELLOW);
      }
    }
};


