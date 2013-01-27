////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   B L O C K S     G A M E 
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBlocksGame : public CGame 
{
  
  int count;
  public:
    static void getGameIcon(byte *dst, byte count)
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

  enum {
    PIECE_I,
    PIECE_O,
    PIECE_S,
    PIECE_Z,
    PIECE_L,
    PIECE_J,
    PIECE_T,
    PIECE_MAX
  };
  enum {
     DIR_0,
     DIR_90,
     DIR_180,
     DIR_270
  };
  void mapPiece(byte piece, byte dir, byte *result)
  {
    switch(piece)
    {
      // I PIECE
      case PIECE_I:
        switch(dir) {
          case DIR_0: case DIR_180:   result[1] = 0b11110000; break;
          case DIR_90: case DIR_270:  result[0] = result[1] = result[2] = result[3] = 0b01000000; break;
        } break;        
        
      // O PIECE
      case PIECE_O:
        result[0] = result[1] = 0b11000000; break;
        break;        
        
      // S PIECE
      case PIECE_S:
        switch(dir) {
          case DIR_0: result[0] =   0b01100000; result[1] = 0b11000000; break;
          case DIR_90: result[0] =  0b01000000; result[1] = 0b01100000;  result[2] = 0b00100000; break;
          case DIR_180: result[1] = 0b01100000; result[2] = 0b11000000; break;
          case DIR_270: result[0] = 0b10000000; result[1] = 0b11000000;  result[2] = 0b01000000; break;
        } break;        
        
      // Z PIECE
      case PIECE_Z:
        switch(dir) {
          case DIR_0: result[0] =   0b11000000; result[1] = 0b01100000; break;
          case DIR_90: result[0] =  0b00100000; result[1] = 0b01100000;  result[2] = 0b01000000; break;
          case DIR_180: result[1] = 0b11000000; result[2] = 0b01100000; break;
          case DIR_270: result[0] = 0b01000000; result[1] = 0b11000000;  result[2] = 0b10000000; break;
        } break;        
        
        
      // L PIECE
      case PIECE_L:
        switch(dir) {
          case DIR_0: result[0] = 0b00100000; result[1] = 0b11100000; break;
          case DIR_90: result[0] = result[1] = 0b01000000; result[2] = 0b01100000; break;
          case DIR_180: result[1] = 0b11100000; result[2] = 0b10000000; break;
          case DIR_270: result[0] = 0b11000000; result[1] = result[2] = 0b01000000; break;
        } break;        
        
      // J PIECE
      case PIECE_J:
        switch(dir) {
          case DIR_0: result[0] =   0b10000000; result[1] = 0b11100000; break;
          case DIR_90: result[0] =  0b01100000; result[1] = result[2] = 0b01000000; break;
          case DIR_180: result[1] = 0b11100000; result[2] = 0b00100000; break;
          case DIR_270: result[0] = result[1] = 0b01000000; result[2] = 0b11000000; break;
        } break;        
              
      // T PIECE
      case PIECE_T:
        switch(dir) {
          case DIR_0: result[0] = 0b10000000; result[1] = 0b11000000; result[2] = 0b10000000; break;
          case DIR_90: result[0] = 0b11100000; result[1] = 0b01000000; break;
          case DIR_180: result[0] = 0b00100000; result[1] = 0b01100000; result[2] = 0b00100000; break;
          case DIR_270: result[1] = 0b01000000; result[2] = 0b11100000; break;
        } break;        
    }
  }
  

  byte placePiece;
  byte placeDir;
  byte placeCol;
  byte placeRow;
  byte gridMap[8];
  byte shadowMap[8];
//  byte pieceMap[4];
  byte flashShadow;
  byte isDropped;
  
    void init()
    {
      isDropped = 0;
      flashShadow = 0;
      placeCol = 1;
      placePiece = PIECE_T;
      placeDir = DIR_0;
      memset(gridMap,0,sizeof gridMap);
      memset(shadowMap,0,sizeof shadowMap);
      Disp8x8.cls();
      newPiece();
    }
    
    void newPiece()
    {
      placePiece = random(PIECE_MAX);
      placeCol = random(6);
      placeDir = random(4);
      movePiece(0,0);
      Timer1Period = 150; // flash shadow map
      isDropped = 0;
    }
    
    byte movePiece(char dr, char dx)
    {
      byte pm[4];
      int row, i;
      char newDir = placeDir + dr;
      if(newDir < DIR_0) 
        newDir = DIR_270;
      else if(newDir > DIR_270) 
        newDir = DIR_0;
      char newCol = constrain(placeCol + dx,0,7);
      for(byte attempt = 0; attempt < 4; ++attempt)
      {
        
        memset(pm, 0, 4);
        mapPiece(placePiece, newDir, pm);
        pm[0] >>= newCol;
        pm[1] >>= newCol;
        pm[2] >>= newCol;
        pm[3] >>= newCol;
        
        // outside play area on left        
        if((pm[0] & 0x80)||(pm[1] & 0x80)||(pm[2] & 0x80)||(pm[3] & 0x80))
        {
          newCol++; // otherwise must move to right
          continue;
        }
        // outside play are to right?
        else if((pm[0] & 0x01)||(pm[1] & 0x01)||(pm[2] & 0x01)||(pm[3] & 0x01))
        {
          newCol--; // otherwise must move to left
          continue;
        }
        else
        {
          break;
        }        
      }
      
      placeDir = newDir;
      placeCol = newCol;

      // scan down looking for first collision
      // betweem the piecemap and the gridmap
      for(row=-3;row<8;++row)
      {
        // piecemap is 4 rows high
        for(i=0;i<4;++i)
        {
          // calc gridmap row corresponding
          // to the piecemap row and ensure
          // it is valid on grid
          int r = row + i;
          if(r >= 0 && r < 8)        
          {
            if(pm[i] && (r == 7)) // resting on bottom row
              goto JUST_GET_OUT_OF_ALL_THESE_LOOPS_ALREADY;
            if(r<7 && !!(pm[i] & gridMap[r+1])) // bitwise collision
              goto JUST_GET_OUT_OF_ALL_THESE_LOOPS_ALREADY;
          }
        }        
      }

JUST_GET_OUT_OF_ALL_THESE_LOOPS_ALREADY: //imho a reasonable use of a goto :)

      // now store the offset data as shadow map
      placeRow = row;
      memset(shadowMap,0,sizeof shadowMap);
      for(i=0;i<4;++i)
      {
        int r = row + i;
        if(r >= 0 && r < 8)        
          shadowMap[r] = pm[i];
      }
    }         


    byte movePieceDropped(char dr, char dx)
    {
      byte pm[4];
      int row, i;
      char newCol = constrain(placeCol + dx,0,7);
      char newDir = placeDir;
      for(byte attempt = 0; attempt < 4; ++attempt)
      {
        newDir += dr;
        if(newDir < DIR_0) 
          newDir = DIR_270;
        else if(newDir > DIR_270) 
          newDir = DIR_0;
          
        memset(pm, 0, 4);
        mapPiece(placePiece, newDir, pm);
        pm[0] >>= newCol;
        pm[1] >>= newCol;
        pm[2] >>= newCol;
        pm[3] >>= newCol;
        
        // outside play area on left        
        if((pm[0] & 0x80)||(pm[1] & 0x80)||(pm[2] & 0x80)||(pm[3] & 0x80))
          continue;
        // outside play are to right?
        else if((pm[0] & 0x01)||(pm[1] & 0x01)||(pm[2] & 0x01)||(pm[3] & 0x01))
          continue;
        // outside play area to bottom
        else if((placeRow>6)||(placeRow>5 && pm[2])||(placeRow>4 && pm[3])||(placeRow>3 && pm[4]))
          continue;
        // collision
        else if((pm[0]&gridMap[placeRow])||(pm[1]&gridMap[placeRow+1])||(pm[2]&gridMap[placeRow+2])||(pm[3]&gridMap[placeRow+3]))
          continue;
        else
        {
          placeDir = newDir;
          placeCol = newCol;
          memset(shadowMap,0,sizeof shadowMap);
          for(i=0;i<4;++i)
          {
            int r = placeRow + i;
            if(r >= 0 && r < 8)        
              shadowMap[r] = pm[i];
          }
          return 1;
        }
      }      
      return 0;
    }         

/*
    byte movePiece(char dr, char dx)
    {
      char newDir = placeDir + dr;
      if(newDir < DIR_0) 
        newDir = DIR_270;
      else if(newDir > DIR_270) 
        newDir = DIR_0;
      char newCol = constrain(placeCol + dx,0,7);
      for(byte attempt = 0; attempt < 4; ++attempt)
      {
        
        byte pm[4];
        memset(pm, 0, 4);
        mapPiece(placePiece, newDir, pm);
        pm[0] >>= newCol;
        pm[1] >>= newCol;
        pm[2] >>= newCol;
        pm[3] >>= newCol;
        
        // outside play area on left        
        if((pm[0] & 0x80)||(pm[1] & 0x80)||(pm[2] & 0x80)||(pm[3] & 0x80))
        {
          newCol++; // otherwise must move to right
          continue;
        }
        // outside play are to right?
        else if((pm[0] & 0x01)||(pm[1] & 0x01)||(pm[2] & 0x01)||(pm[3] & 0x01))
        {
          newCol--; // otherwise must move to left
          continue;
        }
          
        // we're done
        memcpy(pieceMap, pm, 4);
        placeDir = newDir;
        placeCol = newCol;
        return 1;
      }        
      return 0;
    }

    void setShadowMap()
    {
      int row, i;
      
      
      // scan down looking for first collision
      // betweem the piecemap and the gridmap
      for(row=-3;row<8;++row)
      {
        // piecemap is 4 rows high
        for(i=0;i<4;++i)
        {
          // calc gridmap row corresponding
          // to the piecemap row and ensure
          // it is valid on grid
          int r = row + i;
          if(r >= 0 && r < 8)        
          {
            if(pieceMap[i] && (r == 7)) // resting on bottom row
              goto JUST_GET_OUT_OF_ALL_THESE_LOOPS_ALREADY;
            if(r<7 && !!(pieceMap[i] & gridMap[r+1])) // bitwise collision
              goto JUST_GET_OUT_OF_ALL_THESE_LOOPS_ALREADY;
          }
        }        
      }

JUST_GET_OUT_OF_ALL_THESE_LOOPS_ALREADY: //imho a reasonable use of a goto :)

      // now store the offset data as shadow map
      shadowMapTopRow = row;
      memset(shadowMap,0,sizeof shadowMap);
      for(i=0;i<4;++i)
      {
        int r = row + i;
        if(r >= 0 && r < 8)        
          shadowMap[r] = pieceMap[i];
      }
    } 
*/    
    void lockPieceInPlace()
    {
      int i,j;     
      for(int i=0;i<8;++i)
        gridMap[i] |= shadowMap[i];
       
      // look for full rows we can remove
      byte again = 1;
      while(again)
      { 
        again=0;
        for(i=8;i>0;--i)
        {
          if(gridMap[i] == 0x7e)
          {
            Disp8x8.red[i] = 0xff;
            Disp8x8.green[i] = 0x7e;
            Disp8x8.delayWithRefresh(100);
            Disp8x8.red[i] = 0x81;
            for(j=i;j>0;--j)
              gridMap[j] = gridMap[j-1];              
            gridMap[0] = 0;
            memcpy(Disp8x8.green, gridMap,8);
            again = 1;
            break;
          }
        }
      }
                  
      // put another piece into play
      newPiece();
    }
        
    void handleEvent(char event)
    {
      int i;
      switch(event)
      {
        case EV_PRESS_B://left
          isDropped? movePieceDropped(0,-1) : movePiece(0,-1);
          break;
        case EV_PRESS_D://right
          isDropped? movePieceDropped(0,+1) : movePiece(0,+1);
          break;
        case EV_PRESS_A:
          isDropped? movePieceDropped(1,0) : movePiece(1,0);//rotate
          break;
        case EV_PRESS_C:
          isDropped = 1;
          break;
        case EV_RELEASE_C:
          lockPieceInPlace();
          break;
        case EV_TIMER_1: //FLASH SHADOW MAP
          flashShadow = !flashShadow;
          break;
      }
      
      for(int i=0;i<8;++i)
      {  
        Disp8x8.green[i] = gridMap[i];
        Disp8x8.red[i] = 0x81|shadowMap[i];
        if(!isDropped && flashShadow)
          Disp8x8.green[i] |= shadowMap[i];
      }         
    }        
};


