#include "LCD.h"
#include "LEDmatrix.h"
#include <LiquidCrystal.h>

extern const byte rs;
extern const byte en;
extern const byte d4;
extern const byte d5;
extern const byte d6;
extern const byte d7;
extern LiquidCrystal lcd;


extern char* welcomeText;
extern char* endGameText;
extern byte LCDState; // WELCOME, END_GAME, MENU, PLAYING
extern int currentTextPos;
extern unsigned long lastChangeText;

extern byte wasLCDResetForMessageDisplay;
extern byte currentMenuPosBias;
extern bool wasMenuDisplayed;
extern byte menuSelectingDot[8];
extern byte currentMenuPos;
extern bool joyMoved;

unsigned long lastMenuMoved = 0;
extern byte gameState;
extern byte menuLevel;

void showTextLCD(char* text){
    char space = ' ';
    if(millis() - lastChangeText > TIME_BETWEEN_LCD_LETTERS){
        lastChangeText = millis();
        currentTextPos++;
        if(currentTextPos < strlen(text)){
          lcd.print(text[currentTextPos]);
        }
        else if(currentTextPos >= strlen(text) && currentTextPos <= (strlen(text) + LCD_ROWS)){
          lcd.print(space);
        }
        else{
          stopTextLCD();
          LCDState = MAIN_MENU;
        }
    }
}

void stopTextLCD(){
  wasLCDResetForMessageDisplay = 0;
  lcd.clear();
}


void setLCDForMessageDisplay(){
  lcd.setCursor(LCD_ROWS, 0);
  lcd.autoscroll();
  wasLCDResetForMessageDisplay = 1;
  currentTextPos = -1;
}

void displayMenu(){
  wasMenuDisplayed = 1;
  switch (currentMenuPosBias)
  {
  case 0:
      lcd.setCursor(0,0);
      if(currentMenuPos == 0){
        lcd.write(MENU_SELECTING_DOT);
      }
      else{
        lcd.print(" ");
      }
      lcd.print("Start Game");

      lcd.setCursor(0,1);
      if(currentMenuPos == 1){
        lcd.write(MENU_SELECTING_DOT);
      }
      else{
        lcd.print(" ");
      }
      lcd.print("Settings");
      break;
  
    case 1:
      lcd.setCursor(0,0);
      if(currentMenuPos == 1){
        lcd.write(MENU_SELECTING_DOT);
      }
      else{
        lcd.print(" ");
      }
      lcd.print("Settings");

      lcd.setCursor(0,1);
      if(currentMenuPos == 2){
        lcd.write(MENU_SELECTING_DOT);
      }
      else{
        lcd.print(" ");
      }
      lcd.print("About");
      break;

    default:
      break;
  }
}


void changeMenu() {
  int xValue = analogRead(Y_PIN);

  if (xValue > MAX_THRESHOLD || xValue < MIN_THRESHOLD) {
    if(millis() - lastMenuMoved < MENU_MOVE_INTERVAL){
      return;
    }
    else{
      lastMenuMoved = millis();
    }
  }

  if (xValue > MAX_THRESHOLD) {
    if (currentMenuPos < currentMenuPosBias + LCD_COLS - 1) {
      currentMenuPos++;
      wasMenuDisplayed = 0;

    } 
    else if (currentMenuPosBias + LCD_COLS < MENU_COLS) {
      currentMenuPosBias++;
      currentMenuPos++;
      wasMenuDisplayed = 0;

    }
  }

  if (xValue < MIN_THRESHOLD) {
    if (currentMenuPos > currentMenuPosBias) {
      currentMenuPos--;
      wasMenuDisplayed = 0;

    } 
    else if (currentMenuPosBias > 0) {
      currentMenuPosBias--;
      currentMenuPos = currentMenuPosBias;
      wasMenuDisplayed = 0;
    }
  }
  
}

void resetLCDForGame(){
  lcd.clear();
}