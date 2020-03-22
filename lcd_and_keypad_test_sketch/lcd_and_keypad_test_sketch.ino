#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,16,2);

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'C','0','=','/'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {2,3,4,5};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

char pressedChars[16];
byte pressedCharsIndex = 0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
}

void loop() {
  char customKey = customKeypad.getKey();

  if (customKey) {
    pressedChars[pressedCharsIndex] = customKey;
    pressedCharsIndex++;
    Serial.println(pressedChars);
    lcd.setCursor(0,1);
    lcd.print(pressedChars);
  }
}
