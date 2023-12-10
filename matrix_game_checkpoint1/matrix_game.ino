#include "LCD.h"
#include "LEDmatrix.h"

#include "LedControl.h"
#include <LiquidCrystal.h>

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


char* welcomeText = {"Welcome!!!"};
char* endGameText = {"END GAME"};
byte LCDState; //WELCOME, END_GAME, PLAYING
int currentTextPos = -1;
unsigned long lastChangeText = 0;

LedControl lc = LedControl(DIN_PIN, CLOCK_PIN, LOAD_PIN, 1); // DIN, CLK, LOAD, No. DRIVER

unsigned long long lastMoved = 0; 

short xBias = MATRIX_SIZE/2;
short yBias = MATRIX_SIZE/2;
byte xDefaultDistanceBetweenPosAndBias = MATRIX_SIZE/2;
byte yDefaultDistanceBetweenPosAndBias = MATRIX_SIZE/2;
byte xPos = xDefaultDistanceBetweenPosAndBias + xBias;
byte yPos = yDefaultDistanceBetweenPosAndBias + yBias;
byte xLastPos = 0;
byte yLastPos = 0;

bool matrixChanged = true; 
// 2D array representing what is on the map currently (wall/bomb)
byte matrix[MAP_SIZE][MAP_SIZE] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
;

bool playerBlinkingState = 1;
unsigned long playerLastBlink = 0;

bomb* bombs;

byte nrOfBombs = 0;
bool pressState = 0;
bool pressRead = 0;
unsigned long lastPressCheck = 0;

bool bombsBlinkingState = 1;
unsigned long bombsLastBlink = 0;
unsigned long randomNumber;

                            // up     down    left    right
direction directions[NR_OF_DIRECTIONS] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
direction extendedBombCoordonates[NR_OF_EXTENDED_BOMB_COORDONATES] = {{-2, 0}, {-1, -1},{-1, 0}, {-1, 1},
{0, -2}, {0, -1}, {0, 1}, {0, 2}, {1, -1}, {1, 0}, {1, 1}, {2, 0}, {0, 0}};

bool skullAnimation[MATRIX_SIZE][MATRIX_SIZE] = {
  {0, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 1, 1, 0, 0, 1},
  {1, 0, 0, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0}  
};
unsigned long showDeathStart = 0;
unsigned long showWinStart = 0;
unsigned int timeBetweenDeathFrames = 0;

bool winAnimation[MATRIX_SIZE][MATRIX_SIZE] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}  
};

unsigned long AnimationStart = 0;
unsigned long timeBetweenStartFrames = 0;
bool currentStartAnimationFrame;

bool startAnimation[NR_OF_START_ANIMATION_FRAMES][MATRIX_SIZE][MATRIX_SIZE]{
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 1, 1, 0, 1, 0},
    {0, 1, 0, 1, 1, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}

  },
  {
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {0, 1, 1, 1, 1, 1, 1, 0}

  }

};

byte gameState;

byte wasLCDResetForMessageDisplay = 0;

byte currentMenuPosBias = 0;
byte currentMenuPos = 0;
bool wasMenuDisplayed = 0;
byte menuSelectingDot[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};
bool joyMoved = 0;
byte menuLevel = 1;

void setup() {
  lcd.begin(LCD_ROWS, LCD_COLS);
  lcd.setCursor(LCD_ROWS, 0);
  lcd.autoscroll();
  LCDState = WELCOME;

  pinMode(PRESS_PIN, INPUT_PULLUP);
  pinMode(X_PIN, INPUT);
  pinMode(Y_PIN, INPUT);
  pinMode(DIN_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LOAD_PIN, OUTPUT);
  Serial.begin(115200);
  lc.shutdown(0, false); 
  lc.setIntensity(0, MATRIX_BRIGHTNESS); 
  lc.clearDisplay(0);
  lcd.createChar(MENU_SELECTING_DOT, menuSelectingDot);
  randomSeed(analogRead(PRESS_PIN));
  startGameAnimation();
  generateMap();
  updateMatrix();
}

void loop() {
    Serial.print("LCD state: ");
    Serial.println(LCDState);
    Serial.print("was menu displayed: ");
    Serial.println(wasMenuDisplayed);

    if(LCDState == WELCOME){
      wasMenuDisplayed = 0;
      if(wasLCDResetForMessageDisplay == 0){
        setLCDForMessageDisplay();
      }
      showTextLCD(welcomeText);
    }

    if(LCDState == END_GAME){
      wasMenuDisplayed = 0;
      if(wasLCDResetForMessageDisplay == 0){
        setLCDForMessageDisplay();
      }
      showTextLCD(endGameText);
    }

    if(LCDState == MENU){
      if(wasMenuDisplayed == 0){
        lcd.clear();
        lcd.noAutoscroll();
        displayMenu();
      }
      changeMenu();
    }

  if(gameState == MENU){
    showMatrixMenu();
  }

  //check if the joystick moved
  if(gameState == PLAYING){
    if (millis() - lastMoved > MOVE_INTERVAL) { 
      updatePositions(); 
      lastMoved = millis(); 
    }
  }

  // change matrix if necessary
  if (matrixChanged == true) {
    updateMatrix(); 
    matrixChanged = false; 
  }

  if(gameState == PLAYING){
    // player blink
    if(millis() - playerLastBlink > PLAYER_BLINK_INTERVAL){
      playerBlink();
      playerLastBlink = millis();
    }
    // bombs blink
    if(sizeof(bombs)){
      if(millis() - bombsLastBlink > BOMBS_BLINK_INTERVAL){
        bombsBlink();
        bombsLastBlink = millis();
      }
    }
  }


  for(int i = 0; i < nrOfBombs; i++){
    //if visibility time of bomb is up, it's state becomes delete_bomb so that it is deleted in updateMatrix() 
    if(millis() - bombs[i].bombStart > BOMBS_DISAPPEARING_INTERVAL){
      bombs[i].state = DELETE_BOMB;
      matrixChanged = true;
    }

    //if it is time to expand bomb, it's state becomes expand_bomb so that it is expanded in updateMatrix()
    else if(millis() - bombs[i].bombStart > BOMBS_EXPANDING_INTERVAL){
      bombs[i].state = EXPAND_BOMB;
      matrixChanged = true;
    }
  }

  buttonPressLogic();

 //if the time for the death animation is up, generate next map
  if(gameState == DEATH_ANIMATION){
    if(millis() - showDeathStart > DEFAULT_ANIMATION_DISPLAY && LCDState != END_GAME){
      // stopTextLCD();
      gameState = MENU;
      LCDState == MENU;

    }
  }

  //if the time for the winning animation is up, generate next map
  if(gameState == WINNING_ANIMATION){
    if(millis() - showWinStart > DEFAULT_ANIMATION_DISPLAY){
      startGame();
    }
  }

    //if the time for the start animation is up, generate next map
  if(gameState == START_ANIMATION){
    if(millis() - AnimationStart > DEFAULT_START_ANIMATION && LCDState != WELCOME){
      // aici o sa fie meniu
      // startGame();
      gameState = MENU;
      LCDState == MENU;
    }
    else{
      if(millis() - timeBetweenStartFrames > INTERVAL_BETWEEN_FRAMES){
        currentStartAnimationFrame = !currentStartAnimationFrame;
        timeBetweenStartFrames = millis();
        matrixChanged = true;


      }
    }
  }

}

void startGame(){
      generateMap();
      stopTextLCD();
      gameState = PLAYING;
      LCDState = PLAYING;
      resetLCDForGame();
      matrixChanged = true;

}

void startGameAnimation(){
  AnimationStart = millis();
  gameState = START_ANIMATION;
  currentStartAnimationFrame = 0;
}

void buttonPressLogic(){
  
  pressRead = !digitalRead(PRESS_PIN);
  //button press debounce
  if(millis() - lastPressCheck > PRESS_DEBOUNCE_TIME){
    lastPressCheck = millis();

      if(pressRead != pressState){
        pressState = pressRead;

        if(pressState == HIGH){
          // if user wants death animation to be skipped
          if(gameState == DEATH_ANIMATION){
            stopTextLCD();
            gameState = MENU;
            LCDState = MENU;
            wasMenuDisplayed = 0;
            return;
          }
          // if user wants winning animation to be skipped
          else if(gameState == WINNING_ANIMATION){
            startGame();
          }

          if(gameState == START_ANIMATION || LCDState == WELCOME){
            stopTextLCD();
            gameState = MENU;
            LCDState = MENU;
            wasMenuDisplayed = 0;
            return;
          }


          //if user wants to drop bomb
          else if(gameState == PLAYING){
            putBomb();
          }

          if(LCDState == MAIN_MENU){
            selectMainMenu();
          }

          
        }
      }
    
  }

}



void selectMainMenu(){
  switch (currentMenuPos)
  {
  case 0:
    startGame();
    break;
  
  default:
    break;
  }
}