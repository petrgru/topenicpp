//
// Created by petrg on 25.1.17.
//

#include <Arduino.h>
//#include "topeni.h"
#include "menu.h"
long previousMillis = 0;
long interval = 1000;

TopeniMenu menu;


void setup() {
    menu.setup();
    Serial.println("setup");
}

void loop() {
    unsigned long currentMillis = millis();
    menu.loop();

    if(currentMillis - previousMillis > interval) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;
        Serial.println("casovac");
    }
}