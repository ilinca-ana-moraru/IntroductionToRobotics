// DS = [D]ata [S]torage - data
// STCP = [ST]orage [C]lock [P]in latch
// SHCP = [SH]ift register [C]lock [P]in clock

const int startStopPin = 2;

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

void setup() {

  pinMode(startStopPin, INPUT_PULLUP);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], displayOff);
  }

  attachInterrupt(digitalPinToInterrupt(startStopPin),startStop, FALLING);
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
}

void startStop(){
  if(micros() - lastStartStopRead < DEBOUNCE_TIME_MICROS){
    return;
  }
  lastStartStopRead = micros();
  isCounting = !isCounting;
  Serial.print("isCounting: ");
  Serial.print(isCounting);
  Serial.println();
  
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


