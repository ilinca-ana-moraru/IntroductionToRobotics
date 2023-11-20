#include <EEPROM.h>

const int LDRSensorPin = A5;
float LDRSensorValue = 0;

float LDRDisplayValue = 0;

const int trigPin = 4;
const int echoPin = 5;

const int redPin = 9;
const int bluePin = 10;
const int greenPin = 11;

int chosenSubmenu = 0;
bool inMainMenu = 1;
int chosenSubmenuFunctionality = 0;

int sensorsSamplingInterval = 3;
int UltrasonicAlertThreshold = 5;
int LDRAlertThreshold = 30;

#define MILLIS_TO_SECOND 1000
unsigned long lastDistanceRead = 0;



int duration;
int distance;

#define START_OF_DISTANCE_STORRING_SPACE 230
#define START_OF_LIGHT_STORRING_SPACE 260
// #define NR_OF_DISTANCE_MEASUREMENTS_STORRING_SPACE 54
// #define INDEX_OF_NEWEST_MEASUREMENT_STORRING_SPACE 146
#define NR_OF_BYTES_OF_INT 2
int nrOfDistanceMeasurements = 0;
int nrOfLightMeasurements = 0;
#define NR_OF_MEASUREMENTS 10

bool currentActionPrinted = 0;
bool wasAreYouSureAsked = 0;

bool showLastMeasurement = 0;
char potentialEscape;

int indexOfNewestDistanceMeasurement = -1;
int indexOfNewestLightMeasurement = -1;

bool automaticLEDMode = 1;

int customRed, customGreen, customBlue;
int autoRed, autoGreen, autoBlue = 0;

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  printMainMenu();

}

void loop() {

  // alegi submeniul
  if(inMainMenu == 1){
    if(Serial.available() > 0){
      chosenSubmenu = Serial.parseInt();
      chooseFromMainMenu();
    }
  }
  // alegi functionalitatea din submeniu
  else if(chosenSubmenu){
    if(chosenSubmenuFunctionality == 0 && Serial.available() > 0){
      chosenSubmenuFunctionality = Serial.parseInt();
    }
    if(chosenSubmenuFunctionality){
      switch (chosenSubmenu){
      case 1:
        sensorSettings();
        break;
      
      case 2:
        resetLoggerData();
        break;

      case 3:
        systemStatus();
        break;

      case 4:
        RGBControl();

      default:
        break;
      }
    }
  }
  if(millis() - lastDistanceRead > (MILLIS_TO_SECOND * sensorsSamplingInterval)){
    lastDistanceRead = millis();
    measureDistance();
    measureLight();
    if(showLastMeasurement == 1){
      showLastDistance();
      showLastLight();
      Serial.println();
    }
  }

  colorLeds();

}


void colorLeds(){
  if(automaticLEDMode == 0){
    analogWrite(redPin, customRed);
    analogWrite(greenPin, customGreen);
    analogWrite(bluePin, customBlue);
  }
  else{
    if(EEPROM[START_OF_LIGHT_STORRING_SPACE + (indexOfNewestLightMeasurement) *  NR_OF_BYTES_OF_INT] > LDRAlertThreshold ||
      EEPROM[START_OF_DISTANCE_STORRING_SPACE + (indexOfNewestDistanceMeasurement) *  NR_OF_BYTES_OF_INT] < UltrasonicAlertThreshold){
      autoRed = 255;
      autoGreen = 0;
    }
    else{
      autoGreen = 255;
      autoRed = 0;
    }
    analogWrite(redPin, autoRed);
    analogWrite(greenPin, autoGreen);
    analogWrite(bluePin, autoBlue);

  }
}


void reset(){
 nrOfDistanceMeasurements = -1;
 nrOfLightMeasurements = -1;
}


void measureLight(){
  float LDRSensorValue = analogRead(LDRSensorPin);
  LDRDisplayValue = map(LDRSensorValue, 0, 1023, 0, 100);

  EEPROM.put(START_OF_LIGHT_STORRING_SPACE + indexOfNewestLightMeasurement*NR_OF_BYTES_OF_INT, int(LDRDisplayValue));
  if(nrOfLightMeasurements < NR_OF_MEASUREMENTS){
    nrOfLightMeasurements++;
  }
  indexOfNewestLightMeasurement = (indexOfNewestLightMeasurement+1)%(NR_OF_MEASUREMENTS - 1);

}

void showLastLight(){
  if(indexOfNewestLightMeasurement > -1){
    int lastLight = EEPROM[START_OF_LIGHT_STORRING_SPACE + (indexOfNewestLightMeasurement) *  NR_OF_BYTES_OF_INT];
    Serial.print(F("Brightness: "));
    Serial.print(lastLight);
    Serial.println("/100");
  }
}

void showLoggedLight(){
  Serial.println();
  for(byte measurement = 0; measurement < nrOfLightMeasurements; measurement++){
    int currentMeasurement = 0;
    currentMeasurement = EEPROM[START_OF_LIGHT_STORRING_SPACE + (indexOfNewestLightMeasurement + measurement)% (NR_OF_MEASUREMENTS - 1)*  NR_OF_BYTES_OF_INT];
    Serial.print(F("Brightness "));
    Serial.print(measurement+1);
    Serial.print(F(" : "));
    Serial.print(currentMeasurement);
    Serial.print(F("/100"));
    Serial.println();
  }
  Serial.println();
}


void measureDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  // Serial.print("measured distance: ");
  // Serial.println(distance);
  EEPROM.put(START_OF_DISTANCE_STORRING_SPACE + indexOfNewestDistanceMeasurement*NR_OF_BYTES_OF_INT, int(distance));
  if(nrOfDistanceMeasurements < NR_OF_MEASUREMENTS){
    nrOfDistanceMeasurements++;
  }
  indexOfNewestDistanceMeasurement = (indexOfNewestDistanceMeasurement+1)%(NR_OF_MEASUREMENTS - 1);
  

}

void showLastDistance(){
  if(indexOfNewestDistanceMeasurement > -1){
    int lastDistance = EEPROM[START_OF_DISTANCE_STORRING_SPACE + (indexOfNewestDistanceMeasurement) *  NR_OF_BYTES_OF_INT];
    Serial.print(F("Distance: "));
    Serial.println(lastDistance);

  }
}

void showLoggedDistance(){
  Serial.println();
  for(byte measurement = 0; measurement < nrOfDistanceMeasurements; measurement++){
    int currentMeasurement = 0;
    currentMeasurement = EEPROM[START_OF_DISTANCE_STORRING_SPACE + (indexOfNewestDistanceMeasurement + measurement)% (NR_OF_MEASUREMENTS - 1)*  NR_OF_BYTES_OF_INT];
    Serial.print(F("Distance "));
    Serial.print(measurement+1);
    Serial.print(" : ");
    Serial.print(currentMeasurement);
    Serial.println();
  }
  Serial.println();
}

void resetToMainMenu(){
      inMainMenu = 1;
      chosenSubmenuFunctionality = 0;
      chosenSubmenu = 0;
			currentActionPrinted = 0;
      wasAreYouSureAsked = 0;
      showLastMeasurement = 0;
      printMainMenu();

}

void printMainMenu() {
    Serial.println();
    Serial.print(F("Main Menu\n"));
    Serial.print(F("1. Sensor Settings\n"));
    Serial.print(F("2. Reset Logger Data\n"));
    Serial.print(F("3. System Status\n"));
    Serial.print(F("4. RGB LED Control\n"));
    Serial.println();
}
void chooseFromMainMenu(){
    switch (chosenSubmenu)
    {
    case 1:
      Serial.print(F("1. Sensor Settings\n"));
      Serial.print(F("  1. Sensors Sampling Interval\n"));
      Serial.print(F("  2. Ultrasonic Alert Threshold\n"));
      Serial.print(F("  3. LDR Alert Threshold\n"));
      Serial.print(F("  4. Back\n"));
      inMainMenu = 0;
      break;
    
    case 2:
      Serial.print(F("2. Reset Logger Data\n"));
      Serial.print(F("  1. Yes\n"));
      Serial.print(F("  2. No\n"));
      inMainMenu = 0;
      break;
    
    case 3:
      Serial.print(F("3. System Status\n"));
      Serial.print(F("  1. Current Sensor Readings\n"));
      Serial.print(F("  2. Current Sensor Settings\n"));
      Serial.print(F("  3. Display logged Data\n"));
      Serial.print(F("  4. Back\n"));
      inMainMenu = 0;
      break;
    
    case 4:
      Serial.print(F("4. RGB LED Control\n"));
      Serial.print(F("  1. Manual Color Control\n"));
      Serial.print(F("  2. LED: Toggle Automatic ON/OFF\n"));
      Serial.print(F("  3. Back\n"));
      inMainMenu = 0;
      break;

    default:
      Serial.print(F("Wrong input.\n"));
      break;
    }
}

void sensorSettings(){
	
  if(currentActionPrinted == 0){
		// Serial.print("currentActionPrinted: ");
		// Serial.println(currentActionPrinted);
    currentActionPrinted = 1;
    switch (chosenSubmenuFunctionality)
    {
    case 1:
      Serial.print(F("Sensors Sampling Interval\n"));
      Serial.print(F("Type the desired period (seconds) for measuring with the sensors.\n"));
      break;
    
    case 2:
      Serial.print(F("Ultrasonic Alert Threshold\n"));
      Serial.print(F("Type the desired minimum threshold for distance alert.\n"));

      break;

    case 3:
      Serial.print(F("LDR Alert Threshold\n"));
      Serial.print(F("Type the desired maximum threshold for luminosity alert.\n"));
      break;

    case 4:
			Serial.print(F("Going back to main menu\n"));
      break;

    default:
      break;
    }
  }

  else{

    switch(chosenSubmenuFunctionality){
      case 1:
				if(Serial.available() > 0){
					sensorsSamplingInterval = Serial.parseInt();
					if(sensorsSamplingInterval < 1 || sensorsSamplingInterval > 100){
						Serial.print("Value needs to be between 1 and 100.\n");
					}
					else{
            Serial.print("Changed sampling interval value to ");
            Serial.println(sensorsSamplingInterval);
						resetToMainMenu();
					}
				}
        break;

      case 2:
				if(Serial.available() > 0){
					UltrasonicAlertThreshold = Serial.parseInt();
					if(UltrasonicAlertThreshold < 1 || UltrasonicAlertThreshold > 300){
						Serial.print("Value needs to be between 1 and 300.\n");
					}
					else{
            Serial.print("Changed ultrasonic alert threshold value to ");
            Serial.println(UltrasonicAlertThreshold);
						resetToMainMenu();
					}
				}
        break;

      case 3:
				if(Serial.available() > 0){
					LDRAlertThreshold = Serial.parseInt();
					if(LDRAlertThreshold < 1 || LDRAlertThreshold > 100){
						Serial.print("Value needs to be between 1 and 100.\n");
					}
					else{
            Serial.print("Changed LDR alert threshold value to ");
            Serial.println(LDRAlertThreshold);
						resetToMainMenu();
					}
				}
        break;

			case 4:
			  resetToMainMenu();
				break;

      default:
      	break;
    }

  }


}

void resetLoggerData(){
  switch(chosenSubmenuFunctionality){
    case 1:
    if(wasAreYouSureAsked == 0){
      Serial.print("Are you sure?\n");
      Serial.print("  1.Yes\n");
      Serial.print("  2.No\n");
      wasAreYouSureAsked = 1;
    }
    else if(Serial.available()){
      int areYouSureResponse = Serial.parseInt();
      if(areYouSureResponse == 1){
        reset();
        Serial.println("..Reset...");
      }
      resetToMainMenu();
    }
      break;

    case 2:
      resetToMainMenu();
      break;

    default:
      Serial.print("Wrong input\n");
      break;
  }
}

void systemStatus(){
  if(currentActionPrinted == 0){
    currentActionPrinted = 1;
    switch (chosenSubmenuFunctionality){
    case 1:
      Serial.print("Current Sensor Readings. Use e to escape.\n");
      break;
    
    case 2:
      Serial.print("Current Sensor Settings\n");
      break;

    case 3:
     Serial.print("Current Sensor Logged Data\n");
     break;

    case 4:
      Serial.print("Back\n");
      break;

    default:
      break;
    }
  }
  else{
    switch(chosenSubmenuFunctionality){
      case 1:
        showLastMeasurement = 1;
        if(Serial.available()){
          potentialEscape = Serial.read();
          if(potentialEscape == 'e' || potentialEscape == 'E'){
            potentialEscape = 0;
            Serial.print("...Exiting display current readings mode...\n");
            resetToMainMenu();
          }
        }
        break;

      case 2:
        Serial.print("Sampling rate: ");
        Serial.println(sensorsSamplingInterval);
        Serial.print("Distance threshold: ");
        Serial.println(UltrasonicAlertThreshold);
        Serial.print("Luminosity threshold: ");
        Serial.println(LDRAlertThreshold);
        resetToMainMenu();
        break;
      
      case 3:
        showLoggedDistance();
        showLoggedLight();
        Serial.println();
        resetToMainMenu();
        break;

      case 4:
        resetToMainMenu();
        break;

      default:
        break;
    }
  }

}

void RGBControl(){
  if(currentActionPrinted == 0){
    currentActionPrinted = 1;
    switch (chosenSubmenuFunctionality){
    
    case 1:
      Serial.print("Manual Color Control\n");
      Serial.print("Type your desired level of brightness on a 1-100 scale for the Red, Green and Blue components.\n");
      Serial.print("Example: 20 70 45\n");
      break;
    
    case 2:
      Serial.print("LED: Toggle Automatic ON/OFF\n");
      break;
    
    case 3:
      Serial.print("Back\n");
      break;

    default:
      break;
    }

  }

  else{
    switch (chosenSubmenuFunctionality){

    case 1:
      if(Serial.available()){
        String manualColors = Serial.readString();
        char *manualColorsChar = new char[manualColors.length() + 1];
        strcpy(manualColorsChar, manualColors.c_str());
        sscanf(manualColorsChar, "%d %d %d", &customRed, &customGreen, &customBlue);
        if(customRed < 1 || customRed > 100 || customGreen < 1 || customGreen > 100 ||
        customBlue < 1 || customBlue > 100){
          Serial.print("Values need to be between 1 and 100.\n");
        }
        else{
          customRed = map(customRed, 1, 100, 0, 255);
          customGreen = map(customGreen, 1, 100, 0, 255);
          customBlue = map(customBlue, 1, 100, 0, 255);
          resetToMainMenu();

        }
      }
      break;

    case 2:
      automaticLEDMode = !automaticLEDMode;
      if(automaticLEDMode == 1){
        Serial.println("Automatic LED Mode ON");
      }
      else{
        Serial.println("Automatic LED Mode OFF");
      }
      resetToMainMenu();
      break; 

    case 3:
      resetToMainMenu();
      break;

    default:
      break;
    }
  }
}

