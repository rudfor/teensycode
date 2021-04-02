#include <Adafruit_ST7735.h>                          // Screen-specific library

Adafruit_MCP23008 mcpK;                             //Initiate MCP23008 for Channel Select
char* channelName = (char*)"XLR 1";
int activeChannel = 0;

/*=====================================================
#             _    ____ _                            _  ____  
#    ___  ___| |_ / ___| |__   __ _ _ __  _ __   ___| |/ /\ \ 
#   / __|/ _ \ __| |   | '_ \ / _` | '_ \| '_ \ / _ \ | |  | |
#   \__ \  __/ |_| |___| | | | (_| | | | | | | |  __/ | |  | |
#   |___/\___|\__|\____|_| |_|\__,_|_| |_|_| |_|\___|_| |  | |
#                                                      \_\/_/ 
=====================================================*/

void setChannel(bool channelUP, int &activeChannel, Adafruit_ST7735 &tft, bool forceDisplay = false){
  //if (volume == volume_old && force!=true) {return;}
  if(!forceDisplay) {
    switch(activeChannel){
      case 1:
        activeChannel = (channelUP? 2 : 0);
        Serial.print("activeChannel 1");
        break;
      case 2:
        activeChannel = (channelUP? 3 : 1);
        Serial.print("activeChannel 2");
        break;
      case 3:
        activeChannel = (channelUP? 0 : 2);
        Serial.print("activeChannel 3");
        break;
      default:
        activeChannel = (channelUP? 1 : 3);
        Serial.print("activeChannel 0");
        break;
      }
    }
    
  switch(activeChannel){
    case 1:
        channelName = (char*)"XLR 2";                  //how to name channel
        mcpK.digitalWrite(0, LOW);                      //statements for what to do at each channel counting step
        mcpK.digitalWrite(2, LOW);
        mcpK.digitalWrite(3, LOW);
        mcpK.digitalWrite(1, HIGH);
        Serial.print("activeChannel 1");
        break;
    case 2:
        channelName = (char*)"XLR 3";                  //how to name channel
        mcpK.digitalWrite(0, LOW);                      //statements for what to do at each channel counting step
        mcpK.digitalWrite(1, LOW);
        mcpK.digitalWrite(3, LOW);
        mcpK.digitalWrite(2, HIGH);
        Serial.print("activeChannel 2");
        break;
    case 3:
        channelName = (char*)"RCA";                  //how to name channel
        mcpK.digitalWrite(0, LOW);                      //statements for what to do at each channel counting step
        mcpK.digitalWrite(1, LOW);
        mcpK.digitalWrite(2, LOW);
        mcpK.digitalWrite(3, HIGH);
        Serial.print("activeChannel 3");
        break;
    default:
        channelName = (char*)"XLR 1";                  //how to name channel
        mcpK.digitalWrite(1, LOW);
        mcpK.digitalWrite(2, LOW);
        mcpK.digitalWrite(3, LOW);
        mcpK.digitalWrite(0, HIGH);                      //statements for what to do at each channel counting step    
        Serial.print("activeChannel 0");
        break;
    }

  tft.fillRect(1,71,159,36,ST7735_BLACK);
  tft.setCursor(13, 105);
  tft.setTextColor(textColor);
  tft.setTextSize(1);
  tft.println(channelName);
}
