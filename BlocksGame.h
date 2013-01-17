////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   P O N G     G A M E 
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBlocksGame : public CGame 
{
  byte grid[8];
  
  int count;
  public:
    static void getGameIcon(byte *dst)
    {
      dst[ 0] = 0b11110000;          
      dst[ 1] = 0b11110110;          
      dst[ 2] = 0b11000110;          
      dst[ 3] = 0b11000110;          
      dst[ 4] = 0b00000110;          
      dst[ 5] = 0b00000110;          
      dst[ 6] = 0b00000110;          
      dst[ 7] = 0b00000000;          
      
      dst[ 8] = 0b11110000;          
      dst[ 9] = 0b11110000;          
      dst[10] = 0b11000000;          
      dst[11] = 0b11011000;          
      dst[12] = 0b00011000;          
      dst[13] = 0b01111000;          
      dst[14] = 0b01111000;          
      dst[15] = 0b00000000;          
      
    }    

    /*
        1111
        
        2     22  222    2
        222   2     2    2
              2         22
        
          3
        333
        
        44
        44
        
         55
        55
        
         6
        666
        
        77
         77
         
         
         0
        3 1
         2
    */

//J
  0b100111000
  0b011010010
  0b000111001
  0b010010110
//L
  0b001111000  
  0b010010011  
  0b000111100  
  0b110010010
  
//O - special
  
//S
 0b011110000 
 0b010011001 
 0b000011110 
 0b100110010
 
//T
 0b010111000 
 0b010011010 
 0b000111010
 0b010110010
 
//Z
 0b110011000
 0b001011010
 0b000110011 
 0b010110100
  
  

  #define HASH(piece, dir) (((piece)*8)+dir)
  #define SET_SHAPE(a,b,c,d,x0,xx,y0,yy) {info[0]=a;info[1]=b;info[2]=c;info[3]=d;info[5]=x0;info[6]=xx;info[7]=y0;info[8]=yy;}
  void getPieceInfo(byte piece, byte dir, byte *info)
  {
    switch(HASH(piece, dir))
    {
      /*
            0X00  0
                  X 
                  0
                  0
      */
      case HASH(0,0): HASH(0,2): SET_SHAPE(0,1,2,3,0,4,0,7}; break;
      case HASH(0,1): HASH(0,3): SET_SHAPE(0,8,16,24,0,7,0,4}; break;
  
      /*
            1    11  1X1   1
            1X1  X     1   X
                 1        11
      */
      case HASH(1,0): SET_SHAPE(0,8,9,10}; break;
      case HASH(1,1): SET_SHAPE(0,1,8,16}; break;
      case HASH(1,2): SET_SHAPE(0,1,2,10}; break;
      case HASH(1,3): SET_SHAPE(1,9,16,17}; break;
        
      /*
            2  2   2X2  22
          2X2  X   2     X
               22        2
      */             
      case HASH(2,0): SET_SHAPE(2,8,9,10}; break;
      case HASH(2,1): SET_SHAPE(0,8,16,17}; break;
      case HASH(2,2): SET_SHAPE(0,1,2,8}; break;
      case HASH(2,3): SET_SHAPE(0,1,9,17}; break;
          
      /*
          33  
          33
      */             
      case HASH(3,0): case HASH(3,1): case HASH(3,2): case HASH(3,3): SET_SHAPE(0,1,8,9}; break;
      
      /*
           44  4
          44   X4
                4
      */             
      case HASH(4,0): HASH(4,2): SET_SHAPE(1,2,8,9}; break;
      case HASH(4,1): HASH(4,3): SET_SHAPE(1,8,9,17}; break;
      
  
      /*
          5   5   5X5   5
         5X5  X5   5   5X
              5         5
      */             
      case HASH(5,0): SET_SHAPE(1,8,9,10}; break;
      case HASH(5,1): SET_SHAPE(1,8,9,16}; break;
      case HASH(5,2): SET_SHAPE(0,1,2,9}; break;
      case HASH(5,3): SET_SHAPE(1,8,9,17}; break;
  
      /*
          66     6
           66   6X
                6
      */             
      case HASH(6,0): HASH(6,2): SET_SHAPE(0,1,9,10}; break;
      case HASH(6,1): HASH(6,3): SET_SHAPE(1,8,9,16}; break;
    }
  }


    void init()
    {
    }
    void handleEvent(char event)
    {
      switch(event)
      {
        case EV_TIMER_1:1;
          break;
      }
    }
};


