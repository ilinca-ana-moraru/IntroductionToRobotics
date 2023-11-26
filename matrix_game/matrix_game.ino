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
// Thresholds for detecting joystick movement
const int minThreshold = 312;
const int maxThreshold = 712;
#define DEFAULT_JOYSTICK 512
const byte moveInterval = 200; // Timing variable to control the speed of LED movement
unsigned long long lastMoved = 0; // Tracks the last time the LED moved

const byte matrixSize = 8 ;// Size of the LED matrix
const int mapSize = 16;

int xBias = matrixSize/2;
int yBias = matrixSize/2;
byte xDefaultDistanceBetweenPosAndBias = matrixSize/2;
byte yDefaultDistanceBetweenPosAndBias = matrixSize/2;
byte xPos = xDefaultDistanceBetweenPosAndBias + xBias;
byte yPos = yDefaultDistanceBetweenPosAndBias + yBias;
byte xLastPos = 0;
byte yLastPos = 0;

bool matrixChanged = true; // Flag to track if the matrix display needs updating
// 2D array representing the state of each LED (on/off) in the matrix
byte matrix[mapSize][mapSize] = {
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
#define PLAYER_BLINK_INTERVAL  100

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
const int nrOfExtendedBombCoordones = 13;
direction extendedBombCoordonates[nrOfExtendedBombCoordones] = {{-2, 0}, {-1, -1},{-1, 0}, {-1, 1},
{0, -2}, {0, -1}, {0, 1}, {0, 2}, {1, -1}, {1, 0}, {1, 1}, {2, 0}, {0, 0}};

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
    }
  }
  if(displayWin == true){
    if(millis() - showWinStart > DEFAULT_ANIMATION_DISPLAY){
      generateMap();
      displayWin = false;
      matrixChanged = true;
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

    for (int row = xBias; row < xBias + matrixSize; row++) {
      for (int col = yBias; col < yBias + matrixSize; col++) {
        if(row == xPos && col ==yPos){
          lc.setLed(0, xPos - xBias, yPos - yBias, playerBlinkingState);
        }
        else{
          if(matrix[row][col] == WALL){
            lc.setLed(0, row - xBias, col - yBias, 1);
          }
          else if(matrix[row][col] == EMPTY_SPACE){
            lc.setLed(0, row - xBias, col - yBias, 0);
          }
          else if(matrix[row][col] == BOMB){
            lc.setLed(0, row - xBias, col - yBias, bombsBlinkingState);
          }
        }
      }
    }
  }

}



void generateMap(){
  xPos = xDefaultDistanceBetweenPosAndBias + xBias;
  yPos = yDefaultDistanceBetweenPosAndBias + yBias;
  for(int i = 0; i < mapSize; i++){
    for(int j = 0; j < mapSize; j++){
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
    if(matrix[xPos + 3 * directions[i].x][yPos + 3* directions[i].y] == EMPTY_SPACE &&
    matrix[xPos + 2 * directions[i].x][yPos + 2 * directions[i].y] == EMPTY_SPACE &&
    matrix[xPos + directions[i].x][yPos + directions[i].y] == EMPTY_SPACE)
      return;
  }

  direction randomDirection = directions[random(4)];
  while(!(inMatrix(xPos + randomDirection.x + randomDirection.x ,yPos + randomDirection.y + randomDirection.y))){
    randomDirection = directions[random(4)];
  }

  matrix[xPos + 3 * randomDirection.x][yPos + 3 * randomDirection.y] = EMPTY_SPACE;
  matrix[xPos + 2 * randomDirection.x][yPos + 2 * randomDirection.y] = EMPTY_SPACE;
  matrix[xPos + randomDirection.x][yPos + randomDirection.y] = EMPTY_SPACE;      
  
}

void expandBomb(bomb currentBomb){  
  for(int currentCoordonates = 0; currentCoordonates < nrOfExtendedBombCoordones; currentCoordonates++){
    int i = extendedBombCoordonates[currentCoordonates].x + currentBomb.x;
    int j = extendedBombCoordonates[currentCoordonates].y + currentBomb.y;
      if(inMatrix(i, j)){
        matrix[i][j] = BOMB;
      }
    }
  }


void deleteBomb(bomb currentBomb) {

  for (int i = 0; i < nrOfBombs - 1; i++) {
    bombs[i] = bombs[i + 1];
  }
  nrOfBombs--;
  bombs = (bomb*)realloc(bombs, nrOfBombs * sizeof(bomb));

  for(int currentCoordonates = 0; currentCoordonates < nrOfExtendedBombCoordones; currentCoordonates++){
    int i = extendedBombCoordonates[currentCoordonates].x + currentBomb.x;
    int j = extendedBombCoordonates[currentCoordonates].y + currentBomb.y;
      if(inMatrix(i, j)){
        matrix[i][j] = EMPTY_SPACE;
      }
    }

}

bool checkIfLost(bomb currentBomb){
  
  for(int currentCoordonates = 0; currentCoordonates < nrOfExtendedBombCoordones; currentCoordonates++){
    int i = extendedBombCoordonates[currentCoordonates].x + currentBomb.x;
    int j = extendedBombCoordonates[currentCoordonates].y + currentBomb.y;
      if(xPos == i && yPos == j){
        return true;
      }
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
    if (xPos < mapSize - 1) {
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
    if (yPos < mapSize - 1) {
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
      // daca trebuie sa se miste harta
      xBias = xPos - xDefaultDistanceBetweenPosAndBias;
      yBias = yPos - yDefaultDistanceBetweenPosAndBias;

      if(xBias < 0){
        xBias = 0;
      }
      if(xBias + matrixSize > mapSize){
        xBias = mapSize - matrixSize;
      }

      if(yBias < 0){
        yBias = 0;
      }
      if(yBias + matrixSize > mapSize){
        yBias = mapSize - matrixSize;
      }


      matrixChanged = true;

    }
    else{
      xPos = xLastPos;
      yPos = yLastPos;
    }
  }
}

bool inMatrix(byte i, byte j){
  if(i < 0 || i > mapSize - 1 || j < 0 || j > mapSize - 1){
    return false;
  }
  return true;
}

void putBomb(){
  nrOfBombs++;
  bombs = (bomb*) realloc(bombs, nrOfBombs * sizeof(bomb));

  bombs[nrOfBombs - 1].x = xPos;
  bombs[nrOfBombs - 1].y = yPos;
  bombs[nrOfBombs - 1].state = DROPPED_BOMB;
  bombs[nrOfBombs - 1].bombStart = millis();
  matrix[xPos][yPos] = BOMB;
  matrixChanged = true;

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
  for(int i = 0; i < mapSize; i++){
    for(int j = 0; j < mapSize; j++){
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