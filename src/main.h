//
// Created by alarm on 2/4/17.
//

#ifndef TOPENICPP_MAIN_H
#define TOPENICPP_MAIN_H

int selprofil=0;
float NormalTeplotaDay=18;
float NormalTeplotaNight=16;
float UtlumTeplota=5;
float MaxFloorTeplota=17;
float sensorExterier=0;
float sensorInterier=0;
float sensorFloor=0;
int backlight=1;
boolean fstate[3]={0,0,0};
boolean solarstate[4][3][3] = {
        {{0,0,0},{0,0,0},{0,0,0}},
        {{1,0,0},{0,1,0},{0,0,1}},
        {{1,1,0},{0,1,1},{1,0,1}},
        {{1,1,1},{1,1,1},{1,1,1}}};
int srelay[3][3]={
        {40,43,42},
        {34,49,47},
        {44,46,45}
};
/*int srelay[3][3]={
        {40,43,42},
        {34,49,47},
        {44,46,45}
};*/

int solar_power=1001;
#endif //TOPENICPP_MAIN_H
