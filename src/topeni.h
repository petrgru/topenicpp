//
// Created by petrg on 25.1.17.
//
#define DEBUG True
#ifndef TOPENI_H
#define TOPENI_H
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
    virtual void setup(void);

    /**
     * A pure virtual member.
     * výchozí smyčka která se pořád opakuje
     */
    virtual void loop(void);


private:

    int loopCounter;
    /**
    * a private variable.
    * proměnná která zařizuje druh výpisu na Consoli
    */
};
#endif