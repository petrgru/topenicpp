//
// Created by petrg on 31.1.17.
//

#ifndef MUJPRVNI_MENU_H
#define MUJPRVNI_MENU_H
extern int selprofil;
class TopeniMenu {
public:
    /**
     * A constructor.
     */
    TopeniMenu();

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
//    virtual  void scrSaver(void);


private:

    int loopCounter;
    unsigned long previousMillis = 0;
    unsigned long timeinmenu = 10000;
    /**
    * a private variable.
    * proměnná která zařizuje druh výpisu na Consoli
    */
};
#endif //MUJPRVNI_MENU_H
