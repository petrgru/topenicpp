//
// Created by alarm on 2/4/17.
//

#ifndef TOPENICPP_PROMINIEXTENDER_H
#define TOPENICPP_PROMINIEXTENDER_H
class prominiextender {
public:
    /**
     * A constructor.
     */
    prominiextender();

    virtual void setup(void);
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
#endif //TOPENICPP_PROMINIEXTENDER_H
