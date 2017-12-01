//
// Created by petrg on 25.1.17.
//

#include <Arduino.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include "main.h"
#include "localmenu.h"
#include "ntp.h"
#include "topeni.h"
#include "prominiextender.h"
#include "pingwatchdog.h"
#include "mqtt.h"
#define INTERVAL 10000
#define INTERVAL_SEPNUTI 500

float previousMillis;

TopeniMenu menu;
Topeni topeni;
prominiextender promini;
Ntp_client ntpc;
ping_watchdog pingac;
Mqtt mqttprocess;

void loadEEPROM() {
    selprofil=EEPROM.read(0);
    NormalTeplotaDay=EEPROM.read(1);
    NormalTeplotaNight=EEPROM.read(2);
    UtlumTeplota=EEPROM.read(3);
    MaxFloorTeplota=EEPROM.read(4);
}

void pinsetup(){
    int i,f;
    for (i = 0; i < 3; i++) {
        for (f = 0; f < 3; f++) {
            pinMode(srelay[i][f], OUTPUT);
            digitalWrite(srelay[i][f],LOW);
        }
        delay(500);
    }

};


void setup() {
    loadEEPROM();
    Serial.println("Load EEPROM");
    menu.setup();
    ntpc.setup();
    pinsetup();
    Serial.println("setup");
    promini.setup();
    topeni.setup();
    mqttprocess.setup();
//    pingac.setup();
    wdt_enable(WDTO_8S);
}
void loop() {
    wdt_reset();
    menu.loop();
    ntpc.loop();
    topeni.loop();
    promini.loop();
    mqttprocess.loop();
  //  pingac.loop();


}