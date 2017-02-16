//
// Created by petrg on 25.1.17.
//
#define DEBUG True
#ifndef TOPENI_H
#define TOPENI_H
extern int selprofil;
extern float sensorExterier;
extern float sensorInterier;
extern float sensorFloor;
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
    long interval = 60000;
    /**
    * a private variable.
    * proměnná která zařizuje druh výpisu na Consoli
    */
};
#endif