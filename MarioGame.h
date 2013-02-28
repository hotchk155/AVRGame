

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   M A R I O     G A M E
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define LOG_SERIAL 1
struct note_st
{
  unsigned short len:3;
  unsigned short note:13;
};
struct note_st MusicScore[] = {
{1,NOTE_E4},{1,NOTE_E4},{1,0},{1,NOTE_E4},  // Measure 1
{1,0},{1,NOTE_C4},{1,NOTE_E4},{1,0},        // Measure 2
{1,NOTE_G4},{3,0}, // Measure 3
{1,NOTE_G3},{3,0}, // Measure 4
{1,NOTE_C4},{2,0},{1,NOTE_G3}, // Measure 5
{2,0},{1,NOTE_E3},{1,0}, // Measure 6
{1,0},{1,NOTE_A4},{1,0},{1,NOTE_B4}, // Measure 7
{1,0},{1,NOTE_AS4},{1,NOTE_A4},{1,0}, // Measure 8
{2,NOTE_G3},{2,NOTE_E4},{2,NOTE_G4}, // Measure 9
{1,NOTE_A5},{1,0},{1,NOTE_F4},{1,NOTE_G4}, // Measure 10
{1,0},{1,NOTE_E4},{1,0},{1,NOTE_C4}, // Measure 11
{1,NOTE_D4},{1,NOTE_B4},{2,0},    // Measure 12
{1,NOTE_C4},{2,0},{1,NOTE_G3},    // Measure 13
{2,0},{1,NOTE_E3},{1,0},    // Measure 14
{1,0},{1,NOTE_A4},{1,0},{1,NOTE_B4},  // Measure 15
{1,0},{1,NOTE_AS4},{1,NOTE_A4},{1,0},  // Measure 16
{2,NOTE_G3},{2,NOTE_E4},{2,NOTE_G4}, // Measure 9/17
{1,NOTE_A5},{1,0},{1,NOTE_F4},{1,NOTE_G4}, // Measure 10/18
{1,0},{1,NOTE_E4},{1,0},{1,NOTE_C4}, // Measure 11/19
{1,NOTE_D4},{1,NOTE_B4},{2,0},    // Measure 12/20
{2,0},{1,NOTE_G4},{1,NOTE_FS4},   // Measure 21
{1,NOTE_F4},{1,NOTE_DS4},{1,0},{1,NOTE_E4},  // Measure 22
{1,0},{1,NOTE_GS3},{1,NOTE_A4},{1,NOTE_C4},  // Measure 23
{1,0},{1,NOTE_A4},{1,NOTE_C4},{1,NOTE_D4},  // Measure 24
{2,0},{1,NOTE_G4},{1,NOTE_FS4},     // Measure 25
{1,NOTE_F4},{1,NOTE_DS4},{1,0},{1,NOTE_E4},  // Measure 26
{1,0},{1,NOTE_C6},{1,0},{1,NOTE_C6},  // Measure 27
{1,NOTE_C6},{3,0},              // Measure 28
{2,0},{1,NOTE_G4},{1,NOTE_FS4},  // Measure 29
{1,NOTE_F4},{1,NOTE_DS4},{1,0},{1,NOTE_E4}, // Measure 30
{1,0},{1,NOTE_GS3},{1,NOTE_A4},{1,NOTE_C4}, // Measure 31
{1,0},{1,NOTE_A4},{1,NOTE_C4},{1,NOTE_D4}, // Measure 32
{2,0},{1,NOTE_DS4},{1,0},  // Measure 33
{1,0},{1,NOTE_D4},{2,0},  // Measure 34
{1,NOTE_C4},{3,0},   // Measure 35
{4,0},                 // Measure 36
};
#define MUSIC_LEN (sizeof(MusicScore)/sizeof(note_st))
#define MUSIC_TEMPO(bpm) ((60000L / bpm))
#define MUSIC_NOTE_SEPARATION 5

// D=dirt/solid block
// ?=
// B=bad guy
// M=mushroom
// P=pipe
// b=balloon
// S=star
// T=turtle
// F=flag pole
// C=castle
//                          1         2         3         4         5         6         7         8         9        10        11        12        13        14        15        16        17        18        19        20        21        22 
//                 12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901
byte Row[][199]={//{"                                                                                                                                                                                                         "},
                 //{"                                                        M                                                                                                                            DD    F          "},
                 {"                                                        D              B                            M                                                                               DDD    F          "},
                 {"                                          PP      PP                 DDDDDD   DDD?                  ?           DDD      D??D      D  D          DD  D                             DDDD    F    CCC   "},
                 {"                 M                 PP     P       P                M             b         S                                      DD  DD        DDD  DD           DD?D            DDDDD    F    CCC   "},
                 {"         ?      D?D?D     PP       P      P       P               D?D            D        DD     ?  ?  ?    D             DD     DDD  DDD      DDDD  DDD     PP               PP DDDDDD    F   CC CC  "},
                 {"                 B        P        PB     P   BB  P                               B B                             B B  B B      DDDD  DDDD    DDDDD  DDDD    P          B B   P DDDDDDD    D   CC CC  "},
                 {"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD  DDDDDDDDDDD   DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD  DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"}};
//byte Row[][24]= {{0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
//                 {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000},
//                 {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b01100000,0b10000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001100,0b00000100,0b00000000,0b00000000},
//                 {0b00000000,0b00000000,0b10000000,0b00000000,0b00011000,0b01000000,0b00000000,0b00000000,0b00011111,0b11111100,0b00011110,0b00000000,0b00010000,0b00000111,0b00011110,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00011100,0b00000100,0b00000000,0b00000000},
//                 {0b00000000,0b00000000,0b00000000,0b00000110,0b00010000,0b01000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000010,0b01000000,0b01100100,0b00000000,0b00000000,0b00111100,0b00000100,0b00011100,0b00000000},
//                 {0b00000000,0b00100011,0b11000001,0b10000100,0b00010000,0b01000000,0b00000000,0b00001110,0b00000000,0b00000000,0b00000010,0b00001100,0b01010100,0b00100000,0b00001100,0b00000110,0b01100000,0b11001100,0b00001110,0b00000000,0b01111100,0b00000100,0b00111110,0b00000000},
//                 {0b00000000,0b00000000,0b00000001,0b00000100,0b00010000,0b01000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00001110,0b01110001,0b11001110,0b00000000,0b00000000,0b11111100,0b00000100,0b00110110,0b00000000},
//                 {0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111100,0b11111111,0b00111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11001111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111,0b11111111}};


class CMarioGame : public CGame
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
  // Music
  int note_idx;
  int hold;
  int backgroundOffset;
  int blinkON;
  public:
    static void getGameIcon(byte *dst, byte count)
    {
      static int MarioPos, MarioDir;
      
//      if(count & 0x3f)//sinister blinking
//        dst[ 2] = 0b00100100;  
#ifdef UNUSED
      if((count%1) == 0)
      {
         if (MarioDir==0)
            MarioDir=1;
         MarioPos += MarioDir;
         if (MarioPos == 21)
         {
            MarioDir=-1;
         }
         else if (MarioPos == 0)
         {
            MarioDir=1;
         }
         if ((MarioPos >= 0) && (MarioPos <=9))
             dst[ 6] = 0b00100000;
         else if (MarioPos == 10)
             dst[ 5] = 0b00100000;
         else if (MarioPos == 11)
             dst[ 4] = 0b00010000;
         else if ((MarioPos >= 12) && (MarioPos <= 21))
             dst[ 4] = 0b00001000;
      }
#endif
      dst[ 6] = 0b01000000;

      dst[ 9] = 0b00000000;
      dst[10] = 0b00011111;
      dst[11] = 0b00000000;
      dst[12] = 0b11000000;
      dst[13] = 0b00000000;
      dst[14] = 0b00001111;
      dst[15] = 0b11001111;
    }
   
    void init()
    {
#ifdef LOG_SERIAL
  Serial.begin(9600);
#endif
      invaderPeriod = 600;
      playerX = 4;
      newScreen();
      note_idx=0;
      hold=0;
      backgroundOffset=0;
      blinkON=1;
    }

    void newScreen()
    {
      playerCaught = 0;
      mothershipX = -1;
      bulletY = -1;
//      Disp8x8.cls();
//      Disp8x8.green[5] = 0b01011010;
//      Disp8x8.green[6] = 0b01011010;
//      Disp8x8.set(playerX,7,DISP_RED);
      memset(invaders,1,sizeof invaders);
      invadersX = 1;
      invadersY = 1;
      invadersDir = 1;
      toggle = 1;
      Timer1Period = 200;  // Flash good/bad guys
//      Timer2Period = 1000; // Auto-pan the background
      Timer3Period = 400;
      Timer4Period = 400;
      Timer5Period = MUSIC_TEMPO(660);    // Music tempo
      memset(bombX,-1,sizeof bombX);
      invaderPeriod -= 50;
    }

// D=dirt/solid block
// ?=
// B=bad guy
// M=mushroom
// P=pipe
// b=balloon
// S=star
// T=turtle
// F=flag pole
// C=castle
    void drawBackground()
    {
        byte thisMapItem;
        int row, col, color;
        
        for (row = 0; row <= 5; row++)
        {
            for (col = 0; col <= 7; col++)
            {
                thisMapItem=Row[row][col+backgroundOffset];
                switch (thisMapItem)
                {
                   case 'D': color=DISP_YELLOW; break;
                   case 'P': color=DISP_GREEN; break;
                   case 'b': color=DISP_GREEN; break;
                   case 'B': if (blinkON) color=DISP_RED; else color=DISP_OFF; break;
                   case '?': if (blinkON) color=DISP_YELLOW; else color=DISP_OFF; break;
                   case 'M': if (blinkON) color=DISP_GREEN; else color=DISP_OFF; break;
                   case 'S': if (blinkON) color=DISP_YELLOW; else color=DISP_GREEN; break;
                   case 'T': color=DISP_GREEN; break;
                   case 'F': color=DISP_RED; break;
                   case 'C': color=DISP_YELLOW; break;
                    default: color=DISP_OFF;
//                    default: color=DISP_GREEN;
                }
                Disp8x8.set(col,row+2,color);
            }
        }
    }
   
    void handleEvent(char event)
    {
      int i;
      switch(event)
      {
        case EV_PRESS_B:
          if (backgroundOffset)
              backgroundOffset--;
          break;

 //       case EV_TIMER_2:  // TIMER2 used to auto-pan the background
        case EV_PRESS_D:
          if (backgroundOffset<sizeof(Row[0]) - 9)
              backgroundOffset++;
          break;

        case EV_PRESS_A:
        case EV_PRESS_C:
          break;

           
        // TIMER1 USED FOR blinking good guys/bad guys
        case EV_TIMER_1:         
          blinkON = !blinkON;
          break;     
         
        // TIMER3 USED FOR ALIEN BOMBS
        case EV_TIMER_3:                 
            break;
           
        // TIMER4 runs mothership at top of screen
        case EV_TIMER_4:         
          break;

        // TIMER5 runs music
        case EV_TIMER_5:         
#ifdef LOG_SERIAL
            Serial.print("Beat ");
            Serial.print(note_idx);
            Serial.print("  ");
#endif           
            if (hold<=0)
            {
              struct note_st *pnote;
               if (++note_idx >= MUSIC_LEN)
                   note_idx=0;
               pnote=&MusicScore[note_idx];
               hold=pnote->len;
               if (pnote->note != 0)
               {
#ifdef LOG_SERIAL
//               Serial.print(PrintNote);
//               Serial.print("   ");
//               Serial.println(MakeNote(note,sharp_flat));
#endif
                  playSound(pnote->note,(hold*Timer5Period)-MUSIC_NOTE_SEPARATION);
               }
            }
            else
            {
#ifdef LOG_SERIAL
              Serial.print(hold);
              Serial.println("...playing/resting...");
#endif
               hold--;
            }
          break;
      }
      drawBackground();
//         endGame();
    }
};
CGameFactoryImpl<CMarioGame> MarioGame;


