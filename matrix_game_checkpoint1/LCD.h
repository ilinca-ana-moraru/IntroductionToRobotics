#ifndef LCD_H
#define LCD_H

#include "LEDmatrix.h"
#include <LiquidCrystal.h>

#define LCD_ROWS 16
#define LCD_COLS 2
#define LENGTH_OF_WELCOME_TEXT 10

#define WELCOME 10
#define END_GAME 11
#define MAIN_MENU 12
#define SUBMENU 13
#define SECOND_SUBMENU 14
#define TIME_BETWEEN_LCD_LETTERS 300

#define MENU_COLS 3
#define MENU_SELECTING_DOT 1
#define MENU_MOVE_INTERVAL 300

void showTextLCD(char* text);

void stopTextLCD();

void setLCDForMessageDisplay();

void displayMenu();

void changeMenu();

void selectMainMenu();

void resetLCDForGame();

#endif