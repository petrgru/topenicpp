//
// Created by petrg on 25.1.17.
//
#define DEBUG True
#ifndef TOPENI_H
#define TOPENI_H
#define PIN_FAZE_1 {1,2,3};
#define PIN_FAZE_2 {1,2,3};
#define PIN_FAZE_3 {1,2,3};

extern int selprofil;
extern float sensorExterier;
extern float sensorInterier;
extern float sensorFloor;
extern boolean fstate[3];
extern boolean solarstate[4][3][3];
extern int srelay[3][3];
extern int solar_power;

extern float NormalTeplotaDay;
extern float NormalTeplotaNight;
extern float UtlumTeplota;
extern float MaxFloorTeplota;
class Topeni {
public:
    /**
     * A constructor.
     */
    Topeni();

    /**
     * A pure virtual member.
     * metoda která nadstavý výchozí parametry programu.
     */
     virtual int isTimer(void);
    virtual void setup(void);
    virtual void loop(void);

    /**
     * A pure virtual member.
     * výchozí smyčka která se pořád opakuje
     */


private:

    int loopCounter;
    float currentMillis = millis();
    float previousMillis=0;
    long interval = 10000;
    float previousTempI;
    float previousTempF;

    /**
    * a private variable.
    * proměnná která zařizuje druh výpisu na Consoli
    */
};
#endif