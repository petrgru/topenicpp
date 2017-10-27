//
// Created by petrg on 25.1.17.
//
#include <Arduino.h>
#include <HardwareSerial.h>
#include "topeni.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Time.h>
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 24
#define TEMPERATURE_PRECISION 9
#define HYSTERESE 0.5

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

// TODO
// zde bude rozhodovani casu
//
boolean is_day(){

    return TRUE;
}
boolean is_lower_price(){
    int n;
    boolean istrue = FALSE;
    static int timelow[]={1,2,3,4,5,7,8,10,11,12,14,15,17,18,20,21,22};
    for(n=0;n < sizeof(timelow);n++){
        if (timelow[n] == hour()){
            istrue=TRUE;
        }
    }
    if(istrue) Serial.println("Je nizky tarif");
    else Serial.println("Je vysoky tarif");
    return istrue;
}
boolean compare(boolean a[],boolean b[])
{
    int i,size=sizeof(a);
    for(i=0;i<size;i++)
    {
            if(a[i]!=b[i])
            {
                return FALSE;
            }
    }
    return TRUE;
};
void heating_fase(int f){
    unsigned int i,a=0;
    for (i=0;i<3;i++){
        a=2;
        if (compare(fstate,solarstate[f][i])){
            a=i;
            break;
        }

    }
    Serial.println(a);

    if (a == 2){
        a=0;
    } else {
        a++;
    }

    for (i = 0; i < 3; i++) {
        fstate[i]= solarstate[f][a][i];

        Serial.print(fstate[i]);
        Serial.print(" ");
    }
    Serial.println();
/*    for (i = 0; i < sizeof(srelay); i++) {
        digitalWrite(srelay[a][i], state);
    }
*/


}
void solar(int state){
    if (solar_power < 500){
        heating_fase(0);
    }else if(solar_power<1000){
        heating_fase(1);
    }else if(solar_power<1500){
        heating_fase(2);
    }else{
        heating_fase(3);
    }
}
boolean runtemperaturepin(float teplota, float settemp) {

    float temp_offset = 0.3; //hystereze
    if (fstate[0] || fstate[1]||fstate[2]) { //topeni je zapnuto
        if(sensorFloor > MaxFloorTeplota) return FALSE;
        else{
            if (teplota > settemp) {
                return FALSE;
            }
            else return TRUE;
        }
    } else {
        Serial.println("Kontrola hystereze");
        if ((MaxFloorTeplota - sensorFloor) > temp_offset)
            if ((settemp - teplota) > temp_offset) return TRUE;
    }
    return FALSE;
}
void normalstate(boolean state){
    int i,f;
    for (i = 0; i < sizeof(fstate); i++) {
        fstate[i]= state;
    }
    for (i = 0; i < 3; i++) {
        for (f = 0; f < 3; f++) {
            digitalWrite(srelay[i][f], !state);
            delay(20);
        }
    }

}
void pinonoff() {
    boolean state;
    unsigned int i = 0,f=0;
    switch (selprofil) {
        case 0:
            if (is_lower_price()){
                if (is_day())
                    state=runtemperaturepin(sensorInterier,NormalTeplotaDay);
                else
                    state =runtemperaturepin(sensorInterier,NormalTeplotaNight);
                normalstate(state);
            }
            else normalstate(FALSE);
            break;
        case 1:
            solar(state);
            break;
        case 2:
            state=runtemperaturepin(sensorInterier,UtlumTeplota);
            normalstate(state);
            break;
    }
}

//
// vlastni sepnuti topeni a hystereze
//


void kontrola_topeni(void) {
    sensors.requestTemperatures();
    savetemptovalue(insideThermometer, &sensorInterier);
    savetemptovalue(outsideThermometer, &sensorFloor);
    sensorExterier=sensorFloor;


//    if (isDay())
//        runtopeni(true,true,true);
}

Topeni::Topeni() {
    // button
//    loopCounter = 1;

}
void Topeni::setup() {
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
    if (Topeni::isTimer()) {
        kontrola_topeni();
        if(sensorInterier > 40 || sensorInterier < -5) delay(10000);
        if( sensorFloor> 40 || sensorFloor < -5) delay(10000);
        pinonoff();
    }
}

