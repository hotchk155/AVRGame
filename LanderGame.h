////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   L A N D E R     G A M E 
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CLanderGame : public CGame 
{
  byte maxValue[32];
  byte minValue[32];
  byte bases[32];
  float landerX, landerY;
  float velocityX, velocityY;
  int ox, oy;
  byte boostDir;
  byte flashBases;
  byte flashFlame;
  byte flashVertical;
  byte flashHorizontal;
  int fuel;
  public:
    static void getGameIcon(byte *dst)
    {
        dst[ 0] = 0b00000000;
        dst[ 1] = 0b00000000;
        dst[ 2] = 0b00000000;
        dst[ 3] = 0b00000000;
        dst[ 4] = 0b00000000;
        dst[ 5] = 0b00011000;
        dst[ 6] = 0b00111100;
        dst[ 7] = 0b00011000;
        
        dst[ 8] = 0b00000000;
        dst[ 9] = 0b00111100;
        dst[10] = 0b01011010;
        dst[11] = 0b01111110;
        dst[12] = 0b11111111;
        dst[13] = 0b10010001;
        dst[14] = 0b10101001;
        dst[15] = 0b00010000;
    }
  
    void init()
    {
      byte maxValueInit[32] = {13, 	14, 	15, 	15, 	14, 	12, 	13, 	12, 	9, 	6, 	3, 	0, 	2, 	4, 	6, 	8, 	11, 	12, 	12, 	11, 	9, 	7, 	8, 	10, 	11, 	10, 	11, 	12, 	11, 	10, 	11, 	12};        
      byte minValueInit[32] = {13, 	14, 	15, 	15, 	13, 	12, 	13, 	10, 	7, 	4, 	1, 	0, 	1, 	3, 	5, 	7, 	9, 	12, 	12, 	10, 	8, 	7, 	8, 	9, 	11, 	10, 	11, 	12, 	11, 	10, 	11, 	12};   
      memcpy(maxValue,maxValueInit,32);
      memcpy(minValue,minValueInit,32);
      memset(bases,0,32);
      bases[2] = 15;
      bases[3] = 15;
      bases[6] = 13;
      bases[9] = 4;
      bases[17] = 12;
      bases[18] = 12;
      bases[24] = 11;
      bases[29] = 10;
      Disp8x8.cls();
      ox=8;
      oy=-7;
      velocityX=0;
      velocityY=0;
      landerX=12;
      landerY=-5;
      boostDir = 3;
      flashBases = 1;
      flashFlame = 0;
      flashVertical = 0;
      flashHorizontal = 0;
      Timer1Period = 500;
      Timer3Period = 100;
      fuel=1000;
      handleEvent(EV_START);
    }
    void setGrid(int originCol, int originRow, byte showBases)
    {
      Disp8x8.cls();      
      for(int col=0;col<8;++col)
      {
        int index = (originCol + col) % 32;
        int maxRow = (int)maxValue[index] - originRow;
        int minRow = (int)minValue[index] - originRow;
        int baseRow = bases[index];
        if(baseRow) baseRow-=originRow;
        for(int row=0;row<8;++row)
        {
          if(showBases && baseRow>0 && row == baseRow)
            Disp8x8.set(col,row,DISP_YELLOW);
          else if(row>=minRow && row<=maxRow)
            Disp8x8.set(col,row,DISP_GREEN);
        }
      }
    }
    void handleEvent(char event)
    {
        int xx,yy;
        float XX;
        bool showFuel = 0;
        switch(event)
        {
        case EV_PRESS_A:
          break;          
        case EV_PRESS_B:
          if(boostDir>0) --boostDir;
          break;          
        case EV_PRESS_D:
          if(boostDir<6) ++boostDir;
          break;          
        case EV_PRESS_C:
          Timer2Period = 30;
          break;          
        case EV_RELEASE_C:
          Timer2Period = 0;
          flashFlame = 0;
          break;          
        case EV_TIMER_1: // Timer 1 flashes bases          
          flashBases = !flashBases;
          break;          
        case EV_TIMER_2: // Timer 2 flashes exhaust flame
          if(fuel>0)
          {
            --fuel;
            flashFlame = !flashFlame;
          }
          else
          {
            Timer2Period = 0;
            flashFlame = 0;
          }
          showFuel=1;
          break;          
        case EV_TIMER_3: // Timer 3 controls movement of lander
          if(Timer2Period) // firing rocket          
          {
            if(boostDir == 0 || boostDir == 1 || boostDir == 2)
              velocityX+=0.003;
            else if(boostDir == 4 || boostDir == 5 || boostDir == 6)
              velocityX-=0.003;

            if(boostDir == 0 || boostDir == 6)
              velocityY+=0.003;
            else if(boostDir == 2 || boostDir == 4)
              velocityY-=0.003;
            else if(boostDir == 3)
              velocityY-=0.005;

          }

          if(velocityY > 0)
          {
            unsigned long p = 1000-2000*velocityY;
            if(p < 10) p = 10;
            Timer4Period = p;
          }
          else
          {
            Timer4Period = 0;
            flashVertical = 0;            
          }
          
          XX = abs(velocityX);
          if(XX > 0)
          {
            unsigned long p = 1000-2000*XX;
            if(p < 10)
              p = 10;
            Timer5Period = p;
          }
          else
          {
            Timer5Period = 0;
            flashHorizontal = 0;
          }
          landerX += velocityX;          
          landerY += velocityY;          
          velocityY += 0.0005;          
          xx = landerX-ox;
          yy = landerY-oy;
          if(xx<3) 
            ox--;
          else if(xx>4) ox++;
          if(yy<3) 
            oy--;
          else if(yy>4) oy++;
          break;          
        case EV_TIMER_4: // Timer 4 controls vertical velocity indicator
           flashVertical = !flashVertical;                    
           break;
        case EV_TIMER_5: // Timer 5 controls horizontal velocity indicator
           flashHorizontal = !flashHorizontal;                    
           break;
        }
        
        setGrid(ox, oy,flashBases);
        xx = landerX-ox;
        yy = landerY-oy;
        byte flame = flashFlame? DISP_YELLOW : DISP_GREEN ;
        // boost
        // 0.6
        // 1X5
        // 234
        Disp8x8.set(xx, yy, DISP_RED);
        switch(boostDir)
        {
          case 0: Disp8x8.set(xx-1, yy-1, flame); break;
          case 1: Disp8x8.set(xx-1, yy, flame); break;
          case 2: Disp8x8.set(xx-1, yy+1, flame); break;
          case 3: Disp8x8.set(xx, yy+1, flame); break;
          case 4: Disp8x8.set(xx+1, yy+1, flame); break;
          case 5: Disp8x8.set(xx+1, yy, flame); break;
          case 6: Disp8x8.set(xx+1, yy-1, flame); break;
        }
        Disp8x8.set(0, 0, flashHorizontal && velocityX<0? DISP_RED:DISP_GREEN);
        Disp8x8.set(2, 0, flashHorizontal && velocityX>0? DISP_RED:DISP_GREEN);
        Disp8x8.set(1, 0, flashVertical? DISP_RED:DISP_GREEN);
        
        if(showFuel)
        {
          
            if(fuel == 0)
              Disp8x8.set(7,0,DISP_RED);
            else if (fuel < 20)
              Disp8x8.set(7,0,DISP_YELLOW);
            else 
              Disp8x8.set(7,0,DISP_GREEN);

            if(fuel < 100)
              Disp8x8.set(6,0,DISP_OFF);
            else if (fuel < 200)
              Disp8x8.set(6,0,DISP_YELLOW);
            else 
              Disp8x8.set(6,0,DISP_GREEN);

            if(fuel < 400)
              Disp8x8.set(5,0,DISP_OFF);
            else if (fuel < 500)
              Disp8x8.set(5,0,DISP_YELLOW);
            else 
              Disp8x8.set(5,0,DISP_GREEN);

            if(fuel < 700)
              Disp8x8.set(4,0,DISP_OFF);
            else if (fuel < 800)
              Disp8x8.set(4,0,DISP_YELLOW);
            else 
              Disp8x8.set(4,0,DISP_GREEN);            
        }
        
    }
};

