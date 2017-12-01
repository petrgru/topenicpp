//
// Created by alarm on 12/1/17.
//
#include <Ethernet.h>
#include <PubSubClient.h>

#include <stdlib.h>
#include "mqtt.h"


String floatToString(float x, byte precision = 2) {
    char tmp[50];
    dtostrf(x, 0, precision, tmp);
    return String(tmp);
}

void callback(char* topic, byte* payload, unsigned int length) {
    // handle message arrived
}

//MQTT CERDITIONALS
const char* broker = "192.168.22.22";
const int mqtt_port = 1883;
const char *mqtt_user = "tyvole";
const char *mqtt_pass = "tyvole";
const char *mqtt_client_name = "topeni";

EthernetClient ethClient;
PubSubClient mqtt(ethClient);
//Topic instance
//const char* topicLed = "GsmClientTest/led";
const char* topicInit = "/TopeniCholtice";
const char* topicGasValue = "/TopeniCholtice/data";


unsigned long lastReconnectAttemptMqtt = 0;
unsigned long previousMillisMqtt = 0;
const long intervalMqtt = 10000;

void mqttCallback(char* topic, unsigned char* payload, unsigned int len) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.write(payload, len);
    Serial.println();

    // Only proceed if incoming message's topic matches
    /*if (String(topic) == topicLed) {
      ledStatus = !ledStatus;
      digitalWrite(LED_PIN, ledStatus);
      mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
    }*/


}
Mqtt::Mqtt() {

}
int Mqtt::isTimer(unsigned long cas){
    float currentMillis=millis();
    if(currentMillis - Mqtt::previousMillis > cas) {
        // save the last time you blinked the LED
        Mqtt::previousMillis = currentMillis;
        return true;
    }else
        return false;
}

void Mqtt::setup()  {
    // MQTT Broker setu
    mqtt.setServer(broker, mqtt_port);
    mqtt.setCallback(mqttCallback);
    lastReconnectAttemptMqtt = 0;

}

boolean mqttConnect() {
    Serial.print("Connecting to mqtt server");
    if (mqtt.connect(mqtt_client_name,mqtt_user,mqtt_pass)) {
        // Once connected, publish an announcement...
        mqtt.publish(topicInit,"Connected");
        // ... and resubscribe
        mqtt.subscribe("inTopic");
    }

    Serial.println(" OK");

    mqtt.publish(topicInit, "Started");
    //mqtt.subscribe(topicLed);
    return mqtt.connected();
}

void Mqtt::loop() {
    char msg[50];
    char topic[100];
    if (!mqtt.connected()) {
        long now = millis();
        if (now - lastReconnectAttemptMqtt > 5000) {
            lastReconnectAttemptMqtt = now;
            // Attempt to reconnect
            if (mqttConnect()) {
                lastReconnectAttemptMqtt = 0;
            }
        }
    } else {
        if( Mqtt::isTimer(60000)) {
            dtostrf(sensorFloor,2,2,msg);
            snprintf (topic, sizeof(topic), "%s%s", topicInit, "/floor/state");
            mqtt.publish(topic,msg);
            dtostrf(sensorInterier,2,2,msg);
            snprintf (topic, sizeof(topic), "%s%s", topicInit, "/air/state");
            mqtt.publish(topic,msg);
            dtostrf(NormalTeplotaDay,2,2,msg);
            snprintf (topic, sizeof(topic), "%s%s", topicInit, "/NormalTempDay/state");
            mqtt.publish(topic,msg);
            dtostrf(NormalTeplotaNight,2,2,msg);
            snprintf (topic, sizeof(topic), "%s%s", topicInit, "/NormalTempNight/state");
            mqtt.publish(topic,msg);

        }
        // Client connected

        mqtt.loop();
    }
}


//Sending Gas MQTT
void SendGasPpm() {
    unsigned long currentMillis = millis();
    //    int sensor = A0;
    int sensorValue = 0;

    if (currentMillis - previousMillisMqtt >= intervalMqtt) {     // checks if 10 delay is over
        // save last time sensor reading
        previousMillisMqtt = currentMillis;

        //sensorValue = analogRead(sensor);

        int h = sensorValue;
        if (isnan(h)) {
            Serial.println("Failed to read from Gas sensor!");
            //return; This will ensure that data is always sent
        }
        Serial.print("Gas-sensor");
        Serial.print(h);

        mqtt.publish(topicGasValue, "gas" );
    }
}