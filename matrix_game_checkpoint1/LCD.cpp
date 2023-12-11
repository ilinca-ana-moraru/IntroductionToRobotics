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

extern LedControl lc;

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
extern byte mainMenuPick;
extern bool wasAboutTextPrinted;
unsigned long lastScrollChange = 0;
extern byte secondMenuPick;

extern byte matrixBrightness;
unsigned long lastBrightnessChange = 0;

extern byte matrixBrightness;
extern byte lcdBrightness;
extern byte lcdScaledBrighness;

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
  wasLCDResetForMessageDisplay = false;
  lcd.clear();
}


void setLCDForMessageDisplay(){
  lcd.setCursor(LCD_ROWS, 0);
  lcd.autoscroll();
  wasLCDResetForMessageDisplay = true;
  currentTextPos = -1;
}

void displayMenu(){
  wasMenuDisplayed = true;
  if(menuLevel == FIRST_MENU){
    switch (currentMenuPosBias){
      case FIRST_COLUMN:
        lcd.setCursor(0,0);
        if(currentMenuPos == FIRST_COLUMN){
          lcd.write(MENU_SELECTING_DOT);
        }
        else{
          lcd.print(" ");
        }
        lcd.print("Start Game");

        lcd.setCursor(0,1);
        if(currentMenuPos == SECOND_COLUMN){
          lcd.write(MENU_SELECTING_DOT);
        }
        else{
          lcd.print(" ");
        }
        lcd.print("Settings");
        break;

      case SECOND_COLUMN:
        lcd.setCursor(0,0);
        if(currentMenuPos == SECOND_COLUMN){
          lcd.write(MENU_SELECTING_DOT);
        }
        else{
          lcd.print(" ");
        }
        lcd.print("Settings");

        lcd.setCursor(0,1);
        if(currentMenuPos == THIRD_COLUMN){
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
  else if(menuLevel == SECOND_MENU){
    if(mainMenuPick == SETTINGS){
      lcd.setCursor(0,0);
      if(currentMenuPos == FIRST_COLUMN){
        lcd.write(MENU_SELECTING_DOT);
      }
      else{
        lcd.print(" ");
      }
      lcd.print("<LCD brightness");

      lcd.setCursor(0,1);
      if(currentMenuPos == SECOND_COLUMN){
        lcd.write(MENU_SELECTING_DOT);
      }
      else{
        lcd.print(" ");
      }
      lcd.print("<Game brightness");
    }
  }
  else if(menuLevel == THIRD_MENU){
    if(secondMenuPick == LCD_BRIGHTNESS){
      lcd.setCursor(0,0);
      lcd.print("use up/down");
      lcd.setCursor(0,1);
      for(int i = 0; i < lcdScaledBrighness; i++){
        lcd.write(SELECT_BRIGHTNESS);
      }
    }

    else if(secondMenuPick == MATRIX_BRIGHTNESS){
      lcd.setCursor(0,0);
      lcd.print("use up/down");
      lcd.setCursor(0,1);
      for(int i = 0; i < matrixBrightness; i++){
        lcd.write(SELECT_BRIGHTNESS);
      }
    }
  }
}


void changeMenu() {
  int xValue = analogRead(Y_PIN);
  int yValue = analogRead(X_PIN);
  
  //go back to previous menu
  if(yValue < MIN_THRESHOLD){
    if(millis() - lastMenuMoved > MENU_MOVE_INTERVAL){
      lastMenuMoved = millis();
      if(menuLevel > FIRST_MENU){
        menuLevel--;
        wasMenuDisplayed = false;
      }
    }
  }

  //navigate the menu
  if (xValue > MAX_THRESHOLD || xValue < MIN_THRESHOLD) {
    if(millis() - lastMenuMoved < MENU_MOVE_INTERVAL){
      return;
    }
    else{
      lastMenuMoved = millis();
    }
  }
  byte menuCols;
  if(menuLevel == FIRST_MENU){
    menuCols =  MAIN_MENU_COLS;
  }
  else if(menuLevel == SECOND_MENU || menuCols == THIRD_MENU){
    menuCols = SETTINGS_MENU_COLS;
  }
  if (xValue > MAX_THRESHOLD) {
    if (currentMenuPos < currentMenuPosBias + LCD_COLS - 1) {
      currentMenuPos++;
      wasMenuDisplayed = false;

    } 
    else if (currentMenuPosBias + LCD_COLS < menuCols) {
      currentMenuPosBias++;
      currentMenuPos++;
      wasMenuDisplayed = false;

    }
  }

  if (xValue < MIN_THRESHOLD) {
    if (currentMenuPos > currentMenuPosBias) {
      currentMenuPos--;
      wasMenuDisplayed = false;

    } 
    else if (currentMenuPosBias > FIRST_COLUMN) {
      currentMenuPosBias--;
      currentMenuPos = currentMenuPosBias;
      wasMenuDisplayed = false;
    }
  }
  
}

void resetLCDForGame(){
  lcd.clear();
}

void displayAboutText(){
  if(!wasAboutTextPrinted){

    lcd.setCursor(0,0);
    lcd.print("< Nuke 'em All by Ilinca. Available at:");

    lcd.setCursor(0,1);
    lcd.print("github.com/ilinca-ana-moraru");

    wasAboutTextPrinted = true;
  }
  else{
    int yValue = analogRead(X_PIN);

    if(yValue > MAX_THRESHOLD){
      if(millis() - lastScrollChange > TIME_BETWEEN_SCROLL){
        lcd.scrollDisplayLeft();
        lastScrollChange = millis();
      }
    }
  }
}

void checkForBrightnessChange(){
  int xValue = analogRead(Y_PIN);
  if(secondMenuPick == MATRIX_BRIGHTNESS){
    if(xValue > MAX_THRESHOLD){
      if(millis() - lastBrightnessChange > TIME_BETWEEN_BRIGHTNESS_CHANGE){
        if(matrixBrightness > 1){
          lastBrightnessChange = millis();
          matrixBrightness--;
          wasMenuDisplayed = false;
          lc.setIntensity(0, matrixBrightness); 
        }
      }
    }

    if(xValue < MIN_THRESHOLD){
      if(millis() - lastBrightnessChange > TIME_BETWEEN_BRIGHTNESS_CHANGE){
        if(matrixBrightness < 16){
          lastBrightnessChange = millis();
          matrixBrightness++;
          wasMenuDisplayed = false;
          lc.setIntensity(0, matrixBrightness); 
        }
      }
    }

  }

  else if(secondMenuPick == LCD_BRIGHTNESS){
    if(xValue > MAX_THRESHOLD){
      if(millis() - lastBrightnessChange > TIME_BETWEEN_BRIGHTNESS_CHANGE){
        if(lcdScaledBrighness > 1){
          lastBrightnessChange = millis();
          lcdScaledBrighness--;
          wasMenuDisplayed = false;
          lcdBrightness = map(lcdScaledBrighness, 0, 16, 0, 255);
          analogWrite(LCD_BRIGHTNESS_PIN, lcdBrightness);
        }
      }
    }

    if(xValue < MIN_THRESHOLD){
      if(millis() - lastBrightnessChange > TIME_BETWEEN_BRIGHTNESS_CHANGE){
        if(lcdScaledBrighness < 16){
          lastBrightnessChange = millis();
          lcdScaledBrighness++;
          wasMenuDisplayed = false;
          lcdBrightness = map(lcdScaledBrighness, 0, 16, 0, 255);
          analogWrite(LCD_BRIGHTNESS_PIN, lcdBrightness);
        }
      }
    }

  }

}
