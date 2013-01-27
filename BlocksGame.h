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
  byte gridMap[16];
  byte fallingMap[16];
  byte shadowMap[16];
  byte pieceMap[4];
  byte flashShadow;
  byte scrollTop;
  byte shadowMapTopRow;
  int fallRow;
  
    void init()
    {
      shadowMapTopRow = 0;
      scrollTop = 8;
      flashShadow = 0;
      placeCol = 1;
      placePiece = PIECE_T;
      placeDir = DIR_0;
      memset(gridMap,0,sizeof gridMap);
      memset(shadowMap,0,sizeof shadowMap);
      memset(fallingMap,0,sizeof fallingMap);
      Disp8x8.cls();
      newPiece();
    }
    
    void newPiece()
    {
      placePiece = random(PIECE_MAX);
      placeCol = random(6);
      placeDir = random(4);
      movePiece(0,0);
      fallRow = 0;
      for(int i=3;i>0;--i)
      {
        if(pieceMap[i])
        {
          fallRow = -i;
          break;
        }
      }
      setShadowMap();
      setFallingMap();
      Timer1Period = 150; // flash shadow map
      Timer2Period = 500; // falling piece
    }
    
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
          
        // see if we're constrained by pieces already on board
        if(fallRow < 16 && (gridMap[fallRow] & pm[0])) continue;
        if(fallRow < 15 && (gridMap[fallRow+1] & pm[1])) continue;
        if(fallRow < 14 && (gridMap[fallRow+2] & pm[2])) continue;
        if(fallRow < 13 && (gridMap[fallRow+3] & pm[3])) continue;
                
        // we're done
        memcpy(pieceMap, pm, 4);
        placeDir = newDir;
        placeCol = newCol;
        return 1;
      }        
      return 0;
    }

    byte fallPiece()
    {
        if(fallRow < 15 && (gridMap[fallRow+1] & pieceMap[0])) return 0;
        if(fallRow < 14 && (gridMap[fallRow+2] & pieceMap[1])) return 0;
        if(fallRow < 13 && (gridMap[fallRow+3] & pieceMap[2])) return 0;
        if(fallRow < 12 && (gridMap[fallRow+4] & pieceMap[3])) return 0;
        fallRow++;
        setFallingMap();
        return 1;
    }    
        
    void setShadowMap()
    {
      int row, i;
      
      
      // scan down looking for first collision
      // betweem the piecemap and the gridmap
      for(row=-3;row<16;++row)
      {
        // piecemap is 4 rows high
        for(i=0;i<4;++i)
        {
          // calc gridmap row corresponding
          // to the piecemap row and ensure
          // it is valid on grid
          int r = row + i;
          if(r >= 0 && r < 16)        
          {
            if(pieceMap[i] && (r == 15)) // resting on bottom row
              goto JUST_GET_OUT_OF_ALL_THESE_LOOPS_ALREADY;
            if(r<15 && !!(pieceMap[i] & gridMap[r+1])) // bitwise collision
              goto JUST_GET_OUT_OF_ALL_THESE_LOOPS_ALREADY;
          }
        }        
      }

JUST_GET_OUT_OF_ALL_THESE_LOOPS_ALREADY: //imho a reasonable use of a goto :)

      // now store the offset data as shadow map
      shadowMapTopRow = 16;
      memset(shadowMap,0,sizeof shadowMap);
      for(i=0;i<4;++i)
      {
        int r = row + i;
        if(r >= 0 && r < 16)        
        {
          if(pieceMap[i] && shadowMapTopRow > r)
            shadowMapTopRow = r;
          shadowMap[r] = pieceMap[i];
        }
      }
    } 

    void setFallingMap() 
    {      
      // set up the falling map
      memset(fallingMap,0,sizeof fallingMap);
      for(int i=0;i<4;++i)
      {
        int r = fallRow + i;
        if(r >= 0 && r < 16)        
          fallingMap[r] = pieceMap[i];
      }
    }

    void lockPieceInPlace()
    {
      int i,j;
      

      for(int i=0;i<16;++i)
        gridMap[i] |= shadowMap[i];
       
      // look for full rows we can remove
      byte again = 1;
      while(again)
      { 
        again=0;
        for(i=16;i>0;--i)
        {
          if(gridMap[i] == 0x7e)
          {
            byte dispRow = i - scrollTop;
            if(dispRow >= 0 && dispRow < 8)
            {
              Disp8x8.red[dispRow] = 0xff;
              Disp8x8.green[dispRow] = 0x7e;
              Disp8x8.delayWithRefresh(100);
              Disp8x8.red[dispRow] = 0x81;
            }
            for(j=i;j>0;--j)
              gridMap[j] = gridMap[j-1];              
            gridMap[0] = 0;
            memcpy(Disp8x8.green, gridMap + scrollTop,8);
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
          movePiece(0,-1);
          setShadowMap();
          setFallingMap();
          break;
        case EV_PRESS_D://right
          movePiece(0,+1);
          setShadowMap();
          setFallingMap();
          break;
        case EV_PRESS_A:
          movePiece(1,0);//rotate
          setShadowMap();
          setFallingMap();
          break;
        case EV_PRESS_C:
          lockPieceInPlace();
          break;
        case EV_TIMER_1: //FLASH SHADOW MAP
          flashShadow = !flashShadow;
          break;
        case EV_TIMER_2: //TIME FALLING
          if(!fallPiece())
            lockPieceInPlace();
          break;
      }
      
      scrollTop = min(8,max(0,(int)shadowMapTopRow-2));
      for(int i=0;i<8;++i)
      {  
        Disp8x8.green[i] = gridMap[scrollTop+i];
        Disp8x8.red[i] = 0x81|shadowMap[scrollTop+i]|fallingMap[scrollTop+i];
        if(flashShadow)
          Disp8x8.green[i] |= shadowMap[scrollTop+i];
      }   
      
      // scroll bar
      Disp8x8.green[scrollTop-1]|=0x01;
    }        
};


