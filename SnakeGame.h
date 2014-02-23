////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////
/////////
/////////   S N A K E     G A M E 
/////////   
/////////      By Stephen Cropp
/////////
/////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CSnakeGame : public CGame 
{
  int count;
  public:
    static void getGameIcon(byte *dst, byte count)
    {
      dst[ 0] = 0b00000000;          
      dst[ 1] = 0b01110000;          
      dst[ 2] = 0b01000000;          
      dst[ 3] = 0b01000000;          
      dst[ 4] = 0b01000000;          
      dst[ 5] = 0b01000000;          
      dst[ 6] = 0b00000000;          
      dst[ 7] = 0b00000000;          
      
      dst[ 8] = 0b00000000;          
      dst[ 9] = 0b00000000;          
      dst[10] = 0b00000000;          
      dst[11] = 0b00000000;          
      dst[12] = 0b00000000;          
      dst[13] = 0b00001000;          
      dst[14] = 0b00000000;          
      dst[15] = 0b00000000;          
      
     switch(count & 0b00001100)
      {
        case 0b00000000:
          dst[ 1] = 0b01110000;
          dst[ 9] = 0b00010010;
          break;
        case 0b00000100:
          dst[ 1] = 0b01111100;
          dst[ 9] = 0b00000110;  
          dst[ 4] = 0b00000000;          
          dst[ 5] = 0b00000000;     
           break;
        case 0b00001000:
          dst[ 1] = 0b01111110; 
          dst[ 2] = 0b00000010;
          dst[10] = 0b00000010;   
          dst[ 3] = 0b00000000;   
          dst[ 4] = 0b00000000;          
          dst[ 5] = 0b00000000; 
          break;
        case 0b00001100:
          dst[ 1] = 0b01111110;
          dst[ 2] = 0b00000010;
          dst[ 3] = 0b00000010;
          dst[ 4] = 0b00000010;
          dst[12] = 0b00000010;
          dst[ 5] = 0b00000000;   
          break;
      }
    }
    
    // Speed
    char speed;
    // snake 64 vectors
    char snakeX[64];
    char snakeY[64];
    // snake head 0-63
	char snakeHead;
	char snakeHeadOld;
    // snake tail 0-63
	char snakeTail;
	char snakeTailOld;
	// controls
    // A Rotate Left
    // B Rotate Right
    
    char grow;
    char rot;
    char hdg;
    char level;
    char treeCount;
    
    //score == gameScore
    
    // Grid disp8x8 snake red
    // Level disp8x8 plants green

	void plantTree()
	{
	  treeCount++;
	  if (treeCount == level+5) newLevel();
	  char planted = 0;
	  char X = 0;
	  char Y = 0;
	  while (!planted)
	  {
	    X = random(8);
	    Y = random(8);
	    if (Disp8x8.get(Y, X) != DISP_RED)
	   	{
	      Disp8x8.set(Y, X, DISP_GREEN);
	      planted = -1;
	   	}
        playSound(500,100);
	  }
	}
    
    void newLevel()
    {
      playSound(500,100);
      Disp8x8.delayWithRefresh(30);
      playSound(600,100);
      Disp8x8.delayWithRefresh(30);
      playSound(700,100);
      Disp8x8.delayWithRefresh(30);
      playSound(800,100);
      Disp8x8.delayWithRefresh(30);
      Disp8x8.cls();
	  Timer1Period -= 10;
      snakeTail = snakeHead > 1 ? snakeHead - 2 : snakeHead = 0 ? 62 : 63;
  	  Disp8x8.set(snakeY[snakeHead], snakeX[snakeHead], DISP_RED);
  	  Disp8x8.set(snakeY[snakeHead-1], snakeX[snakeHead-1], DISP_RED);
//  	  Disp8x8.set(snakeY[snakeHead-2], snakeX[snakeHead-2], DISP_RED);
  	  level++;
      treeCount = 0;
	  plantTree();
    }
    
    void init()
    {
      Timer1Period = 250;
      count=0;
      speed=200;
      hdg=0;
      rot=0;
      grow=0;
	  level=4;
      newLevel();
      Disp8x8.cls();
      
      for(byte i=0;i<64;i++)
      	{
      	snakeX[i]=0; snakeY[i]=0;
      	}
      snakeHead=1;
      snakeTail=0;
      snakeX[snakeHead]=2; snakeY[snakeHead]=4;
      snakeX[snakeTail]=0; snakeY[snakeTail]=4;
   	  plantTree();
    }

    void handleEvent(char event)
    {
      switch(event)
      {
        case EV_PRESS_B:
        	rot = 1;
        	break;
        case EV_PRESS_D:
			rot = -1;
			break;
        case EV_TIMER_1:
	  		// move tail
			if(grow)
			{
      		gameScore += 100;			
			grow--;
			}
			else
			{
	      		Disp8x8.set(snakeY[snakeTail], snakeX[snakeTail], DISP_OFF);
    	  		snakeTailOld = snakeTail;
      			snakeTail = snakeTail < 63 ? snakeTail + 1 : 0;
      		}
      		
			// move head
    	  	Disp8x8.set(snakeY[snakeHead], snakeX[snakeHead], DISP_RED);
    	    snakeHeadOld = snakeHead;
      		snakeHead = snakeHead < 63 ? snakeHead + 1 : 0;
      		if(rot)
      		{
				hdg += rot;
				if (hdg < 0) hdg = 3;
				if (hdg > 3) hdg = 0;
				rot = 0;
			}
			switch(hdg)
			{
			case 0:
	      		snakeX[snakeHead]=snakeX[snakeHeadOld]<7?snakeX[snakeHeadOld]+1:0;
   				snakeY[snakeHead]=snakeY[snakeHeadOld];
   				break;
   			case 1:
     			snakeX[snakeHead]=snakeX[snakeHeadOld];
      			snakeY[snakeHead]=snakeY[snakeHeadOld]<7?snakeY[snakeHeadOld]+1:0;
      			break;
      		case 2:
		   		snakeX[snakeHead]=snakeX[snakeHeadOld]>0?snakeX[snakeHeadOld]-1:7;
      			snakeY[snakeHead]=snakeY[snakeHeadOld];
      			break;
      		case 3:
		   		snakeX[snakeHead]=snakeX[snakeHeadOld];
      			snakeY[snakeHead]=snakeY[snakeHeadOld]>0?snakeY[snakeHeadOld]-1:7;
      			break;
			}
			//collision detect
			if(Disp8x8.get(snakeY[snakeHead], snakeX[snakeHead]) == DISP_RED)
			{
			 	Disp8x8.set(snakeY[snakeHead], snakeX[snakeHead], DISP_YELLOW);
    	        playSound(400,100);
                Disp8x8.delayWithRefresh(30);
    	        playSound(300,100);
                Disp8x8.delayWithRefresh(30);
    	        playSound(200,100);
                Disp8x8.delayWithRefresh(30);
    	        playSound(100,100);
                endGame();
			}
			else if(Disp8x8.get(snakeY[snakeHead], snakeX[snakeHead]) == DISP_GREEN)
			{
 			  Disp8x8.set(snakeY[snakeHead], snakeX[snakeHead], DISP_YELLOW);
			  grow = 1;
			  plantTree();
			}
			else
			{
         		Disp8x8.set(snakeY[snakeHead], snakeX[snakeHead], DISP_YELLOW);
        		gameScore += snakeHead - snakeTail;
			}
      		break;
      }
    }
};
CGameFactoryImpl<CSnakeGame> SnakeGame;