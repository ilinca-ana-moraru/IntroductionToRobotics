#include "LedControl.h" // Include LedControl library for controlling the LED matrix
#define DIN_PIN  12
#define CLOCK_PIN  11
#define LOAD_PIN  10

#define PRESS_PIN 13
#define X_PIN A0
#define Y_PIN  A1
LedControl lc = LedControl(DIN_PIN, CLOCK_PIN, LOAD_PIN, 1); // DIN, CLK, LOAD, No. DRIVER

#define MATRIX_BRIGHTNESS  15

#define EMPTY_SPACE 0
#define PLAYER 1
#define BOMB 2
#define WALL 3

#define DROPPED_BOMB 4
#define EXPAND_BOMB 5
#define DELETE_BOMB 6

#define MIN_THRESHOLD  312
#define MAX_THRESHOLD 712
#define DEFAULT_JOYSTICK 512
#define MOVE_INTERVAL 200 
unsigned long long lastMoved = 0; 

#define MATRIX_SIZE 8 
#define MAP_SIZE 16

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
#define PLAYER_BLINK_INTERVAL  100

struct bomb{
  byte x;
  byte y;
  byte state;
  unsigned long bombStart;
};
bomb* bombs;

byte nrOfBombs = 0;
bool pressState = 0;
bool pressRead = 0;
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
#define NR_OF_DIRECTIONS 4
                            // up     down    left    right
direction directions[NR_OF_DIRECTIONS] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
#define NR_OF_EXTENDED_BOMB_COORDONATES 13
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
#define DEFAULT_ANIMATION_DISPLAY 2000

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
#define PLAYING 0
#define DEATH_ANIMATION 1
#define WINNING_ANIMATION 2
byte gameState = PLAYING;

void setup() {

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
  randomSeed(analogRead(PRESS_PIN));
  generateMap();
  updateMatrix();
}
void loop() {


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


  //if visibility time of bomb is up, it's state becomes delete_bomb so that it is deleted in updateMatrix() 
  for(int i = 0; i < nrOfBombs; i++){
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
    if(millis() - showDeathStart > DEFAULT_ANIMATION_DISPLAY){
      generateMap();
      gameState = PLAYING;
      matrixChanged = true;
    }
  }

  //if the time for the winning animation is up, generate next map
  if(gameState == WINNING_ANIMATION){
    if(millis() - showWinStart > DEFAULT_ANIMATION_DISPLAY){
      generateMap();
      gameState = PLAYING;
      matrixChanged = true;
    }
  }

}

void updateMatrix() {

  // check for things to update in the matrix
  if(sizeof(bombs) > 0){
    for(int i = 0; i < nrOfBombs; i++){
      if(bombs[i].state == DROPPED_BOMB){
        matrix[bombs[i].x][bombs[i].y] = BOMB;
      }
      if(bombs[i].state == EXPAND_BOMB){
        expandBomb(bombs[i]);
      }
      if(bombs[i].state == DELETE_BOMB){
        if( checkIfLost(bombs[i])){
          gameState = DEATH_ANIMATION;
          deleteBomb(bombs[i]);
        }
        else if(checkIfWon()){
          deleteBomb(bombs[i]);
          gameState = WINNING_ANIMATION;
        }
        else{
          deleteBomb(bombs[i]);
        }
      }
    }
  }
  

  if(gameState == DEATH_ANIMATION){
    showSkull();
    showDeathStart = millis();
  }

  else if(gameState == WINNING_ANIMATION){
    showWin();
    showWinStart = millis();
  }

  // display visible space of the map is game is played
  if(gameState == PLAYING){

    for (int row = xBias; row < xBias + MATRIX_SIZE; row++) {
      for (int col = yBias; col < yBias + MATRIX_SIZE; col++) {
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


// generates new map
void generateMap(){
  xPos = xDefaultDistanceBetweenPosAndBias + xBias;
  yPos = yDefaultDistanceBetweenPosAndBias + yBias;
  for(int i = 0; i < MAP_SIZE; i++){
    for(int j = 0; j < MAP_SIZE; j++){
      randomNumber = random(3);
        if(randomNumber > 0){
          matrix[i][j] = WALL;
        }
        else{
          matrix[i][j] = EMPTY_SPACE;
        }
    }
  }
  // make sure that it is possible for the player to move from initial position and drop bomb
  matrix[xPos][yPos] = EMPTY_SPACE;
  //checks if there is already a way to move from  initial position
  for(int i = 0; i < NR_OF_DIRECTIONS; i++){
    if(matrix[xPos + 3 * directions[i].x][yPos + 3* directions[i].y] == EMPTY_SPACE &&
    matrix[xPos + 2 * directions[i].x][yPos + 2 * directions[i].y] == EMPTY_SPACE &&
    matrix[xPos + directions[i].x][yPos + directions[i].y] == EMPTY_SPACE)
      return;
  }

  // if there is no direction to move, generate one randomly
  direction randomDirection = directions[random(4)];
  randomDirection = directions[random(4)];
  

  matrix[xPos + 3 * randomDirection.x][yPos + 3 * randomDirection.y] = EMPTY_SPACE;
  matrix[xPos + 2 * randomDirection.x][yPos + 2 * randomDirection.y] = EMPTY_SPACE;
  matrix[xPos + randomDirection.x][yPos + randomDirection.y] = EMPTY_SPACE;      
  
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
            generateMap();
            gameState = PLAYING;
            matrixChanged = true;
          }
          // if user wants winning animation to be skipped
          else if(gameState == WINNING_ANIMATION){
            generateMap();
            gameState = PLAYER;
            matrixChanged = true;
          }

          //if user wants to drop bomb
          else{
            putBomb();
          }
        }
      }
    
  }

}

//expands the bomb on the map
void expandBomb(bomb currentBomb){  
  for(int currentCoordonates = 0; currentCoordonates < NR_OF_EXTENDED_BOMB_COORDONATES; currentCoordonates++){
    int i = extendedBombCoordonates[currentCoordonates].x + currentBomb.x;
    int j = extendedBombCoordonates[currentCoordonates].y + currentBomb.y;
      if(inMatrix(i, j)){
        matrix[i][j] = BOMB;
      }
    }
  }

//deletes bomb on the matrix
void deleteBomb(bomb currentBomb) {

  for (int i = 0; i < nrOfBombs - 1; i++) {
    bombs[i] = bombs[i + 1];
  }
  nrOfBombs--;
  bombs = (bomb*)realloc(bombs, nrOfBombs * sizeof(bomb));

  for(int currentCoordonates = 0; currentCoordonates < NR_OF_EXTENDED_BOMB_COORDONATES; currentCoordonates++){
    int i = extendedBombCoordonates[currentCoordonates].x + currentBomb.x;
    int j = extendedBombCoordonates[currentCoordonates].y + currentBomb.y;
      if(inMatrix(i, j)){
        matrix[i][j] = EMPTY_SPACE;
      }
    }

}

//checks if user was killed by the exploding bomb
bool checkIfLost(bomb currentBomb){
  
  for(int currentCoordonates = 0; currentCoordonates < NR_OF_EXTENDED_BOMB_COORDONATES; currentCoordonates++){
    int i = extendedBombCoordonates[currentCoordonates].x + currentBomb.x;
    int j = extendedBombCoordonates[currentCoordonates].y + currentBomb.y;
      if(xPos == i && yPos == j){
        return true;
      }
    }
  return false;
}


//read joystick input and update the position of the player
void updatePositions() {
  int xValue = analogRead(Y_PIN);
  int yValue = analogRead(X_PIN);
  xLastPos = xPos;
  yLastPos = yPos;
  
  // update xPos based on joystick movement (X-axis)
  if (xValue > MAX_THRESHOLD) {
    if (xPos < MAP_SIZE - 1) {
      xPos++;
    } 
  }
  if (xValue < MIN_THRESHOLD) {
    if (xPos > 0) {
      xPos--;
    }
  }

  // update yPos based on joystick movement (Y-axis)
  if (yValue > MAX_THRESHOLD) {
    if (yPos < MAP_SIZE - 1) {
      yPos++;
    } 
  }
  if (yValue < MIN_THRESHOLD) {
    if (yPos > 0) {
      yPos--;
    }
  }
 // check if the position has changed and update the matrix if necessary
  if (xPos != xLastPos || yPos != yLastPos) {
    // if both the X-axis and Y-axis were changed and chenge only the one with the biggest movement on the axis
    if (xPos != xLastPos && yPos != yLastPos){
      if(module(DEFAULT_JOYSTICK - xValue) > module(DEFAULT_JOYSTICK - yValue)){
        yPos = yLastPos;
      }
      else{
        xPos = xLastPos;
      } 
    }
    //check that the next space is empty
    if(matrix[xPos][yPos] == EMPTY_SPACE){
      //move the "view" of the map with the player movement, unless the map is at its end
      xBias = xPos - xDefaultDistanceBetweenPosAndBias;
      yBias = yPos - yDefaultDistanceBetweenPosAndBias;

      if(xBias < 0){
        xBias = 0;
      }
      if(xBias + MATRIX_SIZE > MAP_SIZE){
        xBias = MAP_SIZE - MATRIX_SIZE;
      }

      if(yBias < 0){
        yBias = 0;
      }
      if(yBias + MATRIX_SIZE > MAP_SIZE){
        yBias = MAP_SIZE - MATRIX_SIZE;
      }

      matrixChanged = true;

    }
    // if the next space is not empty, do not move
    else{
      xPos = xLastPos;
      yPos = yLastPos;
    }
  }
}

// checks if the coordonates are within the map
bool inMatrix(int i,int j){
  if(i < 0 || i > MAP_SIZE - 1 || j < 0 || j > MAP_SIZE - 1){
    return false;
  }
  return true;
}

// allocates memory for a bomb and makes the bomb visible
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

//changes the state of the bomb blink
void bombsBlink(){
  bombsBlinkingState = !bombsBlinkingState;
  matrixChanged = true;
}

//changes the state of the player blink
void playerBlink(){
  playerBlinkingState = !playerBlinkingState;
  matrixChanged = true;
}

//shows dying animation
void showSkull(){
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      lc.setLed(0, row, col, skullAnimation[row][col]);
    }
  }
}

//shows winning animation
void showWin(){
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      lc.setLed(0, row, col, winAnimation[row][col]);
    }
  }
}

//checks if there are walls left
bool checkIfWon(){
  for(int i = 0; i < MAP_SIZE; i++){
    for(int j = 0; j < MAP_SIZE; j++){
      if(matrix[i][j] == WALL){
        return false;
      }
    }
  }
  return true;
    
}

//arithmethic module operation
int module(int number){
  if(number < 0){
    return number * (-1);
  }
  return number;

}