//
// Created by petrg on 25.1.17.
//

#include "topeni.h"
#include <Arduino.h>
#include <Wire.h>
#include <OneButton.h>
#include <LiquidCrystal_I2C.h>


// ----- Serial line speed --------------------------------
#define CONSOLE_BAUDRATE    9600       // baudrate used for console
#define Console Serial
LiquidCrystal_I2C lcd(0x26, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

Topeni::Topeni() {
    // button
    loopCounter = 1;
}
void Topeni::setup() {
#if DEBUG
  Console.begin(CONSOLE_BAUDRATE);           // set up Serial library at 9600 bps
  Console.println("Setup");  // prints hello with ending line break
#endif
    lcd.begin(20,4);
    lcd.clear();
    lcd.print("test");


}
void Topeni::loop() {

}