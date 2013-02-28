#ifndef __IRLINK_H__
#define __IRLINK_H__


#define EEPROM_MYCONVTAG 102

#define MAX_CONVTAG 200
#define IR_INVITE_PERIOD 500


#define IR_INVITE      0xFF
#define IR_ACK_INVITE  0xFE
#define IR_ACK_DATA    0xFD

#define IR_OFF         0
#define IR_LINKUP      1
#define IR_LINKED      2



/*

0xff - means start a new converatpon
[Conv][d1][d2][d3][d4]        


*/
enum {
      IRLINK_SEND = 1,
      IRLINK_RECEIVE = 2
  };

#define IR_DATA_SZ 8
#define IR_CHECKSUM_IDX (1 + IR_DATA_SZ)
#define IR_PACKET_SZ (IR_CHECKSUM_IDX + 1)

class CIRLinkClass 
{  
  
public:  
  byte state;
  byte enabled;
  byte mode;
  byte dataTag;
  byte dataPacket[IR_PACKET_SZ];
  byte dataIndex;
  byte myConvTag;
  char pollCount;
  byte count;
  unsigned long nextEvent;
  
  //////////////////////////////////////////////////////////////////
  // CTOR
  CIRLinkClass() {
    pollCount = 0;
    nextEvent = 0;
    mode = IRLINK_SEND|IRLINK_RECEIVE;
    dataIndex = 0;
    myConvTag = EEPROM.read(EEPROM_MYCONVTAG);
    state = IR_OFF;
    count = 0;
  }

/*
  //////////////////////////////////////////////////////////////////
  // INIT
  void init() {    
    Serial.begin(1200);
    Serial.write("TEST");
    delay(100);
    if(
      Serial.available() && Serial.read() == 'T' &&
      Serial.available() && Serial.read() == 'E' &&
      Serial.available() && Serial.read() == 'S' &&
      Serial.available() && Serial.read() == 'T') 
        enabled = 1;
  }
*/
  //////////////////////////////////////////////////////////////////
  // RUN IR STATE MACHINE
  byte linkup()
  {
    unsigned long milliseconds = millis();
    
//    if(!myConvTag || myConvTag > MAX_CONVTAG) 
//    {
//      myConvTag = 1 + random(MAX_CONVTAG);
//      EEPROM.write(EEPROM_MYCONVTAG, myConvTag);
//    }    
//    if(gotDataPacket())
    if(0)
    {
        // is another console trying to link up? 
        if(dataPacket[0] == IR_INVITE)
        {
          // is this a new conversation tag?
          if(dataPacket[1] != myConvTag) 
          {
            // ok store it
            myConvTag = dataPacket[1];
            EEPROM.write(EEPROM_MYCONVTAG, myConvTag);
          }
          
          // acknowledge
          sendCommand(IR_ACK_INVITE);
        }
        else if(dataPacket[0] == IR_ACK_INVITE && dataPacket[1] == myConvTag)
        {
            Disp8x8.cls();
            Disp8x8.green[0] = 0b01111100;
            Disp8x8.green[1] = 0b10000001;
            Disp8x8.green[2] = 0b10100101;
            Disp8x8.green[3] = 0b10000001;
            Disp8x8.green[4] = 0b10100101;
            Disp8x8.green[5] = 0b10111101;
            Disp8x8.green[6] = 0b10000001;  
            Disp8x8.green[7] = 0b01111110;    
            Disp8x8.delayWithRefresh(1000);
            Disp8x8.cls();
            state = IR_LINKED;
            return 1;
        }          
    }
    // TRY TO INSTIGATE A LINK UP
    else //if(milliseconds > nextEvent)
    {
//       Disp8x8.cls();
  //     if(!!(count&1))
       {
         Disp8x8.green[0] = 1;
         Disp8x8.green[1] = 2;
         Disp8x8.green[2] = 4;
         Disp8x8.green[3] = 8;
         Disp8x8.green[4] = 16;
         Disp8x8.green[5] = 32;
         Disp8x8.green[6] = 64;  
         Disp8x8.green[7] = 128;          
       }
       count++;
    //  sendCommand(IR_INVITE);
      nextEvent = milliseconds + IR_INVITE_PERIOD;
    }
    return 0;
  }  


  //////////////////////////////////////////////////////////////////
  // GOT DATA PACKET
  byte gotDataPacket()
  {
    if(!Serial.available() && pollCount)
    {
      if(!--pollCount)
        dataIndex = 0;
      return 0;
    }
    
    while(Serial.available())
    {
      pollCount = 2;            
      dataPacket[dataIndex] = Serial.read();
      if(++dataIndex >= IR_PACKET_SZ)
      {
        dataIndex = 0;
        return 1;        
      }
    }
    
    return 0;
  }

  //////////////////////////////////////////////////////////////////
  // CHECKSUM
  byte checkSum()
  {
    byte cs = 0;
    for(byte i=0;i<IR_CHECKSUM_IDX;++i)
      cs = (cs<<1)^dataPacket[i];
    return cs;
  }

  //////////////////////////////////////////////////////////////////
  // SEND COMMAND
  void sendCommand(char cmd)
  {
    Serial.write(cmd);
    Serial.write(myConvTag);
    
    delay(1);
    while(Serial.available())
      Serial.read();
  }
  
  //////////////////////////////////////////////////////////////////
  // SEND DATA
  void sendData()
  {
    if(!enabled || !(mode & IRLINK_SEND))
      return;
    Serial.write(myConvTag);
    for(byte i=0;i<IR_PACKET_SZ;++i)
      Serial.write(dataPacket[i]);
    Serial.write(checkSum());
    
    delay(1);
    while(Serial.available())
      Serial.read();
  }

/*  
  //////////////////////////////////////////////////////////////////
  // RUN IR STATE MACHINE
  int run(unsigned long ms)
  {
    // DISABLED
    if(state == IR_DISABLED)
    {
      // do nothing
    }
    // ANY DATA?
    else if(gotDataPacket())
    {
      switch(state)
      {
        // WAITING FOR LINK UP?
        case IR_LINKUP:
          // is another console trying to link up? 
          if(dataPacket[0] == IR_INVITE)
          {
            // is this a new conversation tag?
            if(dataPacket[1] != myConvTag) 
            {
              // ok store it
              myConvTag = dataPacket[1];
              EEPROM.write(EEPROM_MYCONVTAG, myConvTag);
            }
            
            // acknowledge
            sendCommand(IR_ACK_INVITE);
            state = IR_LINKED;
          }
          else if(dataPacket[0] == IR_ACK_INVITE && dataPacket[1] == myConvTag)
          {
            Disp8x8.cls();
            Disp8x8.green[0] = 0b01111100;
            Disp8x8.green[1] = 0b10000001;
            Disp8x8.green[2] = 0b10100101;
            Disp8x8.green[3] = 0b10000001;
            Disp8x8.green[4] = 0b10100101;
            Disp8x8.green[5] = 0b10111101;
            Disp8x8.green[6] = 0b10000001;  
            Disp8x8.green[7] = 0b01111110;    
            Disp8x8.delayWithRefresh(1000);
            Disp8x8.cls();
            
            // another console has responded to our link
            state = IR_LINKED;
            return IR_RESULT_LINK;
          }          
          break;
           
        // WE ARE LINKED
        case IR_LINKED:
          if(dataPacket[0] == myConvTag && checkSum() == dataPacket[IR_CHECKSUM_IDX])
          {
            // got data, return it to the caller
            sendCommand(IR_ACK_DATA);
            return IR_RESULT_DATA;
          }
          break;
      }
    }
    // TRY TO INSTIGATE A LINK UP
    else if(state == IR_LINKUP && ms > nextEvent)
    {
      sendCommand(IR_INVITE);
      nextEvent = ms + IR_INVITE_PERIOD;
    }
    return IR_RESULT_NONE;
  }  */
};
extern CIRLinkClass IRLink;
  
#endif //__IRLINK_H__
