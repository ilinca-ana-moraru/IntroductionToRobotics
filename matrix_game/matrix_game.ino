#include "LedControl.h" // Include LedControl library for controlling the LED matrix
#include "time.h"
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const int pressPin = 13;
const int xPin = A0;
const int yPin = A1;
// Create an LedControl object to manage the LED matrix
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); // DIN, CLK, LOAD, No. DRIVER
// Variable to set the brightness level of the matrix
byte matrixBrightness = 2;

#define EMPTY_SPACE 0
#define PLAYER 1
#define BOMB 2
#define WALL 3

#define DROPPED_BOMB 4
#define SHOWN_BOMB 5
#define EXPANDED_BOMB 6
// Variables to track the current and previous positions of the joystick-controlled LED
byte defaultXPos = 4;
byte defaultYPos = 4;
byte xPos = defaultXPos;
byte yPos = defaultXPos;
byte xLastPos = 0;
byte yLastPos = 0;
// Thresholds for detecting joystick movement
const int minThreshold = 312;
const int maxThreshold = 712;
#define DEFAULT_JOYSTICK 512
const byte moveInterval = 200; // Timing variable to control the speed of LED movement
unsigned long long lastMoved = 0; // Tracks the last time the LED moved

const byte matrixSize = 8 ;// Size of the LED matrix
bool matrixChanged = true; // Flag to track if the matrix display needs updating
// 2D array representing the state of each LED (on/off) in the matrix
byte matrix[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}  
};

bool playerBlinkingState = 1;
unsigned long playerLastBlink = 0;
#define PLAYER_BLINK_INTERVAL  150

//bomb variables
struct bomb{
  byte x;
  byte y;
  byte state;
  unsigned long bombStart;
};
bomb* bombs;

int nrOfBombs = 0;
bool lastPressState = 0;
bool pressState = 0;
bool pressRead = 0;
bool lastPressRead = 0;
unsigned long lastPressCheck = 0;
#define PRESS_DEBOUNCE_TIME 50

bool bombsBlinkingState = 1;
unsigned long bombsLastBlink = 0;
#define BOMBS_BLINK_INTERVAL  30
#define BOMBS_EXPANDING_INTERVAL 1000
#define BOMBS_DISAPPEARING_INTERVAL 1500

unsigned long randomNumber;

struct direction{
  int x;
  int y;
};
byte nrOfDirections;
                            // up     down    left    right
direction directions[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};


byte skullAnimation[matrixSize][matrixSize] = {
  {0, 1, 1, 1, 1, 1, 1, 0},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 1, 1, 0, 0, 1},
  {1, 0, 0, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0}  
};
bool displayDeath = 0;
bool displayWin = 0;
unsigned long showDeathStart = 0;
unsigned long showWinStart = 0;
#define DEFAULT_ANIMATION_DISPLAY 2000

byte winAnimation[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 1, 1, 0, 0, 1, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}  
};


void setup() {

  pinMode(pressPin, INPUT_PULLUP);
  Serial.begin(115200);
  lc.shutdown(0, false); 
  lc.setIntensity(0, matrixBrightness); 
  lc.clearDisplay(0);
  randomSeed(analogRead(pressPin));
  generateMap();
  updateMatrix();
}
void loop() {


  // masurarea joystickului
  if (millis() - lastMoved > moveInterval) { 
    updatePositions(); 
    lastMoved = millis(); 
  }
  // schimbarea matrixului si afisarea corespunzatoare
  if (matrixChanged == true) {
    updateMatrix(); 
    matrixChanged = false; 
  }

  if(displayDeath == false && displayWin == false){
    // blink player
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

  //daca a trecut perioada de vizibilitate intregii bombe, sterge o
  for(int i = 0; i < nrOfBombs; i++){
    if(millis() - bombs[i].bombStart > BOMBS_DISAPPEARING_INTERVAL){
      bombs[i].state = EXPANDED_BOMB;
      matrixChanged = true;
    }

  // daca a trecut perioada de aratat o singura casuta, extinde
    else if(millis() - bombs[i].bombStart > BOMBS_EXPANDING_INTERVAL){
      bombs[i].state = SHOWN_BOMB;
      matrixChanged = true;
    }
  }

  // pune bomba noua
  pressRead = !digitalRead(pressPin);
  if(millis() - lastPressCheck > PRESS_DEBOUNCE_TIME){
    lastPressCheck = millis();

      if(pressRead != pressState){
        Serial.print("changed button state\n");
        pressState = pressRead;

        if(pressState == HIGH){
          Serial.print("HIGH button\n");
          if(displayDeath == true){
            Serial.println("exist death");
            generateMap();
            displayDeath = false;
            matrixChanged = true;
          }
          else if(displayWin == true){
            Serial.println("exist won");
            generateMap();
            displayWin = false;
            matrixChanged = true;
          }

          else{
            putBomb();
          }
        }
      }
    
  }
  
  if(displayDeath == true){
    Serial.println(millis() - showDeathStart);
    if(millis() - showDeathStart > DEFAULT_ANIMATION_DISPLAY){
      generateMap();
      displayDeath = false;
      matrixChanged = true;
      Serial.print("Gata timpul de moarte\n");

    }
  }
  if(displayWin == true){
    if(millis() - showWinStart > DEFAULT_ANIMATION_DISPLAY){
      generateMap();
      displayWin = false;
      matrixChanged = true;
      Serial.print("Gata timpul de victorie\n");

    }
  }

}

void updateMatrix() {

  if(sizeof(bombs) > 0){
    for(int i = 0; i < nrOfBombs; i++){
      if(bombs[i].state == DROPPED_BOMB){
        matrix[bombs[i].x][bombs[i].y] = BOMB;
      }
      if(bombs[i].state == SHOWN_BOMB){
        expandBomb(bombs[i]);
      }
      if(bombs[i].state == EXPANDED_BOMB){
        if( checkIfLost(bombs[i])){
          displayDeath = true;
          deleteBomb(bombs[i]);
        }
        else if(checkIfWon()){
          deleteBomb(bombs[i]);
          displayWin = true;
        }
        else{
          deleteBomb(bombs[i]);
        }
      }
    }
  }
  
  if(displayDeath == true){
    showSkull();
    showDeathStart = millis();
    Serial.println("update showDeathStart");
  }

  else if(displayWin == true){
    showWin();
    showWinStart = millis();
  }

  if(displayDeath == false && displayWin == false){
    for (int row = 0; row < matrixSize; row++) {
      for (int col = 0; col < matrixSize; col++) {
        if(row == xPos && col ==yPos){
          lc.setLed(0, xPos, yPos, playerBlinkingState);
        }
        else{
          if(matrix[row][col] == WALL){
            lc.setLed(0, row, col, 1);
          }
          else if(matrix[row][col] == EMPTY_SPACE){
            lc.setLed(0, row, col, 0);
          }
          else if(matrix[row][col] == BOMB){
            lc.setLed(0, row, col, bombsBlinkingState);
          }
        }
      }
    }
  }

}



void generateMap(){
  xPos = defaultXPos;
  yPos = defaultYPos;
  for(int i = 0; i < matrixSize; i++){
    for(int j = 0; j < matrixSize; j++){
      randomNumber = random(3);
        if(randomNumber > 0){
          matrix[i][j] = WALL;
        }
        else{
          matrix[i][j] = EMPTY_SPACE;
        }
    }
  }
  matrix[xPos][yPos] = EMPTY_SPACE;
  // make sure that it is possible for the player to move from initial position
  for(int i = 0; i < nrOfDirections; i++){
    if(inMatrix(xPos + directions[i].x + directions[i].x ,yPos + directions[i].y + directions[i].y) &&
    matrix[xPos + directions[i].x + directions[i].x][yPos + directions[i].y + directions[i].y] == EMPTY_SPACE &&
    matrix[xPos + directions[i].x][yPos + directions[i].y] == EMPTY_SPACE)
      return;
  }

  direction randomDirection = directions[random(4)];
  while(!(inMatrix(xPos + randomDirection.x + randomDirection.x ,yPos + randomDirection.y + randomDirection.y))){
    randomDirection = directions[random(4)];
  }
  matrix[xPos + randomDirection.x + randomDirection.x][yPos + randomDirection.y + randomDirection.y] = EMPTY_SPACE;
  matrix[xPos + randomDirection.x][yPos + randomDirection.y] = EMPTY_SPACE;      
  
}

void expandBomb(bomb currentBomb){  
  for(int i = currentBomb.x - 1; i <= currentBomb.x + 1; i++){
    for(int j = currentBomb.y - 1; j <= currentBomb.y + 1; j++){
      if(inMatrix(i, j)){
        matrix[i][j] = BOMB;
      }
    }
  }

  

}

void deleteBomb(bomb currentBomb) {

  for (int i = 0; i < nrOfBombs - 1; i++) {
    bombs[i] = bombs[i + 1];
  }
  nrOfBombs--;
  bombs = (bomb*)realloc(bombs, nrOfBombs * sizeof(bomb));

  for(int i = currentBomb.x - 1; i <= currentBomb.x + 1; i++){
    for(int j = currentBomb.y - 1; j <= currentBomb.y + 1; j++){
      if(inMatrix(i, j)){
        matrix[i][j] = EMPTY_SPACE;
      }
    }
  }
}

bool checkIfLost(bomb currentBomb){
  if(xPos >= currentBomb.x - 1  && xPos <= currentBomb.x + 1 
  && yPos >= currentBomb.y - 1 && yPos <= currentBomb.y + 1)
  {
    return true;
  }
  return false;
}


// Function to read joystick input and update the position of the LED
void updatePositions() {
  int xValue = analogRead(yPin);
  int yValue = analogRead(xPin);
  // Store the last positions of the LED
  xLastPos = xPos;
  yLastPos = yPos;
  // Update xPos based on joystick movement (X-axis)
  if (xValue > maxThreshold) {
    if (xPos < matrixSize - 1) {
      xPos++;
    } 
  }
  if (xValue < minThreshold) {
    if (xPos > 0) {
      xPos--;
    }
  }

  // Update yPos based on joystick movement (Y-axis)
  if (yValue > maxThreshold) {
    if (yPos < matrixSize - 1) {
      yPos++;
    } 
  }
  if (yValue < minThreshold) {
    if (yPos > 0) {
      yPos--;
    }
  }
 // Check if the position has changed and update the matrix if necessary
  if (xPos != xLastPos || yPos != yLastPos) {
    // sa nu se miste pe diagonala
    if (xPos != xLastPos && yPos != yLastPos){
      if(module(DEFAULT_JOYSTICK - xValue) > module(DEFAULT_JOYSTICK - yValue)){
        yPos = yLastPos;
      }
      else{
        xPos = xLastPos;
      } 
    }
    // sa nu treci prin perete
    if(matrix[xPos][yPos] == EMPTY_SPACE){
      matrixChanged = true;
    }
    else{
      xPos = xLastPos;
      yPos = yLastPos;
    }
  }  
}

bool inMatrix(byte i, byte j){
  if(i < 0 || i > matrixSize - 1 || j < 0 || j > matrixSize - 1){
    return false;
  }
  return true;
}

void putBomb(){
  Serial.println("am pus bomba");
  nrOfBombs++;
  bombs = (bomb*) realloc(bombs, nrOfBombs * sizeof(bomb));

  bombs[nrOfBombs - 1].x = xPos;
  bombs[nrOfBombs - 1].y = yPos;
  bombs[nrOfBombs - 1].state = DROPPED_BOMB;
  bombs[nrOfBombs - 1].bombStart = millis();
  matrix[xPos][yPos] = BOMB;
  matrixChanged = true;

  Serial.print("bomb nr: ");
  Serial.print(nrOfBombs);
  Serial.print(" x: ");
  Serial.print(xPos);
  Serial.print(" y: ");
  Serial.println(yPos);
}


void bombsBlink(){
  bombsBlinkingState = !bombsBlinkingState;
  matrixChanged = true;
}

void playerBlink(){
  playerBlinkingState = !playerBlinkingState;
  matrixChanged = true;
}


void showSkull(){
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, skullAnimation[row][col]);
    }
  }
}


void showWin(){
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, winAnimation[row][col]);
    }
  }
}


bool checkIfWon(){
  for(int i = 0; i < matrixSize; i++){
    for(int j = 0; j < matrixSize; j++){
      if(matrix[i][j] == WALL){
        return false;
      }
    }
  }
  return true;
    
}

int module(int number){
  if(number < 0){
    return number * (-1);
  }
  return number;

}