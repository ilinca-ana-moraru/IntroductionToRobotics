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
byte xPos = 4;
byte yPos = 4;
byte xLastPos = 0;
byte yLastPos = 0;
// Thresholds for detecting joystick movement
const int minThreshold = 312;
const int maxThreshold = 712;

const byte moveInterval = 150; // Timing variable to control the speed of LED movement
unsigned long long lastMoved = 0; // Tracks the last time the LED moved

const byte matrixSize = 8 ;// Size of the LED matrix
bool matrixChanged = true; // Flag to track if the matrix display needs updating
// 2D array representing the state of each LED (on/off) in the matrix
unsigned long matrix[matrixSize][matrixSize] = {
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
#define PLAYER_BLINK_INTERVAL  300

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
unsigned long lastPressCheck = 0;
#define PRESS_DEBOUNCE_TIME 50

bool bombsBlinkingState = 1;
unsigned long bombsLastBlink = 0;
#define BOMBS_BLINK_INTERVAL  50
#define BOMBS_EXPANDING_INTERVAL 1000
#define BOMBS_DISAPPEARING_INTERVAL 1500

unsigned long randomNumber;

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

  // blink player
  if(millis() - playerLastBlink > PLAYER_BLINK_INTERVAL){
    playerBlink();
    playerLastBlink = millis();
  }

  //bombs blink
  if(sizeof(bombs) > 0){
    if(millis() - bombsLastBlink > BOMBS_BLINK_INTERVAL){
      bombsBlink();
      bombsLastBlink = millis();
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
  pressState = !digitalRead(pressPin);
  if(lastPressState != pressState){
    if(millis() - lastPressCheck > PRESS_DEBOUNCE_TIME){
      lastPressState = pressState;
      if(pressState == true){
        putBomb();

      }
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
        deleteBomb(bombs[i]);
      }
    }
  }
  

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



void generateMap(){
  for(int i = 0; i < matrixSize; i++){
    for(int j = 0; j < matrixSize; j++){
      randomNumber = random(3);
        if(randomNumber > 0){
          matrix[i][j] = WALL;
        }
    }
  }

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

  if (yValue > maxThreshold) {
    if (yPos < matrixSize - 1) {
      yPos++;
    } 
  }
  // Update xPos based on joystick movement (Y-axis)
  if (yValue < minThreshold) {
    if (yPos > 0) {
      yPos--;
    }
  }
 // Check if the position has changed and update the matrix if necessary
  if (xPos != xLastPos || yPos != yLastPos) {
    // matrix[xLastPos][yLastPos] = EMPTY_SPACE;
    // matrix[xPos][yPos] = PLAYER;
    matrixChanged = true;
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

}


void bombsBlink(){
  bombsBlinkingState = !bombsBlinkingState;
  matrixChanged = true;
}

void playerBlink(){
  playerBlinkingState = !playerBlinkingState;
  matrixChanged = true;
}
