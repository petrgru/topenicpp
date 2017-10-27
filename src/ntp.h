//
// Created by alarm on 2/20/17.
//

#ifndef TOPENICPP_NTP_H
#define TOPENICPP_NTP_H
//
// Created by petrg on 15.2.17.
//
#define DEBUG True


class Ntp_client {
public:
    /**
     * A constructor.
     */
    //Ntp_client();

    /**
     * A pure virtual member.
     * metoda která nadstavý výchozí parametry programu.
     */
    virtual void setup(void);
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
#endif //TOPENICPP_NTP_H
