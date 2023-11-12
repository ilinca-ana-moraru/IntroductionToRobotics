// DS = [D]ata [S]torage - data
// STCP = [ST]orage [C]lock [P]in latch
// SHCP = [SH]ift register [C]lock [P]in clock

const int startStopPin = 2;
const byte lapPin = 3;
const byte resetPin = 8;

const byte latchPin = 11; // STCP to 12 on Shift Register
const byte clockPin = 10; // SHCP to 11 on Shift Register
const byte dataPin = 12; // DS to 14 on Shift Register

const byte segD1 = 4;
const byte segD2 = 5;
const byte segD3 = 6;
const byte segD4 = 7;

const byte regSize = 8; // 1 byte aka 8 bits

byte displayDigits[] = {
  segD1, segD2, segD3, segD4
};
const byte displayCount = 4;
const byte encodingsNumber = 10;

byte byteEncodings[encodingsNumber] = {
// ABCDEFGDP 
  B11111100, // 0
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110, // 9
};

const bool displayOn = LOW;
const bool displayOff = HIGH;

byte registers[regSize];

unsigned int currentNumber = 0;
unsigned long lastCountIncrement = 0;
#define COUNTING_DELAY 100

volatile bool isCounting = 0;
volatile unsigned long lastStartStopRead = 0;
#define DEBOUNCE_TIME_MICROS  50000
volatile bool startStopStateButtonRead = 0;

volatile unsigned long lastLapRead = 0;
volatile unsigned long lapRead = 0;

volatile byte numberOfLaps = 0;
volatile int savedLaps[4];
volatile byte indexOfOLdestLap = 0;
volatile byte lastLapIndex;
volatile byte firstLapIndex = 0;
unsigned long previousLapMoment = 0;


#define LAP_CLICK_MIN_DURATION  50000
#define CICLE_THROUGH_LAPS 300000
byte currentDisplayedLapIndex = 0;
volatile bool lapButtonRead = 0;
volatile bool lapPressHappens = 0;
volatile bool mightBeAShortPress = 0;
bool viewingMode = 0;
#define DEBOUNCE_TIME_MILLIS 500
unsigned long resetRead;
byte resetButtonState = 0;
byte lastResetButtonState = 0;

unsigned long timeOfPreviousLapShow = 0;

#define TIME_BETWEEN_SHOWING_LAPS 200000

void setup() {
  Serial.begin(115200);

  pinMode(startStopPin, INPUT_PULLUP);
  pinMode(lapPin, INPUT_PULLUP);
  pinMode(resetPin, INPUT_PULLUP);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], displayOff);
  }

  attachInterrupt(digitalPinToInterrupt(startStopPin),startStop, CHANGE);
  attachInterrupt(digitalPinToInterrupt(lapPin),lap, CHANGE);

}

void loop() {
  // showing the current number and incrementing it if the timer si on
  writeNumber(currentNumber);
  if(isCounting){
    if (millis() - lastCountIncrement > COUNTING_DELAY) {
      lastCountIncrement = millis();
      currentNumber++;
      if (currentNumber == 10000) {
        currentNumber = 0;
      }
    }
  }

  //debounce for reset button
  resetButtonState = !digitalRead(resetPin);
  if(isCounting == 0 && resetButtonState != lastResetButtonState){
    if(millis() - resetRead > DEBOUNCE_TIME_MILLIS){
      resetRead = millis();
      Serial.print("Reset button state: ");
      Serial.print(resetButtonState);
      Serial.println();
      lastResetButtonState = resetButtonState;

      if(resetButtonState == HIGH){
        reset();
      }
    }
  }


  //check if the lap button is currently pressed and minimum time for a press has passed but not 
  // enough for a long press needed to cicle through laps check 
  if(lapPressHappens && (micros() - lapRead) > LAP_CLICK_MIN_DURATION){
    
    if(mightBeAShortPress){
      mightBeAShortPress = 0;

    //if the timer is on, save the lap
      if(isCounting == 1){
        saveLap();
      }

    //if the viewing mode is on, show the first lap
      if(viewingMode == 1){
        timeOfPreviousLapShow = micros();
        Serial.print("Show 1 lap\n");
        showLap();
      }

    }

    // if lap press still happens and mightBeAShortPress is false, the first lap was shown and 
    // now should happen the cicling through laps. If the time between the last shown laps has 
    // passed, is time to display the next lap
    if (viewingMode  && mightBeAShortPress == 0){
      if(micros() - timeOfPreviousLapShow > TIME_BETWEEN_SHOWING_LAPS){
        Serial.print("Cicle through laps\n");
        timeOfPreviousLapShow = micros();
        showLap();

      }
    }
  }
  

}

//debounces the button start/stop and changes the value of isCounting 
void startStop(){
  if(micros() - lastStartStopRead < DEBOUNCE_TIME_MICROS){
    return;
  }
  lastStartStopRead = micros();
  startStopStateButtonRead = !startStopStateButtonRead;
  if(startStopStateButtonRead == HIGH){
    if(viewingMode == 1){
      viewingMode = 0;
      currentNumber = 0;
    }
    isCounting = !isCounting;
    Serial.print("isCounting: ");
    Serial.print(isCounting);
    Serial.println();
  }
}

//debounces the lap button and changes the values of lapPressHappens and mightBeAShortPress
// that have further use in the loop
void lap(){
  if(micros() - lastLapRead < DEBOUNCE_TIME_MICROS){
    return;
  }
  lastLapRead = micros();

  lapButtonRead = !lapButtonRead;
  if(lapButtonRead == HIGH){
    lapRead = micros();
    lapPressHappens = 1;
    mightBeAShortPress = 1;
  }

  if(lapButtonRead == LOW){
    lapPressHappens = 0;    
    mightBeAShortPress = 0;    
  }
}

//saves a new lap
void saveLap(){
  if(numberOfLaps == 0){
    numberOfLaps++;
    savedLaps[0] = currentNumber;
    previousLapMoment = savedLaps[0];
  }

  else if(numberOfLaps < 4){
    savedLaps[numberOfLaps] = currentNumber - previousLapMoment;
    previousLapMoment += savedLaps[numberOfLaps];
    numberOfLaps++;
    lastLapIndex = numberOfLaps - 1;
  }

  else{
    savedLaps[firstLapIndex] = currentNumber - previousLapMoment;
    previousLapMoment += savedLaps[firstLapIndex];
    firstLapIndex = firstLapIndex == 0 ? 3 : firstLapIndex - 1;
    lastLapIndex = lastLapIndex == 0 ? 3 : lastLapIndex - 1; 
  }
  
  for(byte i = 0; i <= numberOfLaps - 1; i++){
    Serial.print("Lap ");
    Serial.print(i);
    Serial.print(" : ");
    Serial.print(savedLaps[i]);
    Serial.println();

  }
  Serial.println();

}

//displays the next lap
void showLap(){
  if(numberOfLaps){
    currentNumber = savedLaps[currentDisplayedLapIndex];
    currentDisplayedLapIndex = (currentDisplayedLapIndex + 1)% numberOfLaps;
  }

}

//resets either the current number either the laps depending of the state of the viewing mode.
// if in counting mode, does nothing
void reset(){
    Serial.print("Am intrat pe reset\n");

  if( isCounting == 1){
    return;
  }
  if(viewingMode == 0){
    viewingMode = 1;
    currentNumber = 0;
    previousLapMoment = 0;
    currentDisplayedLapIndex = 0;
    Serial.print("Am resetat currentNumber\n");
    Serial.print("viewingMode: ");
    Serial.print(viewingMode);
    Serial.println();
  }
  else if(viewingMode == 1){
    //reset laps, no need to reset actual values, they will not be displayed and possibly overriden 
    numberOfLaps = 0;
    currentNumber = 0;
    previousLapMoment = 0;
    viewingMode = 0;
    Serial.print("Am resetat laps si viewingMode\n");
  }
}




//gives the code for the wished digit to the shift register which than will make the digit show
// if the display is on
void writeReg(byte encoding) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
}

//gives 0V to the wished display
void tunOffDisplays() {
  for (byte i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], displayOff);
  }
}

// gives current to the wished display
void tunOnDisplay(byte displayNumber) {
  digitalWrite(displayDigits[displayNumber], displayOn);
}

//decides for each display of the four which digit to write in order to display the wished number
void writeNumber(unsigned int number) {
  int currentDisplay = 3;

  while (currentDisplay >= 0) {
    byte lastDigit = number % 10;
    byte encoding = byteEncodings[lastDigit];

    if (currentDisplay == 2) {
      bitSet(encoding, 0);
    }

    tunOffDisplays();
    writeReg(encoding);
    tunOnDisplay(currentDisplay);

    number /= 10;
    currentDisplay--;
    writeReg(B00000000); // Clear the register to remove ghosting
  }
}


