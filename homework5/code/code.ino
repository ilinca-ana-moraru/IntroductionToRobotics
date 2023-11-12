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

volatile byte numberOfLaps = 0;
volatile unsigned long lastLapRead = 0;
volatile unsigned long lapRead = 0;

volatile int savedLaps[4];
volatile byte indexOfOLdestLap = 0;
volatile byte lastLapIndex;
volatile byte firstLapIndex = 0;


#define LAP_CLICK_MIN_DURATION  50000
#define CICLE_THROUGH_LAPS 300000
byte currentDisplayedLapIndex = 0;
volatile bool lapButtonRead = 0;
volatile bool lapPressHappens = 0;
volatile unsigned long lapClickDuration = 0;
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

  if(isCounting == 0){
    if(millis() - resetRead > DEBOUNCE_TIME_MILLIS){
    resetRead = millis();
    resetButtonState = !digitalRead(resetPin);
    if(resetButtonState != lastResetButtonState){
      Serial.print("Reset button state: ");
      Serial.print(resetButtonState);
      Serial.println();
          lastResetButtonState = resetButtonState;

        if(resetButtonState == HIGH){
          reset();
        }
    }
  }

  }
  
  if(lapPressHappens && (micros() - lapRead) > LAP_CLICK_MIN_DURATION && micros()- lapRead < CICLE_THROUGH_LAPS && mightBeAShortPress ){
    mightBeAShortPress = 0;
    if(isCounting == 1){
      saveLap();
    }
    if(viewingMode == 1){
      timeOfPreviousLapShow = micros();
      Serial.print("Show 1 lap\n");
      showLap();

    }
  }

  if (viewingMode && lapPressHappens && (micros() - lapRead) > CICLE_THROUGH_LAPS && mightBeAShortPress == 0){
    if(micros() - timeOfPreviousLapShow > TIME_BETWEEN_SHOWING_LAPS){
      Serial.print("Cicle through laps\n");
      timeOfPreviousLapShow = micros();
      showLap();

    }
  }

}


void startStop(){
  if(micros() - lastStartStopRead < DEBOUNCE_TIME_MICROS){
    return;
  }
  lastStartStopRead = micros();
  startStopStateButtonRead = !startStopStateButtonRead;
  if(startStopStateButtonRead == HIGH){
    isCounting = !isCounting;
    Serial.print("isCounting: ");
    Serial.print(isCounting);
    Serial.println();
  }
}

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


void saveLap(){
  // saves laps
  if(numberOfLaps == 0){
    numberOfLaps++;
    savedLaps[0] = currentNumber;
  }

  else if(numberOfLaps < 4){
    savedLaps[numberOfLaps] = currentNumber - savedLaps[numberOfLaps - 1];
    numberOfLaps++;
    lastLapIndex = numberOfLaps - 1;
  }

  else{
    savedLaps[firstLapIndex] = currentNumber - savedLaps[lastLapIndex];
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

void showLap(){
  if(numberOfLaps){
    currentNumber = savedLaps[currentDisplayedLapIndex];
    currentDisplayedLapIndex = (currentDisplayedLapIndex + 1)% numberOfLaps;
  }

}


void reset(){
    Serial.print("Am intrat pe reset\n");

  if( isCounting == 1){
    return;
  }
  if(viewingMode == 0){
    viewingMode = 1;
    currentNumber = 0;
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
    viewingMode = 0;
    Serial.print("Am resetat laps si viewingMode\n");
  }
}





void writeReg(byte encoding) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
}

void tunOffDisplays() {
  for (byte i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], displayOff);
  }
}

void tunOnDisplay(byte displayNumber) {
  digitalWrite(displayDigits[displayNumber], displayOn);
}

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
    delay(1);

    number /= 10;
    currentDisplay--;
    writeReg(B00000000); // Clear the register to remove ghosting
  }
}


