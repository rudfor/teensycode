#include <Adafruit_ST7735.h>                          // Screen-specific library

#define mcp1 32                                     // First relay will attenuate by 32db.
#define mcp2 16                                     // Second relay will attenuate by 16db.
#define mcp3 8                                      // Third relay will attenuate by 8db.
#define mcp4 4                                      // Fourth relay will attenuate by 4db.
#define mcp5 2                                      // Fifth relay will attenuate by 2db.
#define mcp6 1                                      // Sixth relay will attenuate by 1db.

float textColor = 0x5ADF;
Adafruit_MCP23008 mcpV;                             //Initiate MCP23008 for Volume attinuation

boolean gpvol1 = 0;                                 //Variables for relay calculation
boolean gpvol2 = 0;
boolean gpvol3 = 0;
boolean gpvol4 = 0;
boolean gpvol5 = 0;
boolean gpvol6 = 0;

void setMute(bool mute, int &volume_old, Adafruit_ST7735 &tft, bool force = false){
  
  }

/*=====================================================
             _ __     __    _    ____  
    ___  ___| |\ \   / /__ | |  / /\ \ 
   / __|/ _ \ __\ \ / / _ \| | | |  | |
   \__ \  __/ |_ \ V / (_) | | | |  | |
   |___/\___|\__| \_/ \___/|_| | |  | |
                                \_\/_/ 
=====================================================*/

void setVol(const int volume, int &volume_old, Adafruit_ST7735 &tft, bool force = false){
  if (volume == volume_old && force!=true) {return;}
/*========================================================================
                  _      _         ___  _         _           
    _  _ _ __  __| |__ _| |_ ___  |   \(_)____ __| |__ _ _  _ 
   | || | '_ \/ _` / _` |  _/ -_) | |) | (_-< '_ \ / _` | || |
    \_,_| .__/\__,_\__,_|\__\___| |___/|_/__/ .__/_\__,_|\_, |
        |_|                                 |_|          |__/ 
========================================================================*/
  int volume_temp = volume;
  tft.fillRect(20,17,140,35,ST7735_BLACK);
  
  if (volume == 0) {
    }
  else if (volume > 99) {
    tft.setCursor(42, 50);
    tft.setTextColor(textColor);
    tft.setTextSize(1);
    tft.println(volume);
  }
  else if (volume > 9) {
    tft.setCursor(55, 50);
    tft.setTextColor(textColor);
    tft.setTextSize(1);
    tft.println(volume);
  }
  else {
    tft.setCursor(68, 50);
    tft.setTextColor(textColor);
    tft.setTextSize(1);
    tft.println(volume);
  }

  Serial.print("Setting volume to: ");
  Serial.print(volume);
  Serial.print("\n");
/*=========================================================================================================
                            __   __   _                  _                 _           
    _ __  __ _ _ _ ___ ___  \ \ / /__| |_  _ _ __  ___  | |_ ___   _ _ ___| |__ _ _  _ 
   | '_ \/ _` | '_(_-</ -_)  \ V / _ \ | || | '  \/ -_) |  _/ _ \ | '_/ -_) / _` | || |
   | .__/\__,_|_| /__/\___|   \_/\___/_|\_,_|_|_|_\___|  \__\___/ |_| \___|_\__,_|\_, |
   |_|                                                                            |__/ 
=========================================================================================================*/
/*
set dbVolume to 0 of volume larger than 32,16,8,4,2,1 then toggle flags gpvol6,5,4,3,2,1
*/

  int dbValue = 0;
  gpvol6 = (volume_temp >= 32)?1:0;
  if(gpvol6 == 1){
    dbValue+=mcp1;
    volume_temp -= 32;
    }
  Serial.println(gpvol6);
  
  gpvol5 = (volume_temp >= 16)?1:0;
  if(gpvol5 == 1){
    dbValue+=mcp2;
    volume_temp -=16;
    }
  Serial.println(gpvol5);
  
  gpvol4=(volume_temp >= 8)?1:0;
  if(gpvol4 == 1){
    dbValue+=mcp3;
    volume_temp -=8;
    }
  Serial.println(gpvol4);
  
  gpvol3=(volume_temp >= 4)?1:0;
  if(gpvol3 == 1){
    dbValue+=mcp4;
    volume_temp -=4;
    }
  Serial.println(gpvol3);
  
  gpvol2=(volume_temp >= 2)?1:0;
  if(gpvol2 == 1){
    dbValue+=mcp5;
    volume_temp -=2;
    }
  Serial.println(gpvol2);
  
  gpvol1=(volume_temp >= 1)?1:0;
  if(gpvol1 == 1){
    dbValue+=mcp6;
    volume_temp -=1;
    }
  Serial.println(gpvol1);
  
  Serial.print("dbValue: ");
  Serial.println(dbValue);
  Serial.print("volumeTemp: ");
  Serial.println(volume);
  Serial.print("mute pin: ");

/*=========================================================================================================*/
//         _   _                     _       
//    __ _| |_| |_ ___ _ _ _  _ __ _| |_ ___ 
//   / _` |  _|  _/ -_) ' \ || / _` |  _/ -_)
//   \__,_|\__|\__\___|_||_\_,_\__,_|\__\___|
//                                           
/*=========================================================================================================*/    
    if (volume > volume_old)                                    // If we are increasing the attenuation
      {
      Serial.println("Increasing the attenuation");
      (gpvol1 == 0)?mcpV.digitalWrite(5, LOW):mcpV.digitalWrite(5, HIGH); //1
      (gpvol2 == 0)?mcpV.digitalWrite(4, LOW):mcpV.digitalWrite(4, HIGH); //2
      (gpvol3 == 0)?mcpV.digitalWrite(3, LOW):mcpV.digitalWrite(3, HIGH); //4
      (gpvol4 == 0)?mcpV.digitalWrite(2, LOW):mcpV.digitalWrite(2, HIGH); //8
      (gpvol5 == 0)?mcpV.digitalWrite(1, LOW):mcpV.digitalWrite(1, HIGH); //16
      (gpvol6 == 0)?mcpV.digitalWrite(0, LOW):mcpV.digitalWrite(0, HIGH); //32
      }
    if (volume < volume_old)                                  // If we are decreasing the attenuation
      {
      Serial.println("Decreasing the attenuation");
      (gpvol6 == 0)?mcpV.digitalWrite(0, LOW):mcpV.digitalWrite(0, HIGH); //32
      (gpvol5 == 0)?mcpV.digitalWrite(1, LOW):mcpV.digitalWrite(1, HIGH); //16
      (gpvol4 == 0)?mcpV.digitalWrite(2, LOW):mcpV.digitalWrite(2, HIGH); //8
      (gpvol3 == 0)?mcpV.digitalWrite(3, LOW):mcpV.digitalWrite(3, HIGH); //4
      (gpvol2 == 0)?mcpV.digitalWrite(4, LOW):mcpV.digitalWrite(4, HIGH); //2
      (gpvol1 == 0)?mcpV.digitalWrite(5, LOW):mcpV.digitalWrite(5, HIGH); //1
      }
    volume_old = volume;
    }
/*
     __              _  __            _ __   __   _  ____  
    / /  ___ _ _  __| | \ \   ___ ___| |\ \ / /__| |/ /\ \ 
   < <  / -_) ' \/ _` |  > > (_-</ -_)  _\ V / _ \ | |  | |
    \_\ \___|_||_\__,_| /_/  /__/\___|\__|\_/\___/_| |  | |
                                                    \_\/_/ 
*/
