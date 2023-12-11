#ifndef LCD_H
#define LCD_H

#include "LEDmatrix.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define LCD_ROWS 16
#define LCD_COLS 2
#define LENGTH_OF_WELCOME_TEXT 10

#define WELCOME 10
#define END_GAME 11
#define MAIN_MENU 12
#define SUBMENU 13
#define SECOND_SUBMENU 14
#define TIME_BETWEEN_LCD_LETTERS 300

#define MAIN_MENU_COLS 3
#define SETTINGS_MENU_COLS 2
#define MENU_SELECTING_DOT 1
#define MENU_MOVE_INTERVAL 300

#define SETTINGS 15
#define ABOUT 16
#define LCD_BRIGHTNESS 17
#define MATRIX_BRIGHTNESS 18
#define TIME_BETWEEN_SCROLL 700
#define TIME_BETWEEN_BRIGHTNESS_CHANGE 100
#define LCD_BRIGHTNESS_PIN 3

#define FIRST_MENU 20
#define SECOND_MENU 21
#define THIRD_MENU 22

#define FIRST_COLUMN 0
#define SECOND_COLUMN 1
#define THIRD_COLUMN 2

#define MATRIX_BRIGHTNESS_STORRING_SPACE 120
#define LCD_BRIGHTNESS_STORRING_SPACE 160
void showTextLCD(char* text);

void stopTextLCD();

void setLCDForMessageDisplay();

void displayMenu();

void changeMenu();

void selectMenu();

void resetLCDForGame();

void displayAboutText();

void checkForBrightnessChange();


#endif