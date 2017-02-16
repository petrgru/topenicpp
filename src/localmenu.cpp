//
// Created by petrg on 31.1.17.
//
#include <menu.h>//menu macros and objects
#include <Wire.h>
#include <LiquidCrystal_I2C.h>//F. Malpartida LCD's driver
#include <menuLCDs.h>
#include <menuFields.h>
#include <menuPrint.h>
#include <quadEncoder.h>//quadrature encoder driver and fake stream
#include <keyStream.h>//keyboard driver and fake stream (for the encoder button)
#include <chainStream.h>// concatenate multiple input streams (this allows adding a button to the encoder)
#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>;
#include "localmenu.h";

byte macAddress[6] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// rotary encoder pins
#define encA    2
#define encB    3
#define encBtn  4

#define LEDPIN 13
#define SPENDINMENU 3000

//LiquidCrystal_I2C lcd(0x27);//, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
LiquidCrystal_I2C lcd(0x26, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address and pinout
extern float sensorExterier=0;
extern float sensorInterier=0;
extern float sensorFloor=0;
//aux vars
int ledCtrl=0;
bool runMenu=false;
bool scrSaverEnter=true;
int percent;//just testing changing this var
double fps=0;
unsigned long lastFpsChk=0;
int counter=0;

///////////////////////////////////////////////////////////////////////////
//functions to wire as menu actions


bool ledOn() {
    Serial.println("set led on!");
    digitalWrite(LEDPIN,ledCtrl=1);
    return false;
}

bool ledOff() {
    Serial.println("set led off!");
    digitalWrite(LEDPIN,ledCtrl=0);
    return false;
}

bool quit() {
    Serial.println("Quiting after action call");
    return true;
}


/////////////////////////////////////////////////////////////////////////
// MENU DEFINITION
// here we define the menu structure and wire actions functions to it
// empty options are just for scroll testing

/*bool setLed() {
  digitalWrite(LEDPIN,ledCtrl);
  return false;
}*/
/*
TOGGLE(ledCtrl,setLed,"Led:",
VALUE("On",HIGH,ledOn),
VALUE("Off",LOW,ledOff)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",
VALUE("Zero",0),
VALUE("One",1),
VALUE("Two",2)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose ",
VALUE("First",1),
VALUE("Second",2),
VALUE("Third",3),
VALUE("Last",-1)
);

MENU(subMenu,"SubMenu"
,OP("A",quit)
,OP("B",quit)
,OP("C",quit)
,OP("D",quit)
,OP("E",quit)
,OP("F",quit)
,OP("G",quit)
,OP("H",quit)
);

MENU(mainMenu,"Main menu",
SUBMENU(setLed),
        OP("LED On",ledOn),
        OP("LED Off",ledOff),
        SUBMENU(selMenu),
        SUBMENU(chooseMenu),
        SUBMENU(subMenu),
        FIELD(percent,"Percent","%",0,100,10,1),
        FIELD(fps,"fps [","]",0,0,0,0),
        FIELD(counter,"counter [","]",0,0,0,0),
        OP("Exit",pauseMenu)
);*/


bool saveprofil(){
    Serial.println(selprofil);
    Serial.println(" uloz");
    EEPROM.update(0,selprofil);
    return false;
}




TOGGLE(selprofil,selProfil,"Select",
       VALUE("Normal",0,saveprofil),
       VALUE("Solar ",1,saveprofil),
       VALUE("Utlum ",2,saveprofil)
);


int selteploty;
extern float NormalTeplotaDay;
extern float NormalTeplotaNight;
extern float UtlumTeplota;
bool saveteplota(){
    Serial.println(NormalTeplotaDay);
    EEPROM.update(1,NormalTeplotaDay);
    EEPROM.update(2,NormalTeplotaNight);
    EEPROM.update(3,UtlumTeplota);
    return false;
}




//the quadEncoder
#define ENC_SENSIVITY 4
quadEncoder quadEncoder(encA,encB);//simple quad encoder driver
quadEncoderStream enc(quadEncoder,ENC_SENSIVITY);// simple quad encoder fake Stream

//a keyboard with only one key :D, this is the encoder button
keyMap encBtn_map[]={{-encBtn,menu::enterCode}};//negative pin numbers means we have a pull-up, this is on when low
keyLook<1> encButton(encBtn_map);

//alternative to previous but now we can input from Serial too...
Stream* in3[]={&enc,&encButton,&Serial};
chainStream<3> allIn(in3);

EthernetServer server(23);

//describing a menu output, alternatives so far are Serial or LiquidCrystal LCD
menuLCD menu_lcd(lcd,20,4);//menu output device
menuPrint menu_out_ser(Serial);
menuPrint menu_out_net(server);

void scrSaver() {
    if (scrSaverEnter) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Mode: ");
        switch (selprofil){
            case 0: lcd.print("Normal");
                break;
            case 1: lcd.print("Solar");
                break;
            case 2: lcd.print("Utlum");
                break;
        }
        lcd.setCursor(0,1);
        lcd.print("-------");
//        scrSaverEnter=false;
        menu_lcd.redraw();
    }
}
bool pauseMenu() {
    runMenu = false;
    scrSaverEnter = true;
    scrSaver();
}


MENU(selTeplota,"Teploty",
     FIELD(NormalTeplotaDay,  "NormalDay  ","C",0,23,10,0,saveteplota),
     FIELD(NormalTeplotaNight,"NormalNight","C",0,23,10,0,saveteplota),
     FIELD(UtlumTeplota,      "Utlum      ","C",0,23,10,0,saveteplota),
     FIELD(sensorInterier,    "Uvnitr     "," C"),
     FIELD(sensorExterier,    "Venku      "," C"),
     FIELD(sensorFloor,       "Podlaha    "," C"),
     OP("Exit",mainMenu)
);

MENU(mainMenu,"Main menu",
     SUBMENU(selProfil),
     SUBMENU(selTeplota),
     OP("Exit",pauseMenu)
);

TopeniMenu::TopeniMenu() {
    // button
    loopCounter = 1;
}




void TopeniMenu::setup() {
    pinMode(LEDPIN,OUTPUT);
    pinMode(encBtn,INPUT_PULLUP);
    Serial.begin(115200);
    while(!Serial);
    Wire.begin();
    quadEncoder.begin();
    lcd.begin(20,4);
    lcd.home();
    menu::wrapMenus=true;
    mainMenu.data[0]->enabled=false;
    mainMenu[7].disable();
    mainMenu[8].disable();
    if (Ethernet.begin(macAddress))
        Serial.println(Ethernet.localIP());
    else
        Serial.println("failed");
    server.begin();
    Serial.println("Server started");
}
void TopeniMenu::loop() {
    unsigned long currentMillis = millis();
    char ch;
    Ethernet.maintain();
    EthernetClient client = server.available();
    if (client.connected()){
        mainMenu.poll(menu_out_net,client);
    }
    if (runMenu) {
        mainMenu.poll(menu_lcd,allIn);
        mainMenu.poll(menu_out_ser,Serial);
    }
    else if (allIn.available())
    {
        ch=allIn.read();
        if (ch==menu::enterCode) {
            runMenu=true;
        }
    }
    else {
        scrSaverEnter=true;
        runMenu=false;
        scrSaver();
    }
//    if (allIn.read()) previousMillis=currentMillis;
/*    if ((currentMillis - previousMillis > SPENDINMENU)) {
        previousMillis = currentMillis;
        //pauseMenu();
        scrSaver();
        menu_lcd.redraw();
    }
*/
    //simulate the delay of your program... if this number rises too much the menu will have bad navigation experience
    //if so, then the menu can be wired into a timmer... leaving the shorter end to your code while it is running
    //if (allIn.available()) {Serial.println("Key");}
    counter=millis()/1000%60;
    int d=micros()-lastFpsChk;
    if (d>0) {
        fps=1000000.0/d;
        lastFpsChk+=d;
    }
    delay(50);

}

