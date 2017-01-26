/****************************************************************************************************************************\
 * Arduino project "ESP Easy" � Copyright www.esp8266.nu
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You received a copy of the GNU General Public License along with this program in file 'License.txt'.
 *
 * IDE download    : https://www.arduino.cc/en/Main/Software
 * ESP8266 Package : https://github.com/esp8266/Arduino
 *
 * Source Code     : https://sourceforge.net/projects/espeasy/
 * Support         : http://www.esp8266.nu
 * Discussion      : http://www.esp8266.nu/forum/
 *
 * Additional information about licensing can be found at : http://www.gnu.org/licenses
\*************************************************************************************************************************/

// This file is to be loaded onto an Arduino Pro Mini so it will act as a simple IO extender to the ESP module.
// Communication between ESP and Arduino is using the I2C bus, so only two wires needed.
// It best to run the Pro Mini on 3V3, although the 16MHz versions do not officially support this voltage level on this frequency.
// That way, you can skip levelconverters on I2C.
// Arduino Mini Pro uses A4 and A5 for I2C bus. ESP I2C can be configured but they are on GPIO-4 and GPIO-5 by default.
#include <MenuSystem.h>
#include <Wire.h>
#include <OneButton.h>
#include <LiquidCrystal_I2C.h>
#include "menu.h"
//#include "menu.cpp"
#define I2C_MSG_IN_SIZE    4
#define I2C_MSG_OUT_SIZE   4

#define CMD_DIGITAL_WRITE  1
#define CMD_DIGITAL_READ   2
#define CMD_ANALOG_WRITE   3
#define CMD_ANALOG_READ    4


#define lcdBrightnessPin 2
#define lcdContrastPin 3
#define DHTPin 31
#define encoderCLKPin 8
#define encoderDTPin 9
#define encoderSWPin 10
#define boilerSensorPin 4
#define collectorSensorPin 5
#define t1SensorPin 6
#define t2SensorPin 7
#define ChipSelect 53

#define DEBUG 1
volatile uint8_t sendBuffer[I2C_MSG_OUT_SIZE];

double roomTemp, roomHumidity = 50;
int roomPressure = 1111;
unsigned long lastUpdate, lastAction = 0;
unsigned long statusDelay = 10L * 1000L; //quit menu after ms of no use
unsigned long sleepDelay = 30L * 1000L; //sleep after ms of no nuse
unsigned long updateInterval = 10L * 1000L;//sensor update interval
unsigned long logInterval = 300L * 1000L;//Log and esp to thingspeak interval

int status = 1;
bool autoMode = true;
bool sleeping = false;
int buttonState = 0;
int lastButtonState = 0;

int current;
String IP;
long millisHeld;
long secsHeld;
long prevSecsHeld;
byte previous = HIGH;
int settingsMinTempDifference, settingsMinTempCollector, settingsMaxTempCollector, settingsMaxTempBoiler, altitude, brightness, contrast;
double roomMinTemp, roomMaxTemp, roomMaxHumidity, roomMinHumidity, roomMinPressure, roomMaxPressure;
unsigned long lastLog, firstTime;



OneButton hlsvetlo_sw(22,true);
OneButton kuchyn_sw(24,true);
LiquidCrystal_I2C lcd(0x26, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
//LiquidCrystal_I2C lcd(0x26, 20, 4);


void hlsvetlo() {
  static int m = LOW;
  // reverse the LED 
  m = !m;
  digitalWrite(50, m);
#if DEBUG
  Serial.print("switch ");Serial.println(m);  
#endif
} 

void kuchyn() {
  static int m = LOW;
  // reverse the LED 
  m = !m;
  digitalWrite(50, m);
#if DEBUG
  Serial.print("switch kuchyn ");Serial.println(m);  
#endif
} 


void setup()
{
#if DEBUG
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Hello world!");  // prints hello with ending line break 
#endif
  menu_setup();
  pinMode(50,OUTPUT);
  pinMode(A0,OUTPUT);
  analogWrite(A0,20);
  
//  scanf( "%d", &hlsvetlo_rele);
  hlsvetlo_sw.attachClick(hlsvetlo);
  kuchyn_sw.attachClick(kuchyn);
  Wire.begin(0x7f);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  serialPrintHelp();

}


void loop() {
  hlsvetlo_sw.tick();
  kuchyn_sw.tick();
  //if (status != 0)   menuEntranceHandler();
  if (status == 0 || sleeping)  controlhandler();
  serialHandler();
  }

void receiveEvent(int count)
{
  if (count == I2C_MSG_IN_SIZE)
  {
    byte cmd = Wire.read();
    byte port = Wire.read();
    int value = Wire.read();
    value += Wire.read()*256;
    switch(cmd)
      {
        case CMD_DIGITAL_WRITE:
          pinMode(port,OUTPUT);
          digitalWrite(port,value);
          break;
        case CMD_DIGITAL_READ:
          pinMode(port,INPUT_PULLUP);
          clearSendBuffer();
          sendBuffer[0] = digitalRead(port);
          break;
        case CMD_ANALOG_WRITE:
          analogWrite(port,value);
          break;
        case CMD_ANALOG_READ:
          clearSendBuffer();
          int valueRead = analogRead(port);
          sendBuffer[0] = valueRead & 0xff;
          sendBuffer[1] = valueRead >> 8;
          break;
      }
  }
}

void clearSendBuffer()
{
  for(byte x=0; x < sizeof(sendBuffer); x++)
    sendBuffer[x]=0;
}

void requestEvent()
{
  Wire.write((const uint8_t*)sendBuffer,sizeof(sendBuffer));
}

