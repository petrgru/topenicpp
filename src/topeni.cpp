//
// Created by petrg on 25.1.17.
//

#include "topeni.h"
#include <Arduino.h>

// ----- Serial line speed --------------------------------
#define CONSOLE_BAUDRATE    9600       // baudrate used for console
#define Console Serial


Topeni::Topeni() {
    // button
    loopCounter = 1;
}
void Topeni::setup() {

}
