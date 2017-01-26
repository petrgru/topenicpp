// read from this https://github.com/UnknownHero99/SolarCollectorController/tree/master/arduino
// https://github.com/26dx/WaterCounter/blob/master/WaterCounter/src/water-counter-0-2-4.ino
// for graphics https://github.com/blinkworth1/Adrian_myController13/blob/master/example/Jonno2/Jonno2.ino
// https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library


//jine menu https://github.com/0xPIT/menu
//#include <MenuSystem.h>
//#include "main.cpp"
class MyRenderer : public MenuComponentRenderer
{
public:
  virtual void render(Menu const& menu) const
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Controller Menu");
    lcd.setCursor(0, 1);
    menu.get_current_component()->render(*this);

  }

  virtual void render_menu_item(MenuItem const& menu_item) const
  {
    lcd.print("*");
    lcd.print(menu_item.get_name());
    lcd.setCursor(0, 2);
    lcd.print(" ");
    lcd.print(menu_item.get_name());

  }

  virtual void render_back_menu_item(BackMenuItem const& menu_item) const
  {
    lcd.print(menu_item.get_name());
  }

  virtual void render_numeric_menu_item(NumericMenuItem const& menu_item) const
  {
    lcd.print(menu_item.get_name());
  }

  virtual void render_menu(Menu const& menu) const
  {
    lcd.print(menu.get_name());
  }
};
MyRenderer my_renderer;

// Menu variables

MenuSystem ms(my_renderer);
//MenuItem espIP("web ui IP", &reqestIp);
Menu settingsMenu("Settings");
Menu lcdSettingsMenu("LCD Settings");
MenuItem brightnessMenu("Set Brightness", &pumpAutoModeLcd);
MenuItem contrastMenu("Set Contrast", &pumpAutoModeLcd);
MenuItem exitMenu("Exit", &exitLcd);
MenuItem backMenu("back", &backLcd);





void menu_setup()
{
  //
  lcd.begin(20,4);
  ms.get_root_menu().add_menu(&settingsMenu);

  settingsMenu.add_menu(&lcdSettingsMenu);
  lcdSettingsMenu.add_item(&brightnessMenu);
  lcdSettingsMenu.add_item(&contrastMenu);
  lcdSettingsMenu.add_item(&backMenu);
  ms.get_root_menu().add_item(&exitMenu);
  lcd.backlight();
}

void pumpAutoModeLcd(MenuItem* p_menu_item) {
  autoMode = true;
  ms.back();
  ms.display();
  lcd.setCursor(0, 0);
  lcd.print("Inmenus: ");

  delay(1500);

}
void exitLcd(MenuItem* p_menu_item)
{
  lcd.clear();
  ms.reset();
  status = 1;
  statuslcd();
}
void backLcd(MenuItem* p_menu_item)
{
  ms.back();
  ms.display();
}

void statuslcd() {
  lcd.backlight();
  lcd.setCursor(0, 0);
   lcd.print("Tc: ");
  lcd.print("mB");
  delay(1500);
}

void menuEntranceHandler() {
  current = digitalRead(encoderSWPin);
  if (current == LOW && previous == HIGH && (millis() - firstTime) > 200) {
    firstTime = millis();
  }

  millisHeld = (millis() - firstTime);
  secsHeld = millisHeld / 1000;

  if (millisHeld > 50) {

    if (current == HIGH && previous == LOW && secsHeld >= 2) {
      status = 0;
      ms.display();
      lastAction = millis();
      delay(200);
    }
  }

    lcd.clear();
    statuslcd();
  previous = current;
  prevSecsHeld = secsHeld;
}

void controlhandler() {
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      lastAction = millis();
      if (sleeping) wake();
      else {
        if (status == 0)
          ms.select();
        ms.display();
      }
    }
  }

  lastButtonState = buttonState;

}

void wake() {
  sleeping = false;
  lcd.backlight();
}

void serialPrintHelp() {
  Serial.println("***************");
  Serial.println("w: go to previus item (up)");
  Serial.println("s: go to next item (down)");
  Serial.println("a: go back (right)");
  Serial.println("d: select \"selected\" item");
  Serial.println("?: print this help");
  Serial.println("h: print this help");
  Serial.println("***************");
}

void serialHandler() {
  char inChar;
  if((inChar = Serial.read())>0) {
    switch (inChar) {
    case 'w': // Previus item
      ms.prev();
      ms.display();
      break;
    case 's': // Next item
      ms.next();
      ms.display();
      break;
    case 'a': // Back presed
      ms.back();
      ms.display();
      break;
    case 'd': // Select presed
      ms.select();
      ms.display();
      break;
    case '?':
    case 'h': // Display help
      serialPrintHelp();
      break;
    default:
      break;
    }
  }
}
