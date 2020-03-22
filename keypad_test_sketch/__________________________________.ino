#include <Keypad.h>

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

void setup() {
  Serial.begin(9600);
}

void loop() {
  char customKey = customKeypad.getKey();

  if (customKey) {
    Serial.println(customKey);
  }
}
