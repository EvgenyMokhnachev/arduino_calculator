//#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

const byte lcdWitdh = 16;
const byte lcdHeight = 2;
const byte lcdChars = lcdWitdh * lcdHeight;
LiquidCrystal_I2C lcd(0x3F, lcdWitdh, lcdHeight);

const byte ROWS = 4;
const byte COLS = 5;
const char division = '/';
const char multiply = '*';
const char difference = '-';
const char summ = '+';
const char comma = '.';
const char clearBtn = 'C';
const char backspace = 'B';
const char leftBracket = '(';
const char rightBracket = ')';
const char equal = '=';
char hexaKeys[ROWS][COLS] = {
  {division, multiply, difference, summ, backspace},
  {comma, '7', '4', '1', leftBracket},
  {'0', '8', '5', '2', rightBracket},
  {equal, '9', '6', '3', clearBtn}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {6, 7, 8, 9, 10};
Keypad keyboard = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//String inputData = "((4/2)+(4*2)-(4+2)*(4-2))/2";// = -1
//String inputData = "4/2+4*2-4+2*4-2/2";// = 13
//String inputData = "14-1";// = 13
String inputData = "";
String showData = "";

void setup() {
//  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  render();
}

void loop()
{
  char pressedKey = keyboard.getKey();
  if (pressedKey) {
    switch(pressedKey) {
      case backspace: {
        if (inputData.length() > 0) {
          inputData = inputData.substring(0, inputData.length() - 1);
        }
        break;
      }

      case clearBtn: {
        inputData = "";
        break;
      }
      
      case comma: {
        int specSymbolLastIndex = lastCharIndexOf(inputData, "/*-+()");
        int lastCommaIndex = lastIndexOf(inputData, comma);
        if (lastCommaIndex > specSymbolLastIndex || specSymbolLastIndex == (inputData.length() - 1)) {
          return;
        }
        inputData += pressedKey;
        break;
      }

      case division:
      case multiply:
      case difference:
      case summ: {
        char specSymbols[] = {division, multiply, difference, summ, leftBracket};
        int specSymbolLastIndex = lastCharIndexOf(inputData, "/*-+(");
        if (inputData.length() == 0 || specSymbolLastIndex == (inputData.length() - 1)) {
          return;
        }
        inputData += pressedKey;
        break;
      }

      case leftBracket: {
        if (inputData.length() > 0 && inputData[inputData.length() - 1] == rightBracket) {
          return;
        }
        inputData += pressedKey;
        break;
      }

      case rightBracket: {
        int leftBracketsCount = howMuchChars(inputData, leftBracket);
        int rightBracketsCount = howMuchChars(inputData, rightBracket);
        int specSymbolLastIndex = lastCharIndexOf(inputData, "/*-+(");
        if (leftBracketsCount <= rightBracketsCount || specSymbolLastIndex == (inputData.length() - 1)) {
          return;
        }
        inputData += pressedKey;
        break;
      }

      case equal: {
        inputData = parserExpressions(inputData);
        break;
      }

      default: {
        inputData += pressedKey;
      }
    }

    render();
  }
  
}

String parserExpressions(String expressions) {
  int firstRightBracketIndex = indexOf(expressions, rightBracket, 0);
  int lastLeftBracketIndex = lastIndexOf(expressions.substring(0, firstRightBracketIndex), leftBracket);
  String leftExpression = lastLeftBracketIndex == -1 ? "" : expressions.substring(0, lastLeftBracketIndex);
  String rightExpression = firstRightBracketIndex == -1 ? "" : expressions.substring(firstRightBracketIndex + 1, expressions.length());
  String expression = expressions.substring(firstRightBracketIndex + 1, lastLeftBracketIndex);
  String expressionReult = parseExpression(expression);
  String newExpressions = leftExpression + expressionReult + rightExpression;
  return newExpressions;
}

String parseExpression(String expr) {
  String result = trimBrackets(expr);
  int divOrMultIndex = charIndexOf(expr, "/*");
  if (divOrMultIndex > -1) {
    result = parseAction(result, expr[divOrMultIndex]);
  } else {
    int summOrDiffIndex = charIndexOf(expr, "+-");
    if (summOrDiffIndex > -1) {
      result = parseAction(result, expr[summOrDiffIndex]);
    }
  }
  return result == expr ? result : parseExpression(result);
}

String parseAction(String expr, char action) {
  int actionIndex = indexOf(expr, action, 0);
  if (actionIndex != -1) {
    int beforeSpecIndex = lastCharIndexOf(expr.substring(0, actionIndex), "-+/*");
    int afterSpecIndex = charIndexOf(expr.substring(actionIndex + 1, expr.length()), "-+/*");
    int beforeNumberIndex = beforeSpecIndex == -1 ? 0 : beforeSpecIndex + 1;
    int afterNumberEndIndex = afterSpecIndex == -1 ? expr.length() : actionIndex + 1 + afterSpecIndex;
    String beforeNumber = expr.substring(beforeNumberIndex, actionIndex);
    String afterNumber = expr.substring(actionIndex + 1, afterNumberEndIndex);
    float beforeNumberFloat = beforeNumber.toFloat();
    float afterNumberFloat = afterNumber.toFloat();
    float resultFloat;
    switch(action) {
      case (division): {
        resultFloat = beforeNumberFloat / afterNumberFloat;
        break;
      }
      case (multiply): {
        resultFloat = beforeNumberFloat * afterNumberFloat;
        break;
      }
      case (summ): {
        resultFloat = beforeNumberFloat + afterNumberFloat;
        break;
      }
      case (difference): {
        resultFloat = beforeNumberFloat - afterNumberFloat;
        break;
      }
    }
    String resultString = trimZeros(String(resultFloat));
    String leftExpression = expr.substring(0, beforeNumberIndex);
    String rightExpression = expr.substring(afterNumberEndIndex, expr.length());
    return leftExpression + resultString + rightExpression;
  }
  return expr;
}

//расширить функцию, что бы она (2)/(2) не обрезала до 2)/(2,
//но в тоже время ((2)/(2)) обрезала до (2)/(2)
String trimBrackets(String expr) {
  String result = expr;
  
  int leftBracketIn = indexOf(expr, leftBracket, 0);
  if (leftBracketIn == 0) {
    result = expr.substring(1, expr.length());}

  int rightBracketIn = lastIndexOf(result, rightBracket);
  if (rightBracketIn == result.length() - 1) {
    result = result.substring(0, result.length() - 1);}
  
  return result;
}

String trimZeros(String str) {
  int commaIndex = indexOf(str, comma, 0 );
  if (commaIndex > -1) {
    String afterComma = str.substring(commaIndex, str.length());
    char afterCommaLastChar = afterComma[afterComma.length() - 1];
    if (afterCommaLastChar == '0' || afterCommaLastChar == '.') {
      return trimZeros(str.substring(0, str.length() - 1));
    }
  }
  return str;
}

int indexOf(String str, char x, int from) { 
  String fromStr = str.substring(from, str.length());
  for (int i = 0; i < fromStr.length(); i++) {
    if (fromStr[i] == x) {
      return from + i; 
    }
  }
  return -1; 
}

int lastIndexOf(String str, char x) { 
  int index = -1;
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == x) {
      index = i; 
    }
  }
  return index; 
}

int lastCharIndexOf(String str, String chars) {
  int charLastIndex = -1;
  for (int charIndex = 0; charIndex < chars.length(); charIndex++) {
    int currentCharLastIndex = lastIndexOf(str, chars[charIndex]);
    if (currentCharLastIndex > charLastIndex) {
      charLastIndex = currentCharLastIndex;
    }
  }
  return charLastIndex;
}

int charIndexOf(String str, String chars) {
  int resultCharIndex = -1;
  for (int charIndex = 0; charIndex < chars.length(); charIndex++) {
    int currentCharIndex = indexOf(str, chars[charIndex], 0);
    if ((resultCharIndex == -1 || resultCharIndex > currentCharIndex) && currentCharIndex != -1) {
      resultCharIndex = currentCharIndex;
    }
  }
  return resultCharIndex;
}

int howMuchChars(String str, char ch) { 
  return howMuchChars(str, ch, 0);
}

int howMuchChars(String str, char ch, int result) { 
  int charIndex = lastIndexOf(str, ch);
  if (charIndex > -1) {
    result++;
    return howMuchChars(str.substring(0, charIndex), ch, result);
  } else {
    return result;
  }
}

void render() {
  if (inputData.length() > lcdChars) {
    showData = inputData.substring(inputData.length() - lcdChars, inputData.length());
  } else {
    showData = inputData;
  }

  lcd.clear();
  for (byte i = 0; i < showData.length(); i++) {
    if (i < lcdWitdh) {
      lcd.setCursor(i,0);
    } else {
      lcd.setCursor(i - lcdWitdh ,1);
    }
    lcd.print(showData[i]);
  }
}
