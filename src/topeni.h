//
// Created by petrg on 25.1.17.
//
#define DEBUG True
#ifndef TOPENI_H
#define TOPENI_H
extern int selprofil;
extern float FloarTemp;
extern float AirTemp;
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
    virtual void loop(void);

    /**
     * A pure virtual member.
     * výchozí smyčka která se pořád opakuje
     */


private:

    int loopCounter;
    /**
    * a private variable.
    * proměnná která zařizuje druh výpisu na Consoli
    */
};
#endif