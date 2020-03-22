#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,16,2);

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("SFDU KTbz1-3");
  lcd.setCursor(0,1);
  lcd.print("Evgeny Mohnachev");
}


void loop()
{
  
}
