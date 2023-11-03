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
const int NA = -1;

const int segSize = 8;
 
unsigned long lastTimeCheckForBlinking = millis();
const int blinkingPeriod = 500;

const int minSelectClickingTime = 1000000 * 0.1;
const int minResetClickingTime = 1000000 * 0.5;

byte swState = LOW;
byte lastSwState = LOW;
unsigned long startPressingButtonTime;

int xValue = 0;
int yValue = 0;
bool joyMoved = false;
int minThreshold = 400;
int maxThreshold = 600;

int segments[segSize] = { 
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};



typedef struct segmentDatatype{
  bool state;
  int pin;
  segmentDatatype* up;
  segmentDatatype* down;
  segmentDatatype* left;
  segmentDatatype* right;

};

  segmentDatatype segmentA = {0, pinA, 0, 0, 0, 0};
  segmentDatatype segmentB = {0, pinB, 0, 0, 0, 0};
  segmentDatatype segmentC = {0, pinC, 0, 0, 0, 0};
  segmentDatatype segmentD = {0, pinD, 0, 0, 0, 0};
  segmentDatatype segmentE = {0, pinE, 0, 0, 0, 0};
  segmentDatatype segmentF = {0, pinF, 0, 0, 0, 0};
  segmentDatatype segmentG = {0, pinG, 0, 0, 0, 0};
  segmentDatatype segmentDP = {0, pinDP, 0, 0, 0, 0};
  segmentDatatype segmentNA;


  segmentDatatype *currentSegment;


void joystickPressButtonChange(){
  swState = !digitalRead(pinSW);
  if(swState==1){
    startPressingButtonTime = micros();
  }
  else{
    unsigned long clickDuration = micros() - startPressingButtonTime;
    if(clickDuration > minSelectClickingTime && clickDuration < minResetClickingTime){
      select();
    }
    else if( clickDuration >= minResetClickingTime){
      reset();
    }
  }
}

void select(){
  currentSegment->state = HIGH;
}

void reset(){
  segmentA.state = LOW;
  segmentB.state = LOW;
  segmentC.state = LOW;
  segmentD.state = LOW;
  segmentE.state = LOW;
  segmentF.state = LOW;
  segmentG.state = LOW;
  *currentSegment = segmentDP;

}

void setup() {

  initSegments();

  Serial.begin(115200);

  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }

  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);

  pinMode(pinSW, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(pinSW), joystickPressButtonChange, CHANGE);

}

void loop() {
  
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  
  blink();
  updateLeds();


//left
  if (xValue < minThreshold && neutralPosition(yValue) && joyMoved == false && currentSegment->left !=&segmentNA) {
    changeCurrentSegment();
    currentSegment = currentSegment->left;
  }
//right
  if (xValue > maxThreshold && neutralPosition(yValue) && joyMoved == false && currentSegment->right !=&segmentNA) {
    changeCurrentSegment();
    currentSegment = currentSegment->right;
  }
//up
    if (yValue < minThreshold && neutralPosition(xValue) && joyMoved == false && currentSegment->up !=&segmentNA) {
      changeCurrentSegment();
      currentSegment = currentSegment->up;
  }
//down
    if (yValue > maxThreshold && neutralPosition(xValue) && joyMoved == false && currentSegment->down !=&segmentNA) {
      changeCurrentSegment();
      currentSegment = currentSegment->down;
  }

//forcing user to return joystick to original position 
  if (xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }
  
  

}

void blink(){
if(millis() - lastTimeCheckForBlinking > blinkingPeriod){
  currentSegment->state = !currentSegment->state;
  lastTimeCheckForBlinking = millis();
  // digitalWrite(currentSegment.pin, currentSegment.state);

  }
}

void changeCurrentSegment(){
  currentSegment->state = LOW;
  joyMoved = true;
}

void updateLeds(){
  digitalWrite(segmentA.pin, segmentA.state);
  digitalWrite(segmentB.pin, segmentB.state);
  digitalWrite(segmentC.pin, segmentC.state);
  digitalWrite(segmentD.pin, segmentD.state);
  digitalWrite(segmentE.pin, segmentE.state);
  digitalWrite(segmentF.pin, segmentF.state);
  digitalWrite(segmentG.pin, segmentG.state);
  digitalWrite(segmentDP.pin, segmentDP.state);
  // digitalWrite(currentSegment->pin, currentSegment->state);
}



  void initSegments()
  {
    segmentA.up = &segmentNA;
    segmentA.down = &segmentG;
    segmentA.left = &segmentF;
    segmentA.right = &segmentB;

    segmentB.up = &segmentA;
    segmentB.down = &segmentG;
    segmentB.left = &segmentF;
    segmentB.right = &segmentNA;

    segmentC.up = &segmentG;
    segmentC.down = &segmentD;
    segmentC.left = &segmentE;
    segmentC.right = &segmentDP;

    segmentD.up = &segmentG;
    segmentD.down = &segmentNA; 
    segmentD.left = &segmentE;
    segmentD.right = &segmentC;

    segmentE.up = &segmentG;
    segmentE.down = &segmentD;
    segmentE.left = &segmentNA;
    segmentE.right = &segmentC;
    
    segmentF.up = &segmentA;
    segmentF.down = &segmentG;
    segmentF.left = &segmentNA;
    segmentF.right = &segmentB;

    segmentG.up = &segmentA;
    segmentG.down = &segmentD;
    segmentG.left = &segmentNA;
    segmentG.right = &segmentNA;

    segmentDP.up = &segmentNA;
    segmentDP.down = &segmentNA;
    segmentDP.left = &segmentC;
    segmentDP.right = &segmentNA;

    currentSegment = &segmentDP;
    currentSegment->state = 1;
  }

bool neutralPosition(int axis){
  if(axis >= minThreshold  && axis <= maxThreshold)
    return true;
  return false;
}
