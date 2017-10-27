//
// Created by alarm on 4/3/17.
//

#ifndef TOPENICPP_PINGWATCHDOG_H
#define TOPENICPP_PINGWATCHDOG_H


class ping_watchdog {
public:
    /**
     * A constructor.
     */
    //Ntp_client();

    /**
     * A pure virtual member.
     * metoda která nadstavý výchozí parametry programu.
     */
    virtual int isTimer(void);
    virtual void setup(void);
    virtual void loop(void);
    virtual void ping_test(void);
    /**
     * A pure virtual member.
     * výchozí smyčka která se pořád opakuje
     */


private:

    int loopCounter;
    float currentMillis = millis();
    float previousMillis=0;
    long interval = 10000;
    int sended_ping;
    int received_ping;
    /**
    * a private variable.
    * proměnná která zařizuje druh výpisu na Consoli
    */
};
#endif //TOPENICPP_PINGWATCHDOG_H
