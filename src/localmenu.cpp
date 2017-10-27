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
#include <genericKeyboard.h>
#include <EEPROM.h>
#include <string.h>
#include <SPI.h>
#include <Ethernet.h>;
#include <EthernetUdp.h>
#include "localmenu.h";
#define btnRIGHT  menu::enterCode  // Constants recognized by the ArduinoMenu library
#define btnUP     menu::downCode
#define btnDOWN   menu::upCode
#define btnLEFT   menu::escCode
#define btnENTER  menu::enterCode
#define btnNONE   -1

#include <Button.h>        //https://github.com/JChristensen/Button

#define BUTTON_PIN1 26       //Connect a tactile button switch (or something similar)
#define BUTTON_PIN2 28       //Connect a tactile button switch (or something similar)
#define BUTTON_PIN3 30       //Connect a tactile button switch (or something similar)
#define BUTTON_PIN4 32       //Connect a tactile button switch (or something similar)
#define BUTTON_SV1 22
#define BUTTON_SV2 50
#define RELE_SV1 38
#define RELE_SV2 36
//int pinsv1=38;
//int pinsv2=36;
//int pinvyp1=22;
//int pinvyp2=50;
//from Arduino pin 2 to ground.
#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high unless the
//switch is closed, this is negative logic, i.e. a high state
//means the button is NOT pressed. (Assuming a normally open switch.)
#define DEBOUNCE_MS 20     //A debounce time of 20 milliseconds usually works well for tactile button switches.
#define LONG_PRESS 1000    //We define a "long press" to be 1000 milliseconds.

Button myBtn1(BUTTON_PIN1, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button myBtn2(BUTTON_PIN2, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button myBtn3(BUTTON_PIN3, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button myBtn4(BUTTON_PIN4, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button

Button Btnsv1(BUTTON_SV1, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button
Button Btnsv2(BUTTON_SV2, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the button

byte macAddress[6] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// rotary encoder pins
#define encA    2
#define encB    3
#define encBtn  4

#define LEDPIN 13
#define SPENDINMENU 60000

byte fullfill[8] = {
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
};
byte emptyfill[8] = {
        B11111,
        B10001,
        B10001,
        B10001,
        B10001,
        B10001,
        B11111,
};
//LiquidCrystal_I2C lcd(0x27);//, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
LiquidCrystal_I2C lcd(0x26, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address and pinout
extern int backlight;
unsigned long backlight_time;
//aux vars
int ledCtrl=0;
bool runMenu=false;
bool scrSaverEnter=true;
int percent;//just testing changing this var
double fps=0;
unsigned long lastFpsChk=0;
int counter=0;

int stavsv1=1;
int stavsv2=1;


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

bool saveprofil(){
    Serial.println(selprofil);
    Serial.println(" uloz");
    EEPROM.update(0,selprofil);
    return false;
}

int selteploty;

bool saveteplota(){
    Serial.println(NormalTeplotaDay);
    EEPROM.update(1,NormalTeplotaDay);
    EEPROM.update(2,NormalTeplotaNight);
    EEPROM.update(3,UtlumTeplota);
    EEPROM.update(4,MaxFloorTeplota);
    return false;
}















//the quadEncoder
#define ENC_SENSIVITY 4
quadEncoder quadEncoder(encA,encB);//simple quad encoder driver
quadEncoderStream enc(quadEncoder,ENC_SENSIVITY);// simple quad encoder fake Stream

//a keyboard with only one key :D, this is the encoder button
keyMap encBtn_map[]={{-encBtn,menu::enterCode}};//negative pin numbers means we have a pull-up, this is on when low
keyLook<1> encButton(encBtn_map);
void lcdbacklight(){
    if(backlight==0){
//        lcd.display();
        lcd.backlight();
        backlight=1;
    }
    backlight_time=millis();
}
void svetla(){
    Btnsv1.read();
    Btnsv2.read();
    if(Btnsv1.wasReleased())
    {
        stavsv1=!stavsv1;
        digitalWrite(RELE_SV1,stavsv1);

    }
    if(Btnsv2.wasReleased())
    {
        stavsv2=!stavsv2;
        digitalWrite(RELE_SV2,stavsv2);
    }

}

int button_press(){
    svetla();
    myBtn1.read();
    myBtn2.read();
    myBtn3.read();
    myBtn4.read();
    if (myBtn1.wasReleased()) {
//        Serial.println("btnLEFT");
        lcdbacklight();
        return btnLEFT;
    }
    if (myBtn2.wasReleased()) {
        lcdbacklight();
        return btnDOWN;
    }
    if (myBtn3.wasReleased()) {
        lcdbacklight();
        return btnUP;
    }
    if (myBtn4.wasReleased()) {
        lcdbacklight();
        return btnENTER;
    }

    return btnNONE;
}


genericKeyboard mykb(button_press);
//alternative to previous but now we can input from Serial too...
Stream* in3[]={&mykb,&enc,&encButton,&Serial};
chainStream<4> allIn(in3);

EthernetServer server(23);


//describing a menu output, alternatives so far are Serial or LiquidCrystal LCD
menuLCD menu_lcd(lcd,20,4);//menu output device
menuPrint menu_out_ser(Serial);
menuPrint menu_out_net(server);
char *subString(char *someString, int n)
{
    char *outp = malloc(sizeof(char)*n+1);
    strncpy(outp, someString, n);
    outp[n] = '\0';
    return outp;
}




void scrSaver() {
    unsigned int i;
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
        lcd.setCursor(17,0);
//        lcd.print("   ");
//        lcd.setCursor(17,0);
//        lcd.createChar(0,fullfill);
//        lcd.createChar(1,emptyfill);
        for (i = 0; i < sizeof(fstate); i++)
            lcd.write((uint8_t)fstate[i]);
        lcd.setCursor(0,1);
        lcd.print("                    ");
        lcd.setCursor(0,1);
        lcd.print("In: ");
        lcd.print(sensorInterier);
        lcd.print(" Fl: ");
        lcd.print(sensorExterier);
        lcd.setCursor(0,2);
        lcd.print("SD: ");
        lcd.print(NormalTeplotaDay);
        lcd.print(" SN: ");
        lcd.print(NormalTeplotaNight);
        lcd.setCursor(0,3);
        lcd.print(hour());
        lcd.print(":");
        lcd.print(minute());
        lcd.print(" ");
        lcd.print(day());
        lcd.print("/");
        lcd.print(month());
        lcd.print("/");
        lcd.print(year());
        lcd.print(" ");
        lcd.print(subString(dayStr(weekday()),3));


        scrSaverEnter=false;
        //
    }
    else{
        menu_lcd.redraw();
    }

}

bool pauseMenu() {
    runMenu = false;
    scrSaverEnter = true;
    scrSaver();
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

TOGGLE(selprofil,selProfil,"Select",
       VALUE("Normal",0,saveprofil),
       VALUE("Solar ",1,saveprofil),
       VALUE("Utlum ",2,saveprofil)
);

MENU(selTeplota,"Teploty",
     FIELD(NormalTeplotaDay,  "NormalDay  ","C",12,24,1,0,saveteplota),
     FIELD(NormalTeplotaNight,"NormalNight","C",12,20,1,0,saveteplota),
     FIELD(UtlumTeplota,      "Utlum      ","C",5,20,1,0,saveteplota),
     FIELD(MaxFloorTeplota,   "Max floor  ","C",5,25,1,0,saveteplota)
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
    lcd.createChar(1, fullfill);
    lcd.createChar(0, emptyfill);

    pinMode(RELE_SV1, OUTPUT);
    digitalWrite(RELE_SV1,stavsv2);
    pinMode(RELE_SV2, OUTPUT);
    digitalWrite(RELE_SV2,stavsv1);

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
        menu_lcd.redraw();
    }
    else {
        scrSaverEnter=true;
        runMenu=false;
        scrSaver();
    }
    if (allIn.available() && (currentMillis - previousMillis > SPENDINMENU)) {
        previousMillis = currentMillis;
        mainMenu.canExit=true;
        scrSaverEnter = true;
        runMenu = false;
        allIn.flush();
        scrSaver();
    }
    if (runMenu==false && (currentMillis - previousMillis > SPENDINMENU)){
        previousMillis = currentMillis;
        scrSaverEnter = true;
        scrSaver();
    }

    //simulate the delay of your program... if this number rises too much the menu will have bad navigation experience
    //if so, then the menu can be wired into a timmer... leaving the shorter end to your code while it is running
    //if (allIn.available()) {Serial.println("Key");}
    counter=millis()/1000%60;
    int d=micros()-lastFpsChk;
    if (d>0) {
        fps=1000000.0/d;
        lastFpsChk+=d;
    }
    if ((millis()-backlight_time > 30000) && backlight==1){
        lcd.noBacklight();
        //lcd.noDisplay();
        backlight=0;
    }
//    delay(50);


}

