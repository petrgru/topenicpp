//
// Created by petrg on 25.1.17.
//
#include <Arduino.h>
#include <HardwareSerial.h>
#include "topeni.h"
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 24
#define TEMPERATURE_PRECISION 9


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress insideThermometer, outsideThermometer;

#define ABLA 60000;


int Topeni::isTimer(){
    float currentMillis=millis();
    if(currentMillis - Topeni::previousMillis > interval) {
        // save the last time you blinked the LED
        Topeni::previousMillis = currentMillis;
        return TRUE;
    }else
        return FALSE;
}

void printAddress(DeviceAddress deviceAddress)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        // zero pad the address if necessary
        if (deviceAddress[i] < 16) Serial.print("0");
        Serial.print(deviceAddress[i], HEX);
    }
}

// function to print the temperature for a device
void savetemptovalue(DeviceAddress deviceAddress,float *temp){
    float tempC = sensors.getTempC(deviceAddress);
    *temp=tempC;
}
void printTemperature(DeviceAddress deviceAddress)
{
    float tempC = sensors.getTempC(deviceAddress);
    Serial.print("Temp C: ");
    Serial.print(tempC);
    Serial.print(" Temp F: ");
    Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
    Serial.print("Resolution: ");
    Serial.print(sensors.getResolution(deviceAddress));
    Serial.println();
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
    Serial.print("Device Address: ");
    printAddress(deviceAddress);
    Serial.print(" ");
    printTemperature(deviceAddress);
    Serial.println();
}

void Normal(void){
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures();
    Serial.println("DONE");

    // print the device information
//    printData(insideThermometer);
//    printData(outsideThermometer);
    savetemptovalue(insideThermometer,&sensorInterier);
    savetemptovalue(outsideThermometer,&sensorExterier);

}

Topeni::Topeni() {
    // button
    loopCounter = 1;

}
void Topeni::setup(){
    sensors.begin();

    // locate devices on the bus
    Serial.print("Locating devices...");
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount(), DEC);
    Serial.println(" devices.");

    // report parasite power requirements
    Serial.print("Parasite power is: ");
    if (sensors.isParasitePowerMode()) Serial.println("ON");
    else Serial.println("OFF");
    if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
    if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Unable to find address for Device 1");

    // method 2: search()
    // search() looks for the next device. Returns 1 if a new address has been
    // returned. A zero might mean that the bus is shorted, there are no devices,
    // or you have already retrieved all of them. It might be a good idea to
    // check the CRC to make sure you didn't get garbage. The order is
    // deterministic. You will always get the same devices in the same order
    //
    // Must be called before search()
    //oneWire.reset_search();
    // assigns the first address found to insideThermometer
    //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");
    // assigns the seconds address found to outsideThermometer
    //if (!oneWire.search(outsideThermometer)) Serial.println("Unable to find address for outsideThermometer");

    // show the addresses we found on the bus
    Serial.print("Device 0 Address: ");
    printAddress(insideThermometer);
    Serial.println();

    Serial.print("Device 1 Address: ");
    printAddress(outsideThermometer);
    Serial.println();

    // set the resolution to 9 bit per device
    sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
    sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);

    Serial.print("Device 0 Resolution: ");
    Serial.print(sensors.getResolution(insideThermometer), DEC);
    Serial.println();

    Serial.print("Device 1 Resolution: ");
    Serial.print(sensors.getResolution(outsideThermometer), DEC);
    Serial.println();
}
void Topeni::loop() {
    if(Topeni::isTimer()){
        switch (selprofil) {
            case 0:
                Serial.println("Normal");
                Normal();
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
