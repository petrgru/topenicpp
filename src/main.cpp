//
// Created by petrg on 25.1.17.
//

#include <Arduino.h>
#include <EEPROM.h>
#include "main.h"
#include "localmenu.h"
#include "topeni.h"
#include "prominiextender.h"
#define INTERVAL 10000
float previousMillis;

TopeniMenu menu;
Topeni topeni;
prominiextender promini;
void loadEEPROM() {
    selprofil=EEPROM.read(0);
    NormalTeplotaDay=EEPROM.read(1);
    NormalTeplotaNight=EEPROM.read(2);
    UtlumTeplota=EEPROM.read(3);
}

void setup() {
    loadEEPROM();
    Serial.println("Load EEPROM");
    menu.setup();
    Serial.println("setup");
    promini.setup();
    topeni.setup();
}

void loop() {
    float currentMillis = millis();
    menu.loop();
    topeni.loop();
    promini.loop();

    if(currentMillis - previousMillis > INTERVAL) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;
        Serial.println("casovac");
        Serial.println(sensorInterier);
    }

}