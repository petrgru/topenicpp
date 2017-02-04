//
// Created by petrg on 25.1.17.
//
#include <Arduino.h>
#include <HardwareSerial.h>
#include "topeni.h"
#define INTERVAL 60000;
float previousMillis=0;
long interval = 60000;

Topeni::Topeni() {
    // button
    loopCounter = 1;
}
void Topeni::loop() {
    float currentMillis=millis();
    if(currentMillis - previousMillis > interval) {
        previousMillis = currentMillis;
        Serial.println("casovac");
        switch (selprofil) {
            case 0:
                Serial.println("Normal");
                break;
            case 1:
                Serial.println("Solar");
                break;
            case 2:
                Serial.println("Utlum");
                break;

        }
    }
}
