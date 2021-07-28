/*
    This helper file contains all the function calls required by the "OS"

*/
#ifndef _HELPER_H
#define _HELPER_H

//  For the ESP32 WROOM 32, these are the default GPIOs.
//  Display - updated Feb.,26,2021 - to adjust for SD Card on SPI bus GPIO5 is now SD_CS as per library.
#define TFT_DC          27  //GPIO27
#define TFT_CS          15   //GPIO15
#define TFT_MOSI        23  //GPIO23  - VSPI-MOSI
#define TFT_CLK         18  //GPIO18  - VSPI CLK
#define TFT_RST         4   //GPIO4
#define TFT_MISO        19  //GPIO19  - VSPI-MISO
#define TFT_BLANK       8   //GPIO8
//  new for blanking tft via MOSFET - GPIO6
//**********************************************************
//Bluetooth Low Energy
//**********************************************************
#define BUILTIN_LED   2

//  Touch SPI
#define _sclk     25   //GPIO25
#define _mosi     32  //GPIO32
#define _miso     39  //GPIO39
#define TOUCH_CS  33  //GPIO33

#define TFT_LANDSCAPE             1
#define TFT_PORTRAIT_NORMAL       2
#define TFT_LANDSCAPE_INVERT      3
#define TFT_PORTRAIT_INVERT       4

#define border_colour ILI9341_YELLOW
#define num_button_colour ILI9341_RED
#define key_button_colour ILI9341_WHITE
#define yes_no_button_colour ILI9341_GREEN
#define num_text_colour ILI9341_BLACK
#define MENU 0
#define ALPHA 1
#define NUMERIC 2
#define YESNO 3
int GFIPZ = 3;           //  values from 1 to 4. 1 - 1:1, 2 - 2:1, 3 - 3:1, 4 - 4:1.

const uint8_t TFT_ORIENTATION = TFT_PORTRAIT_NORMAL;

//  VARS  ********************************************************
bool DEBUG = false;

const int TOUCH_IRQ = 35;// to identify the pin to watch.


//  MLX90614 ir
bool IR_READY = false;
bool IR_ACTIVE = false;
//  INITIALIZATION ******************************************************
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

float flirFrame[32 * 24];     // buffer for full frame of temperatures

XPT2046_Touchscreen touch(TOUCH_CS);
TS_Point rawLocation;
double ATC, OTC, ATF, OTF;

//  FUNCTIONS & ROUTINES  ***********************************************
// The almighty RAM LOAD INTERRUPT(someone touched me and I know where!) TouchX,TouchY,PAD_DOWN gets updated
// void IRAM_ATTR ScreenPressed() {
//   // halt further interrupts
//   detachInterrupt(TOUCH_IRQ);
//   if ( touch.touched() )
//   {
//     // Serial.print("INSIDE TOUCH INT \n");
//     rawLocation = touch.getPoint();
//     TouchX = rawLocation.x;
//     TouchY = rawLocation.y;
//     PAD_DOWN = true;
//   }
//   digitalWrite(BUILTIN_LED, HIGH);
//   // Serial.print("led high \n");
//
//   // OK good to go...
//   attachInterrupt(digitalPinToInterrupt(TOUCH_IRQ), ScreenPressed, FALLING);
// }
// //This is to re-enabe the touch interrupt
// void ReEnableTouchInt() {
//   attachInterrupt(digitalPinToInterrupt(TOUCH_IRQ), ScreenPressed, FALLING);
// }

void InitDisplay() {
  tft.begin(60000000);
  delay(10);
  tft.setRotation(TFT_ORIENTATION);
  tft.fillScreen(ILI9341_BLACK);
}

void clearAll() {
  tft.fillRoundRect(7, 7, 226, 21, 8, ILI9341_BLACK);
  tft.fillRoundRect(7, 37, 226, 196, 8, ILI9341_BLACK);
}
void clearBody() {
  tft.fillRoundRect(7, 37, 226, 196, 8, ILI9341_BLACK);
}
void clearStatus() {
  tft.fillRoundRect(7, 7, 226, 21, 8, ILI9341_BLACK);
}
/*
   show status will have two colours to display red and yellow
*/
void showStatus(String z) {
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.fillRoundRect(7, 7, 226, 21, 8, ILI9341_BLACK);
  tft.setCursor(15, 15);
  tft.print(z);
}
//  Clear Screen! just so i don't have to type blah blah blah! (macro)
void ClearScreen() {
  tft.fillScreen(ILI9341_BLACK);
}

void showBiosInfo() {
  ClearScreen();
  tft.setCursor(0, 0);
  tft.drawRoundRect(0, 0, 320, 240, 10, border_colour);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setCursor(65, 10);
  tft.print("ETMS - CAPRICORN SOFTWARE - 2021");
  tft.setCursor(90, 20);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.print("BIOS V1.02.6 - 2/15/2021");
  tft.setCursor(135, 30);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.println("-System-");
  tft.setCursor(100, 40);
  tft.setTextColor(ILI9341_MAGENTA, ILI9341_BLACK);
  tft.print("ESP WROOM 32 240MHz");
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(30, 50);
  tft.print("CPU Dual Core..Pass");
  tft.setCursor(30, 60);
  tft.print("Memory.........Pass");
  tft.setCursor(180, 50);
  tft.print("ILI9341........Pass");
  tft.setCursor(180, 60);
  tft.print("XPT2046........Pass");
}

void initMyTouch() {
  //original havn't touched
  SPI.begin(_sclk, _miso, _mosi);
  SPI.setClockDivider(1);
  SPI.beginTransaction(SPISettings(60000000, MSBFIRST, SPI_MODE0));
  SPI.endTransaction();
  touch.begin();
}
/*
  ScanI2CBus is implemented - 9/21/2020
  this has to be done before init sensor group
  sets the sensor availability so the system can ignore offline sensors
  to manage performance and no bad data woes.
*/
void scanI2CBus() {
  int lastRow = 220;
  byte error, address;
  int nDevices;
  Wire.begin();
  tft.setCursor(15, lastRow);
  tft.print("I2C Bus Scan:");
  for (address = 1; address < 127; address++ )
  {
    /*
      The i2c_scanner uses the return value of
      the Write.endTransmisstion to see if
      a device did acknowledge to the address.
    */

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      tft.setCursor(15, lastRow - 20);
      tft.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
      tft.print("I2C device at address 0x");
      if (address < 0x10) {
        tft.print("0");
      }
      tft.print(address, HEX);
      tft.print(" hex !");
      //tft.setCursor(60, lastRow);
      //tft.print(address);
      tft.setCursor(100, lastRow);
      tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      nDevices++;
      delay(500);
    }
    else if (error == 4)
    {
      tft.setCursor(20, lastRow);
      tft.print("Error at address 0x");
      if (address < 16)
        tft.print("0");
      tft.println(address, HEX);
    }
  }
  if (nDevices == 0)
  {
    tft.setCursor(180, lastRow);
    tft.print("No I2C devices found");
  }
  else
  {
    tft.setCursor(15, lastRow);
    tft.print("I2C Bus Scan:");
    tft.setCursor(100, lastRow);
    tft.print("Done....            ");
  }
  delay(500);
}
/*
   This initialize handles most of the devices that is available
   If it finds a sensor it attempts to initialize or if the
   sensor is not attached it will flag the unit so the system
   will ignore it.
*/
void initializeSensorGroup() {
  int lineCount = 80;
  int itemStart = 40;
  int resutPos = 170;
  IR_READY = false;


  // MLX90614 test results.
  lineCount += 10;
  tft.setCursor(itemStart, lineCount);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print("IR");
  if (IR_READY) {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.print("Pass");
  } else {
    tft.setCursor(resutPos, lineCount);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.print("Fail");
  }
  delay(5000);
  Wire.setClock(400000);
}

void showSystemStatus() {
  // in here we just get a look at all hardware connected to the unit.
  clearAll();
  showStatus("not ready!");
}

/*
   Because the adafruit library only returns numbers, we will take the values and
   plot them to the screen at a default scale and pallete. This is adjustable with
   functions setZoom() and setFlirColours will set the optional parameters.
*/
uint16_t numberToColour(float t) {
  // t is your temp range (20-39)
  // here is where we map the temperature to a selected pixel from a group of custom palettes.
  if (t < 20) return ILI9341_BLACK;
  else if (t < 23) return ILI9341_BLUE;
  else if (t < 25) return ILI9341_MAGENTA;
  else if (t < 27) return ILI9341_CYAN;
  else if (t < 29) return ILI9341_GREEN;
  else if (t < 31) return ILI9341_YELLOW;
  else if (t < 33) return ILI9341_ORANGE;
  else if (t < 35) return ILI9341_PINK;
  else if (t < 37) return ILI9341_RED;
  else if (t < 39) return ILI9341_BLACK;
}

void bufferToScreen() {
  uint8_t x = 200 / GFIPZ;
  uint8_t y = 180 / GFIPZ;
  for (uint8_t h = 0; h < 24; h++)
  {
    for (uint8_t w = 0; w < 32; w++)
    {
      float t = flirFrame[h * 32 + w];
      //centering position offset depending on zoom factor 1 - 4

      int xpixel = x + (w * GFIPZ);
      int ypixel = y + (h * GFIPZ);
      //tft.fillRect(xpixel, ypixel, (xpixel + GFIPZ), (ypixel + GFIPZ), numberToColour(t));

      //
      tft.drawPixel(xpixel, ypixel,  numberToColour(t));
      tft.drawPixel(xpixel + 1, ypixel,  numberToColour(t));
      tft.drawPixel(xpixel, ypixel + 1,  numberToColour(t));
      tft.drawPixel(xpixel + 1, ypixel + 1,  numberToColour(t));
    }
  }
}

void ShowIRResults() {
  showStatus("IR Body Temp:");
  // Wire.setClock(400000);
  ClearScreen();         //Clear the display
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(35, 5);
  tft.println("Body Temp:");
  tft.setTextSize(2);
  tft.setCursor(20, 23);
  tft.print(OTC);
  tft.print((char)247);
  tft.println("C");
  if (OTC > 37.8) {
    tft.setTextSize(1);
    tft.setCursor(32, 45);
    tft.println("FAIL - Fever!");
  }
  if (OTC < 36.1) {
    tft.setTextSize(1);
    tft.setCursor(0, 45);
    tft.println("Ready - Place Sensor!");
  }
  if (OTC < 37.8 & OTC > 36.1) {
    tft.setTextSize(1);
    tft.setCursor(32, 45);
    tft.println("PASS - Normal");
  }
}
// //  Read the IR sensor
void ReadIRTemp() {
  Wire.setClock(100000);
  OTC = mlx.readObjectTempC();
  OTF = mlx.readObjectTempF();
  ATC = mlx.readAmbientTempC();
  ATF = mlx.readAmbientTempF();
  ShowIRResults();
}
/*
   OxiTest - Here we wait for the finger and when we have a reading we will display the results.
   Reading the IR value it will permit us to know if there's a finger on the sensor or not. Also
   detecting a heartbeat. If a finger is detected, clear the display,draw the first bmp
   picture (little heart). Near it display the average BPM you can display the BPM if you want.
   If a heart beat is detected, clear the display and draw the second picture (bigger heart).
   And still display the average BPM
*/
void showAdmin() {
  clearBody();
  tft.setCursor(40, 80);
  showStatus("No Administrator!");
}
#endif
