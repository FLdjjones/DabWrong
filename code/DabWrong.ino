#include <SPI.h>                  //  spi
#include <Adafruit_GFX.h>         //  graphics
#include <Adafruit_ILI9341.h>     //  display
#include <Adafruit_MLX90614.h>    //  Melexis IR temp
//#include <Adafruit_MLX90640.h>    //  Flir

#include <XPT2046_Touchscreen.h>  //  touch

#include <time.h>                 //  standard time stuff

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "Helper.h"               //  my helper header


//  SETUP ***************************************************************
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);// for debug purposes.
  // Keep power when running from battery
  //bool isOk = setPowerBoostKeepOn(1);
  InitDisplay();
  ClearScreen();
  Serial.println("Adafruit MLX90614 test");
  showBiosInfo();
  delay(2000);
  ClearScreen();
  scanI2CBus();
  Serial.println("Adafruit MLX90614 test");
  //initializeSensorGroup();
  delay(2000);
  ClearScreen();
  Serial.println("Adafruit MLX90614 test");
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };

  // Wire.setClock(100000);
  // OTC = mlx.readObjectTempC();
  // OTF = mlx.readObjectTempF();
  // ATC = mlx.readAmbientTempC();
  // ATF = mlx.readAmbientTempF();
  //initMyTouch();
  //ReEnableTouchInt();

  digitalWrite(BUILTIN_LED, HIGH);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setTextSize(6);
}
//  LOOP  *******************************************************************
void loop() {
  //set the BUILTIN_LED off.
  //
  // Serial.println();
  tft.setCursor(0, 150);
  tft.print(mlx.readObjectTempF());

  delay(500);
  // Serial.printf("Last touch was at x = %s y = %s\n",TouchX, TouchY );
  // digitalWrite(BUILTIN_LED, HIGH);
  // ReadIRTemp()
  // digitalWrite(BUILTIN_LED, LOW);
  if(mlx.readObjectTempF() < 550 & mlx.readObjectTempF() > 400){
    tft.setTextColor(ILI9341_BLACK, ILI9341_GREEN);
    GreenScreen();
  }
  else{
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    ClearScreen();
  }
}// end of loop

void GreenScreen() {
  tft.fillScreen(ILI9341_GREEN);
}
