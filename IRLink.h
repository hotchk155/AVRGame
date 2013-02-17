#ifndef __IRLINK_H__
#define __IRLINK_H__
enum {
      IRLINK_SEND = 1,
      IRLINK_RECEIVE = 2
  };
class CIRLinkClass {  
  
public:  
  byte enabled;
  byte mode;
  CIRLinkClass() {
    enabled = 0;
    mode = IRLINK_SEND|IRLINK_RECEIVE;
  }
  void enable() {    
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
  
  void sendEvent(char ev)
  {
    if(!enabled || !(mode & IRLINK_SEND))
      return;
    Serial.write(ev);
  }
  
  int getEvent()
  {
    if(!enabled|| !(mode & IRLINK_RECEIVE))
      return 0;
    if(Serial.available()) {
      switch(Serial.read())
      {
        case 'A': return EV_PRESS_A;
        case 'B': return EV_PRESS_B;
        case 'C': return EV_PRESS_C;
        case 'D': return EV_PRESS_D;
        case 'a': return EV_RELEASE_A;
        case 'b': return EV_RELEASE_B;
        case 'c': return EV_RELEASE_C;
        case 'd': return EV_RELEASE_D;
      }      
    }  
    return 0;
  }  
};
extern CIRLinkClass IRLink;
  
#endif //__IRLINK_H__
