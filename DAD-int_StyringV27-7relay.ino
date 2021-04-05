/*-------------------------------------------------------------------------------------------
 * Martinsey project with DAD-int
 * 
 * Helpfull links:
 * La-skala-attenuator project Page: http://www.dimdim.gr/diyaudio/la-skala-attenuator/ 
 * 
 * For choosing colors: http://www.barth-dev.de/online/rgb565-color-picker/
 * 
 --------------------------------------------------------------------------------------------*/

//#include <UTFT.h>                                   // Library for the TFT display.
//#include <RotaryEncoder.h>                          // Library for the encoder.
#include <Wire.h>                                     // Library for the I2C communication.
#include "Adafruit_MCP23008.h"                        // Library for the I/O expander.
#include <spi4teensy3.h>                              // SPI optimized for teensy communication library
#include <Adafruit_GFX.h>                             // Core graphics library
#include <Adafruit_ST7735.h>                          // Screen-specific library
#include <Fonts/FreeSansBold24pt7b.h>                 // Font library
#include <IRremote.h>                                 // IR library
#include "setVol.h"
#include "setChannel.h"
//#include <SPI.h>                                    // SPI communication library

//float textColor = 0x5ADF;

int volume = 0;                                       // Default attenuation.    was float, now it is int
int volume_old = 0;                                   // Store old volume.       was float, now it is int

                                                      // Pins for ST7735 displays
//#define TFT_CS   99                                 // Slave Select for 1st screen
#define TFT_SCK  27
#define TFT_MOSI 28
#define TFT_DC   29
#define TFT_RST  30                                   // you can also connect this to the Teensy reset, in which case, set this #define pin to 0!
#define TFT_CS2  31                                   // Slave Select for 2nd screen

//below definitions were optional.
#define GPMUTE   33                                   // MUTE PIN
#define GPVOL1   34                                   // 1
#define GPVOL2   35                                   // 2
#define GPVOL3   36                                   // 4
#define GPVOL4   37                                   // 8
#define GPVOL5   38                                   // 16
#define GPVOL6   39                                   // 32

//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);     // Initiate the 1st display.
Adafruit_ST7735 tft2 = Adafruit_ST7735(TFT_CS2,  TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);   // Initiate the 2nd display.

//-------------------------------------MPC23008 code---------------------------------
// now defined in setVol and setChannel

//---------------------------------Rotary encoder pinout-------------------------------
int VOLUPPIN=26;           // RotEnc A terminal for right rotary encoder.
int VOLDOWNPIN=25;         // RotEnc B terminal for right rotary encoder.
int pos;
int newPos;
//RotaryEncoder encoder(VOLDOWNPIN, VOLUPPIN);       // Setup the first Rotary Encoder

//---------------------------------Channel Select-------------------------------
int buttonCounter = 0;
//int activeChannel= 0;
int powerButton = 0;
int channelButton = 1;

boolean ButtonPressed = false;
boolean Toggle = false;
int Debounce = 0;

//-------------------------------IR reciever setup-----------------------------
int IRpin = 32;  // pin for the IR sensor
IRrecv irrecv(IRpin);
decode_results results;
int prev_result;
//int DebounceIR = 0;
//int ButtonPressedIR = 0;
//int ToggleIR = 0;

#define PWR1 0x401     // Code for power on/off
#define PWR2 0xC01     // Code for power on/off
#define VOU1 0x2FE     // Code for Volume up
#define VOU2 0xAFE     // Code for Volume up
#define VOD1 0x2FC     // Code for Volume down
#define VOD2 0xAFC     // Code for Volume down
#define MUT1 0x2FA     // Code for mute
#define MUT2 0xAFA     // Code for mute
#define CHU1 0x40B     // Code for Channel UP
#define CHU2 0xC0B     // Code for Channel UP
#define CHD1 0x40A     // Code for Channel DOWN
#define CHD2 0xC0A     // Code for Channel DOWN


bool isOn = false;
int powerButton2 = 0;
int mute2 = 0;
int channelUp2 = 0;
int channelDown2 = 0;
int volumeUp2 = 0;
int volumeDown2 = 0;

int channelUp3 = 0;
int channelDown3 = 0;
int mute3 = 0;

//---------------------------------Power ON/OFF Select-------------------------------
int systemOnOff = 0;
boolean ButtonPressed1 = false;
boolean Toggle1 = false;
int Debounce1 = 0;

/*================================================
    ____  _____ _____ _   _ ____     ____  
   / ___|| ____|_   _| | | |  _ \   / /\ \ 
   \___ \|  _|   | | | | | | |_) | | |  | |
    ___) | |___  | | | |_| |  __/  | |  | |
   |____/|_____| |_|  \___/|_|     | |  | |
                                    \_\/_/ 
================================================*/
void setup() {

  pinMode(channelButton, INPUT);
  pinMode(powerButton, INPUT);

  //myGLCD.InitLCD();
  //myGLCD.setFont(BigFont);

//---------------------------------------------MCP23008 pinmodes----------------------------------------------------
  mcpV.begin();                                // use 0 for default address, Volume attinuation
  mcpV.pinMode(0, OUTPUT);
  mcpV.pinMode(1, OUTPUT);
  mcpV.pinMode(2, OUTPUT);
  mcpV.pinMode(3, OUTPUT);
  mcpV.pinMode(4, OUTPUT);
  mcpV.pinMode(5, OUTPUT);
  mcpV.pinMode(6, OUTPUT);
  mcpV.pinMode(7, OUTPUT);

  mcpK.begin(1);                                // use 1 for 2nd chip address,Channel selecter
  mcpK.pinMode(0, OUTPUT);
  mcpK.pinMode(1, OUTPUT);
  mcpK.pinMode(2, OUTPUT);
  mcpK.pinMode(3, OUTPUT);
  mcpK.pinMode(4, OUTPUT);
  mcpK.pinMode(5, OUTPUT);
  mcpK.pinMode(6, OUTPUT);
  mcpK.pinMode(7, OUTPUT);
//-------------------------------------------------------------------------------------------------------------------

  setVol(volume,volume_old,tft2);                   // Set the attenuation at its default value. Must be as close to the beginning of the code as possible to avoid audio spikes at start-up.
  
  pinMode(VOLUPPIN, INPUT);         // Button switch or Encoder pin for volume up
  //digitalWrite(VOLUPPIN, HIGH);   // If H/W debouncing is implemented, set to LOW
  pinMode(VOLDOWNPIN, INPUT);       // Button switch or Encoder pin for volume down
  //digitalWrite(VOLDOWNPIN, HIGH); // If H/W debouncing is implemented, set to LOW
     

  Serial.begin(115200);             // Start serial communication

  tft2.initR(INITR_BLACKTAB);       // Initialize a ST7735S chip, black tab
  //tft.initR(INITR_BLACKTAB);      // Initialize a ST7735S chip, black tab
  uint16_t time = millis();
  tft2.fillScreen(ST7735_BLACK);
  time = millis() - time;

  tft2.setRotation(3);
  tft2.setFont(&FreeSansBold24pt7b);

  irrecv.enableIRIn();              // Start the IR receiver

  mcpK.digitalWrite(1, LOW);
  mcpK.digitalWrite(2, LOW);
  mcpK.digitalWrite(3, LOW);
  mcpK.digitalWrite(0, HIGH);

  mcpV.digitalWrite(7, LOW);
  mcpK.digitalWrite(7, LOW);

  delay(100);
//setVol(0,volume_old,tft2);
//setChannel(true,activeChannel,tft2,true);
//digitalWrite(gpmutepin, LOW);
  tft2.fillScreen(ST7735_BLACK);
  pos = digitalRead(VOLUPPIN);
}
/*=====================================================================
     __              ___    ____  _____ _____ _   _ ____     ____  
    / /___ _ __   __| \ \  / ___|| ____|_   _| | | |  _ \   / /\ \ 
   / // _ \ '_ \ / _` |\ \ \___ \|  _|   | | | | | | |_) | | |  | |
   \ \  __/ | | | (_| |/ /  ___) | |___  | | | |_| |  __/  | |  | |
    \_\___|_| |_|\__,_/_/  |____/|_____| |_|  \___/|_|     | |  | |
                                                            \_\/_/ 
=====================================================================*/


/*=====================================================================
//    _     ___   ___  ____     ____  
//   | |   / _ \ / _ \|  _ \   / /\ \ 
//   | |  | | | | | | | |_) | | |  | |
//   | |__| |_| | |_| |  __/  | |  | |
//   |_____\___/ \___/|_|     | |  | |
//                             \_\/_/ 
=====================================================================*/
void loop() {
//-----------------------IR reciever decoded result--------------------
  if (irrecv.decode(&results)) {
    if (results.value == PWR1 && PWR1 != prev_result) {powerButton2 = 1;}
    if (results.value == PWR2 && PWR2 != prev_result) {powerButton2 = 1;}
    if (results.value == MUT1 && MUT1 != prev_result) {mute2 = 1;}
    if (results.value == MUT2 && MUT2 != prev_result) {mute2 = 1;}
    if (results.value == CHU1 && CHU1 != prev_result) {channelUp2 = 1;}
    if (results.value == CHU2 && CHU2 != prev_result) {channelUp2 = 1;}
    if (results.value == CHD1 && CHD1 != prev_result) {channelDown2 = 1;}
    if (results.value == CHD2 && CHD2 != prev_result) {channelDown2 = 1;}

    /*
    if volume up triggered.
    */
    if (systemOnOff) {
      if (results.value == VOU1 || results.value == VOU2) {
        if (volume < 64)               {volume ++;}
        if(isMute) {isMute=false;}
        setVol(volume,volume_old,tft2);
        }
      if (results.value == VOD1 || results.value == VOD2) {
        if (volume > 0)                   {volume --;}
        if(isMute) {isMute=false;}
        setVol(volume,volume_old,tft2);
        }
      if (mute2 == 1) {
        setMute(!isMute, volume, volume_old, tft2);
        mute2 = 0;
        }
      if (channelUp2 == 1 || channelDown2 == 1) {
        if (channelUp2 != channelDown2){
          setChannel(channelUp3, activeChannel,tft2);
          }
          channelUp2 = 0;
          channelDown2 = 0;
        }
      }
  prev_result = results.value;
  irrecv.resume(); 
  }

//----------------------------System On/Off-------------------------------------------
  if (digitalRead(powerButton)==HIGH || powerButton2 == 1)  {Debounce1++;}
  else {
    Debounce1 = 0;
    ButtonPressed1 = false;
    Toggle1 = false;
    }

  ButtonPressed1 = (Debounce1 >= 10)?true:false;
  
  if (ButtonPressed1 == true && Toggle1 == false)
    {
    Toggle1 = true;
    systemOnOff = (systemOnOff==0)?1:0;                     //Switch OnOff mode 
    if (systemOnOff == 1)
      {
      mcpK.digitalWrite(7, HIGH);
      tft2.fillScreen(ST7735_BLACK);
      tft2.setRotation(3);
      setChannel(true,activeChannel,tft2,true);
      setVol(volume,volume_old,tft2);
      powerButton2 = 0;
      }
 
  if (systemOnOff == 0){
    setMute(true);
    mcpV.digitalWrite(7, LOW);
    mcpK.digitalWrite(7, LOW);
    tft2.fillScreen(ST7735_BLACK);
    tft2.setRotation(3);
    //delay(50);
    //setChannel(true,activeChannel,tft2,true);
    //setVol(volume,volume_old,tft2,true);
    //tft2.fillRect(55,17,51,35,ST7735_BLACK);
    //tft2.fillRect(55,72,51,35,ST7735_BLACK);
    powerButton2 = 0;
    }
  }

 //-----------------------Rotary encoder Volume Up/Down----------------------------
   newPos = digitalRead(VOLUPPIN); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (newPos != pos && systemOnOff == 1 && mute3 == 0){
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(VOLDOWNPIN) != newPos) {
        if (volume > 0){volume --;}
     } 
     else {
        if (volume < 64){volume ++;}
     }
     //Serial.print("Position: ");
     //Serial.println(volume);
     pos = newPos; // Updates the previous state of the outputA with the current state
     setVol(volume,volume_old,tft2);
   }
/*
//-----------------------IR reciever decoded result--------------------
  if (irrecv.decode(&results)) {
    //Serial.println(results.value);
    irrecv.resume(); 
    }
*/
//-----------------------Channel Debouncer code--------------------------------
//@deprecated
  if (digitalRead(channelButton)==HIGH && systemOnOff == 1)   {Debounce++;}
  else {
    Debounce = 0;
    ButtonPressed = false;
    Toggle = false;
    }
  if (Debounce >= 100)                          {ButtonPressed = true; }
  if (ButtonPressed == true && Toggle == false) {
    Toggle = true;
    channelUp3 = 1;
    }

//-----------------------IR Channel Switch code--------------------------------

//-----------------------Channel Switch Up/Down code--------------------------------

}
/*=====================================================
                _         _               
    ___ _ _  __| |  ___  | |___  ___ _ __ 
   / -_) ' \/ _` | |___| | / _ \/ _ \ '_ \
   \___|_||_\__,_|       |_\___/\___/ .__/
                                    |_|   
=====================================================*/
