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
#define TIMER1PERIOD 100
#define TIMER2PERIOD 100
class CFourInARowGame : public CGame 
{
  public:
    static void getGameIcon(byte *dst)
    {
      dst[ 0] = 0b00000000;          
      dst[ 1] = 0b00001100;          
      dst[ 2] = 0b00011100;          
      dst[ 3] = 0b00101100;          
      dst[ 4] = 0b01101100;          
      dst[ 5] = 0b01111110;          
      dst[ 6] = 0b00001100;          
      dst[ 7] = 0b00000000;          
      
      dst[ 8] = 0b00000000;          
      dst[ 9] = 0b00000000;          
      dst[10] = 0b00000000;          
      dst[11] = 0b00000000;          
      dst[12] = 0b00000000;          
      dst[13] = 0b00000000;          
      dst[14] = 0b00000000;          
      dst[15] = 0b00000000;          
      
    }    

    char placeForCPU;
    char placeCol;
    char placeRow;    
    char cursorCol;    
    byte flashCursor;
    
    void calc(int x1, int y1, int x2, int y2, int x3, int y3, byte thisColour, byte otherColour, int *maxScore, int tag)
    {      
      if(x1<0 || y1<0 || x2<0 || y2<0 || x3<0 || y3<0)
        return;
      if(x1>7 || y1>7 || x2>7 || y2>7 || x3>7 || y3>7)
        return;
      int s = 0;
      byte r1 = Disp8x8.get(x1,y1);
      byte r2 = Disp8x8.get(x2,y2);
      byte r3 = Disp8x8.get(x3,y3);
      if(r1 == otherColour || r2 == otherColour || r3 == otherColour)
        return;
      if(r1 == thisColour) ++s;      
      if(r2 == thisColour) ++s;      
      if(r3 == thisColour) ++s;      
      if(s>*maxScore)    
        *maxScore = s;
    }
/*
    char getPos(char pos, char orientation, char *p1, char *p2, char *p3)
    {
      #define CALC_POS(a,b,c) *p1=(pos+a); *p2=(pos+b); *p3=(pos+c);
      switch(orientation)
      {
        case  0: CALC_POS(1,2,3); break;
        case  1: CALC_POS(-1,1,2); break;
        case  2: CALC_POS(-2,-1,1); break;
        case  3: CALC_POS(-3,-2,-1); break;
        case  4: CALC_POS(7,14,21); break;
        case  5: CALC_POS(-7,7,14); break;
        case  6: CALC_POS(-14,-7,7); break;
        case  7: CALC_POS(-21,-14,-7); break;
        case  8: CALC_POS(9,18,27); break;
        case  9: CALC_POS(-9,9,18); break;
        case 10: CALC_POS(-18,-9,9); break;
        case 11: CALC_POS(-27,-18,-9); break;
        case 12: CALC_POS(8,16,24); break;
        case 13: CALC_POS(-8,8,16); break;
        case 14: CALC_POS(-16,-8,8); break;
        case 15: CALC_POS(-24,-16,-8); break;
        default:
          return 0;
      }
      if(*p1<0||*p1>63||*p2<0||*p2>63||*p3<0||*p3>63)
        return 0;
      return 1;
    }
*/    
    
    int scoreMove(int x, int y, byte thisColour, byte otherColour)
    {
     int maxScore = 0;
     // horizontal
     // X000 0X00 00X0 000X     
     calc(  x+1,   y,   x+2,   y,   x+3,   y,   thisColour, otherColour, &maxScore,1);
     calc(  x-1,   y,   x+1,   y,   x+2,   y,   thisColour, otherColour, &maxScore,2);
     calc(  x-2,   y,   x-1,   y,   x+1,   y,   thisColour, otherColour, &maxScore,3);
     calc(  x-3,   y,   x-2,   y,   x-1,   y,   thisColour, otherColour, &maxScore,4);
     
     // vertical   X  O  O  O
     //            O  X  O  O
     //            O  O  X  O
     //            O  O  O  X
     calc(  x,   y+1,   x,   y+2,   x,   y+3,   thisColour, otherColour, &maxScore,5);
     calc(  x,   y-1,   x,   y+1,   x,   y+2,   thisColour, otherColour, &maxScore,6);
     calc(  x,   y-2,   x,   y-1,   x,   y+1,   thisColour, otherColour, &maxScore,7);
     calc(  x,   y-3,   x,   y-2,   x,   y-1,   thisColour, otherColour, &maxScore,8);
     
     // diagonal X     O     O     O
     // TL-BR     O     X     O     O
     //            O     O     X     O
     //             O     O     O     X
     calc(  x+1,   y+1,   x+2,   y+2,   x+3,   y+3,   thisColour, otherColour, &maxScore,9);
     calc(  x-1,   y-1,   x+1,   y+1,   x+2,   y+2,   thisColour, otherColour, &maxScore,10);
     calc(  x-2,   y-2,   x-1,   y-1,   x+1,   y+1,   thisColour, otherColour, &maxScore,11);
     calc(  x-3,   y-3,   x-2,   y-2,   x-1,   y-1,   thisColour, otherColour, &maxScore,12);
     
     // diagonal     X     O     O     O
     // BL-TR       O     X     O     O
     //            O     O     X     O
     //           O     O     O     X
     calc(  x-1,   y+1,   x-2,   y+2,   x-3,   y+3,   thisColour, otherColour, &maxScore,13);
     calc(  x+1,   y-1,   x-1,   y+1,   x-2,   y+2,   thisColour, otherColour, &maxScore,14);
     calc(  x+2,   y-2,   x+1,   y-1,   x-1,   y+1,   thisColour, otherColour, &maxScore,15);
     calc(  x+3,   y-3,   x+2,   y-2,   x+1,   y-1,   thisColour, otherColour, &maxScore,16);
     
     return maxScore;
    }
    
    void cpuMove()
    {
      int row, col;
      int score[8] = {0};
      // for each column
      for(col=0;col<8;++col)
      {        
        // check if the column is full
        if(Disp8x8.get(col,0))
        {
          score[col] = -2;
          continue;
        }
        
        // find the lowest available
        // slot in this column
        for(row=0;row<7;++row)
        {
          if(Disp8x8.get(col,row+1))
            break;
        }
        
        // check how good this position is for
        // purpose of winning
        score[col] = scoreMove(col, row, DISP_RED, DISP_GREEN);
        if(score[col] == 3)
        {
          // a certain win! force us to go here
          score[col] = 4;
          continue;
        }
        else
        {
          int s;
          // check if moving here exposes a win for other player
          if(row>0)
          {
            s = scoreMove(col, row-1, DISP_GREEN, DISP_RED);
            if(s == 3)
            {
              score[col] = -1; // avoid it like hell
              continue;
            }
          }
          else
          {
            // check how good this position is
            // from aspect of blocking other player
            int s = scoreMove(col, row, DISP_GREEN, DISP_RED);
            if(s > score[col])
              score[col] = s;
          }          
        }
      }
      
      // find out which column has the highest score
      int maxScore = 0;
      placeCol = 0;
      for(col=0;col<8;++col)
      {
        if(score[col] == -2)
          continue;
        if(score[col] > maxScore)
        {
          placeCol = col;
          maxScore = score[col];
        }                    
      }            

      // randomize among columns with same score
      int shuffle = random(8);
      col = placeCol;
      while(shuffle--)
      {
        if(score[col] == maxScore)
          placeCol = col;
        if(++col>7)
          col=0;
      }
      placeRow = 0;
      placeForCPU = 1;
      Disp8x8.set(placeCol, 0, DISP_RED);                
      Timer2Period = TIMER2PERIOD;
    }
    
    void init()
    {
      placeForCPU = 0;
      placeCol = 0;
      placeRow = 0;
      flashCursor = 0;
      cursorCol = 0;
      Timer1Period = TIMER1PERIOD;
      Timer2Period = 0;
      
      Serial.begin(9600);
    }
    void handleEvent(char event)
    {
      
      int i;
      switch(event)
      {
          case EV_PRESS_A:
          case EV_PRESS_C:
            if(Timer1Period)
            {
               Disp8x8.set(cursorCol, 0, DISP_GREEN);                
               if(Disp8x8.get(cursorCol, 1)) // check if the counter is inserted on top row
               {
                 for(i=0;i<8;++i) // need to move the cursor to next free column
                 {
                   if(++cursorCol > 7)
                     cursorCol = 0;
                   if(!Disp8x8.get(cursorCol, 0))
                     break;
                 }
               }
               placeRow = 0;
               placeCol = cursorCol;
               placeForCPU = 0;
               Timer2Period = TIMER2PERIOD;
               Timer1Period = 0;
            }
            break;
            
          case EV_PRESS_B:
            if(Timer1Period)
            {
               Disp8x8.set(cursorCol, 0, DISP_OFF);
               for(i=0;i<8;++i)
               {
                 if(--cursorCol < 0)
                   cursorCol = 7;
                 if(!Disp8x8.get(cursorCol, 0))
                   break;
               }
               Disp8x8.set(cursorCol, 0, DISP_GREEN);
             }
             break;
             
          case EV_PRESS_D:
             if(Timer1Period)
             {
               Disp8x8.set(cursorCol, 0, DISP_OFF);
               for(i=0;i<8;++i)
               {
                 if(++cursorCol > 7)
                   cursorCol = 0;
                 if(!Disp8x8.get(cursorCol, 0))
                   break;
               }
               Disp8x8.set(cursorCol, 0, DISP_GREEN);
             }
             break;

        case EV_TIMER_1: // TIMER1 FLASHES CURSOR
            flashCursor++;
            Disp8x8.set(cursorCol, 0, (flashCursor&3)? DISP_GREEN : DISP_YELLOW);
            break;
        case EV_TIMER_2: // TIMER FOR DROPPING PLAYER COUNTER        
            if(placeRow == 7 || Disp8x8.get(placeCol, placeRow+1))
            {
              Timer2Period = 0;
              if(placeForCPU)
              {
                Timer1Period = TIMER1PERIOD;
              }
              else
              {
                cpuMove();
              }
            }
            else
            {
              Disp8x8.set(placeCol, placeRow, DISP_OFF);              
              placeRow++;
              Disp8x8.set(placeCol, placeRow, placeForCPU? DISP_RED : DISP_GREEN);              
            }
            break;
      }
      
    }
};


