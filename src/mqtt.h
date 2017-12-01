//
// Created by alarm on 12/1/17.
//

#ifndef TOPENICPP_MQTT_H
#define TOPENICPP_MQTT_H



extern int selprofil;
extern float sensorExterier;
extern float sensorInterier;
extern float sensorFloor;
extern float NormalTeplotaDay;
extern float NormalTeplotaNight;
extern float UtlumTeplota;
extern float MaxFloorTeplota;
extern boolean fstate[3];

class Mqtt {
public:
    /**
     * A constructor.
     */

    Mqtt();

    /**
     * A pure virtual member.
     * metoda která nadstavý výchozí parametry programu.
     */
    virtual int isTimer(unsigned long cas);
    virtual void setup(void);

    /**
     * A pure virtual member.
     * výchozí smyčka která se pořád opakuje
     */
    virtual void loop(void);
//    virtual  void scrSaver(void);


private:
    float previousMillis=0;

};

#endif //TOPENICPP_MQTT_H
