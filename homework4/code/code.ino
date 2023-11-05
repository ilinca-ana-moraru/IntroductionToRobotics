// declare all the joystick pins
const int pinSW = 2; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

// declare all the segments pins
const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;

// declare number of segments
#define SEGSIZE  8
 
// declare global variables/values used for making current segment blink
unsigned long lastTimeCheckForBlinking = millis();
#define BLINKING_PERIOD  500

// declare global variables/values for measuring the duration of the button press of the joystick
#define DEBOUNCE_TIME 40000
#define MIN_CLICK_TIME_SELECT 50000
#define MIN_CLICK_TIME_RESET  500000
unsigned long clickDuration = 0;
bool swState = LOW;
unsigned long startPressingButtonTime;
#define RISING 1
#define FALLING 0
unsigned long debounce = 0;
bool pressState = RISING; 

//declare global variables/values for measuring the movement of the joystick on the 2 axis
int xValue = 0;
int yValue = 0;
bool joyMoved = false;
#define MIN_THRESHOLD  300
#define MAX_THRESHOLD  700

// declare the datatype for segments
typedef struct segmentDatatype{
  bool selected;
  int pin;
  segmentDatatype* up;
  segmentDatatype* down;
  segmentDatatype* left;
  segmentDatatype* right;

};

// declare the segments
segmentDatatype segments[SEGSIZE] = {
  {0, pinA, 0, 0, 0, 0},
  {0, pinB, 0, 0, 0, 0},
  {0, pinC, 0, 0, 0, 0},
  {0, pinD, 0, 0, 0, 0},
  {0, pinE, 0, 0, 0, 0},
  {0, pinF, 0, 0, 0, 0},
  {0, pinG, 0, 0, 0, 0},
  {0, pinDP, 0, 0, 0, 0}
};

// declare global variables for the current segment
segmentDatatype *currentSegment;
bool currentSegmentState;



void setup() {

  initSegments();

  Serial.begin(115200);

  for (int i = 0; i < SEGSIZE; i++) {
    pinMode(segments[i].pin, OUTPUT);
  }

  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);

  pinMode(pinSW, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(pinSW), joystickPressButtonChange, CHANGE);

}

void loop() {
  
  //read the values of the joystick throughout the program
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  
  // make the current led blink and the selected segments light throughout the program 
  blink();
  updateLeds();


  // if movement is detected, the current segment's value changes
  // joymoved is checked to be false to make sure the joystick was returned to netural position
  // the neighbour that corresponds to the direction detected needs to be other than 0, otherwise there is
  // no need to change the current segment

  //left direction
  if (xValue < MIN_THRESHOLD && joyMoved == false && currentSegment->left != 0) {
    joyMoved = true;
    currentSegment = currentSegment->left;
  }
  
  //right direction
  if (xValue > MAX_THRESHOLD && joyMoved == false && currentSegment->right != 0) {
    joyMoved = true;
    currentSegment = currentSegment->right;

  }
  
  //up direction
  if (yValue < MIN_THRESHOLD && joyMoved == false && currentSegment->up != 0) {
    joyMoved = true;
    currentSegment = currentSegment->up;

  }

  //down direction
  if (yValue > MAX_THRESHOLD && joyMoved == false && currentSegment->down != 0) {
    joyMoved = true;
    currentSegment = currentSegment->down;

  }

  //forcing user to return joystick to original position, otherwise, next moves will produce no efect 
  if (xValue >= MIN_THRESHOLD && xValue <= MAX_THRESHOLD && yValue >= MIN_THRESHOLD && yValue <= MAX_THRESHOLD) {
    joyMoved = false;
  }
}



// initSegments initialises all neighbours for each segment and the current segment as the DP segment
void initSegments(){
  segments[0].down = &segments[6];
  segments[0].left = &segments[5];
  segments[0].right = &segments[1];

  segments[1].up = &segments[0];
  segments[1].down = &segments[6];
  segments[1].left = &segments[5];

  segments[2].up = &segments[6];
  segments[2].down = &segments[3];
  segments[2].left = &segments[4];
  segments[2].right = &segments[7];

  segments[3].up = &segments[6];
  segments[3].left = &segments[4];
  segments[3].right = &segments[2];

  segments[4].up = &segments[6];
  segments[4].down = &segments[3];
  segments[4].right = &segments[2];
  
  segments[5].up = &segments[0];
  segments[5].down = &segments[6];
  segments[5].right = &segments[1];

  segments[6].up = &segments[0];
  segments[6].down = &segments[3];

  segments[7].left = &segments[2];

  currentSegment = &segments[7];
  currentSegmentState = 1;
}


// function makes the current segment blink by changing the state after "BLINKING_PERIOD" has passed
// the output is then changed based on the current segment's state  
void blink(){
  if(millis() - lastTimeCheckForBlinking > BLINKING_PERIOD){
    currentSegmentState = !currentSegmentState;
    lastTimeCheckForBlinking = millis();
  }

}

// function makes the segments light or not based on the "selected" atribute 
// and the current segment based on the state which is changing in order to blink
void updateLeds(){
  for(int i = 0; i < SEGSIZE; i++){
    if(&segments[i] != currentSegment){
      digitalWrite(segments[i].pin, segments[i].selected);
    }
    digitalWrite(currentSegment->pin, currentSegmentState);
  }

}

// checks if the joystick is at the neutral position for the axis
bool neutralPosition(int axis){
  if(axis >= MIN_THRESHOLD  && axis <= MAX_THRESHOLD)
    return true;
  return false;
}

// the function is called when a change in the joystick pressbutton is detected and based on the 
// time between press and release it is determined if a segment was selected or a reset was requested
// and a fuction is called acordingly
void joystickPressButtonChange(){
  if(micros() - debounce < DEBOUNCE_TIME){
    return;
  }

  if(pressState == RISING){
    startPressingButtonTime = micros();
    Serial.print("Inceput\n");
  }
  
  if(pressState == FALLING){
    clickDuration = micros() - startPressingButtonTime;
    Serial.print(clickDuration);
    Serial.print("\n");
    Serial.print("Sfarsit\n");
    Serial.print("\n");
    if(clickDuration >= MIN_CLICK_TIME_SELECT && clickDuration < MIN_CLICK_TIME_RESET){
      Serial.print("Am intrat pe select\n");
      select();
    }

    else if(clickDuration >= MIN_CLICK_TIME_RESET){
      Serial.print("Am intrat pe reset\n");
      reset();
    }
  }
  pressState = !pressState;
  debounce = micros();
}


// function makes the selected segment change state
void select(){
  currentSegment->selected = !currentSegment->selected;
  updateLeds();

}

// function turns off all leds and resets the current segment as the DP segment
void reset(){
  for(int i = 0; i < SEGSIZE; i++){
    segments[i].selected = LOW;
  }
  currentSegment->selected = LOW;
  currentSegment = &segments[7];

}