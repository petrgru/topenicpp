// all Serial.print's are using F("") to reduce SRAM Size

// Libraries ------------------------------------------------------
#include <TimeLib.h>
//#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <Timezone.h>
#include <SD.h>
#include "ntp.h"

// Pin Definitions for Arduino Ethernet Shield with W5100-bug. ----------------------------------------
// activate_ethernet() or activate_sdcard() depending on what will be used. Both cannot be active at the same time
#define SS_SD_CARD   4    // Set Pin to LOW in order to activate SD-Card on SPI and SS_ETHERNET to HIGH
#define SS_ETHERNET 10    // Set Pin to LOW in order to activate Ethernet on SPI and SS_SD_CARD to HIGH

// Timezone Definition --------------------------------------------
//Central European Time (Switzerland, Germany, France) --> see Timezone Library for further informations (https://github.com/JChristensen/Timezone)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time - begin of DST {abbrev, week, dow, month, hour, offset} -> last Sunday in March, at 2 o'clock adding 2 hours to UTC
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time - end of DST {abbrev, week, dow, month, hour, offset} -> last Sunday in October, at 3 o'clock adding 1 hours to UTC
Timezone CE(CEST, CET);                                     //set Timezone CE using definitions CEST and CET
TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev

time_t prevDisplay = 0; // when the digital clock was displayed
time_t local;           // store current time from NTP converted to timezone
String DST = "";        // string to hold if DST is active or not
String ClockSync = "";  // string to hold if Clock is synchronized

// IP of NTP Servers:
// use any ntp-server. see http://www.pool.ntp.org/zone/@ for any server in your region - Remember NTP-Time is always UTC.
// Use IP Address of the given NTP-Server - find IP using any WHOIS Website
IPAddress timeServer(195,113,144,201);
//IPAddress timeServer(192,168,1,1);

const int NtpUpdateInterval = 30; // Update Interval for NTP in seconds -> don't update too often to not bother NTP-Servers admin

EthernetUDP Udp;                  // definition of shortcut for EthernetUDP from here on use Udp. ...
unsigned int localPort = 8888;    // local port to listen for UDP packets

/*-------- NTP code ----------*/
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123); //NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}


// when getNtpTime() starts ethernet() will be activatet. at the end sdcard will be activated
time_t getNtpTime()
{

    Serial.println(F("Starting to get NTP Time"));
    while (Udp.parsePacket() > 0) ; // discard any previously received packets
    Serial.println(F("Transmit NTP Request"));
    sendNTPpacket(timeServer);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
        int size = Udp.parsePacket();
        if (size >= NTP_PACKET_SIZE) {
            Serial.println(F("Receive NTP Response"));
            Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
            unsigned long secsSince1900;
            // convert four bytes starting at location 40 to a long integer
            secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];
            local = CE.toLocal((secsSince1900 - 2208988800UL), &tcr);
            setTime(local); // set internal clock to local time (based on timezone-library and definition at the top
            return local;
        }
    }
    Serial.println(F("No NTP Response :-("));
    return 0; // return 0 if unable to get the time
}


void printDigits(int digits){
    // utility for digital clock display: prints preceding colon and leading 0
    Serial.print(F(":"));
    if(digits < 10)
        Serial.print(F("0"));
    Serial.print(digits);
}


void digitalClockDisplay(){
    // digital clock display of the time
/*    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(F(" "));
    Serial.print(dayStr(weekday()));
    Serial.print(F(", "));
    Serial.print(day());
    Serial.print(F(". "));
    Serial.print(monthStr(month()));
    Serial.print(F(" "));
    Serial.print(year());
    Serial.print(F(" DST: "));
    Serial.print(DST);
    Serial.print(F(", Clock: "));
    Serial.print(ClockSync);
    Serial.println();
*/}


// Setup Routine ----------------------------------------------------
void Ntp_client::setup()
{
    Udp.begin(localPort);
    setSyncProvider(getNtpTime);
    setSyncInterval(NtpUpdateInterval);
}

void Ntp_client::loop()

{
    if (timeStatus() != timeNotSet)
    {
        if (now() != prevDisplay)
        {
            //update the display only if time has changed
            prevDisplay = now();
            digitalClockDisplay();
            switch (timeStatus())
            { case timeNotSet:
                    ClockSync = "not set";
                    break;
                case timeNeedsSync:
                    ClockSync = "needs sync";
                    break;
                case timeSet:
                    ClockSync = "set";
                    break;
                default:
                    ClockSync = "TimeStatus() not responding to Enums";
            }
            switch (CE.locIsDST(local))
            {  case true: DST = "Yes"; break;
                case false: DST = "No"; break;
                default: DST = "N/A"; break;
            }
        }
    }
}

