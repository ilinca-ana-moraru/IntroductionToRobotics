#define button1Pin 8
#define button2Pin 9
#define button3Pin 10


#define led1Pin 2
#define led2Pin 3
#define led3Pin 4

#define blinkingLedPin 5

#define buzzerPin 12
unsigned int buzzerToneClosingDoors = 300;
unsigned int buzzerToneArriving = 500;

unsigned int toneDuration = 1000;

#define debounceTime 50
#define blinkingDuration 30
#define showCurrentFloorLedDuration 3000
#define travelToNextFloorDuration 2000

byte is1FloorCalling = 0;
byte is2FloorCalling = 0;
byte is3FloorCalling = 0;

byte blinkingLedValue = 0;

byte button1Value = 0;
byte button2Value = 0;
byte button3Value = 0;

byte previous1Reading = 0;
byte previous2Reading = 0;
byte previous3Reading = 0;


unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long previousMillisBlinking = 0;
int currentFloor = 1;

void setup() {
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);

  updateLeds();

}


void loop() {

  is1FloorCalling = readButtonValue(button1Pin, previous1Reading);
  is2FloorCalling = readButtonValue(button2Pin, previous2Reading);
  is3FloorCalling = readButtonValue(button3Pin, previous3Reading);


   if(is1FloorCalling == HIGH)
    callLift(1);
  
   if(is2FloorCalling == HIGH)
    callLift(2);

   if(is3FloorCalling == HIGH)
    callLift(3);

 

}

int readButtonValue(int buttonPin,byte previousReading){

    byte  reading = digitalRead(buttonPin);
    byte buttonState; 
    int time;

    if(reading != previousReading){
      time = millis();
    }

    if(millis - time > debounceTime){
      buttonState = !reading;
    }

    return buttonState;
}

void callLift(int floorToAriveAt){

  byte doorsWereClosed = 0;

  if(floorToAriveAt == currentFloor)
    return;

  while(floorToAriveAt - currentFloor != 0){
    int lastFloor = currentFloor;

    // acum plecam, trebuie sa luminam becul si sa facem zgomot in acelasi timp, apoi blinking
    // do once
    if(doorsWereClosed == 0){
      tone(buzzerPin, buzzerToneClosingDoors, toneDuration);
      unsigned long previousMillis = millis();
      currentMillis = millis();
      while(currentMillis - previousMillis <= toneDuration){
        // s a terminat timpul de inchis usile
        currentMillis = millis();
      } 

      doorsWereClosed = 1;
      currentFloor = 0;
      updateLeds(); 
      //blinking 

      for(int i = 1; i <= travelToNextFloorDuration/blinkingDuration; i++){
        tone(buzzerPin, 220, travelToNextFloorDuration/blinkingDuration);
        LiftIsTravelingLedBlink();
      }  
      blinkingLedValue = LOW;
      digitalWrite(blinkingLedPin, blinkingLedValue);


      //change to next floor
      currentFloor = lastFloor;
      if(floorToAriveAt - currentFloor > 0)
        currentFloor += 1;
      else
        currentFloor -= 1;
    
      updateLeds();
    }
    // rest of floors
    else{
      // value for in between floors
      currentFloor = 0;
      updateLeds(); 
      //blinking 
      for(int i = 1; i <= travelToNextFloorDuration/blinkingDuration; i++){
        LiftIsTravelingLedBlink();
        tone(buzzerPin, 220, travelToNextFloorDuration/blinkingDuration); 
      }
      blinkingLedValue = LOW;
      digitalWrite(blinkingLedPin, blinkingLedValue);
  
      //change to next floor
      currentFloor = lastFloor;
      if(floorToAriveAt - currentFloor > 0)
        currentFloor += 1;
      else
        currentFloor -= 1;
    
      updateLeds();
    }

      //in case the new floor is intermediary, make sure the led is on for the time showCurrentFloorLedDuration is set

      if(currentFloor != floorToAriveAt){
          unsigned long previousMillis = millis();
          currentMillis = millis();
          while(currentMillis - previousMillis <= showCurrentFloorLedDuration){
            tone(buzzerPin, 220, showCurrentFloorLedDuration); 
            currentMillis = millis();
            LiftIsTravelingLedBlink();
        } 
      
      }

      if(currentFloor == floorToAriveAt)
        tone(buzzerPin, buzzerToneArriving, toneDuration);
  }
}  


void updateLeds(){
  if(currentFloor == 1)
    button1Value = HIGH;
  else 
    button1Value = LOW;

  if(currentFloor == 2)
    button2Value = HIGH;
  else
    button2Value = LOW;

  if(currentFloor == 3)
    button3Value = HIGH;
  else 
    button3Value = LOW;


  digitalWrite(led1Pin, button1Value);
  digitalWrite(led2Pin, button2Value);
  digitalWrite(led3Pin, button3Value);
}

void LiftIsTravelingLedBlink(){
  unsigned long currentMillisBlink = millis();
  if( currentMillisBlink - previousMillisBlinking >= blinkingDuration){
    previousMillisBlinking = currentMillisBlink;

  if(blinkingLedValue == LOW)
    blinkingLedValue = HIGH;
  else
    blinkingLedValue = LOW;

  digitalWrite(blinkingLedPin, blinkingLedValue);

  }
  else LiftIsTravelingLedBlink();
}

void showCurrentFloor(){

  unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= showCurrentFloorLedDuration){
      // s a terminat timpul de afisat etajul
      previousMillis = currentMillis;

      }
    else
      showCurrentFloor();
}

