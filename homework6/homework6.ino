#include <EEPROM.h>


const int trigPin = 4;
const int echoPin = 5;

int menuLevel = 1;
int previousOption = 0;
int chosenSubmenu = 0;
bool inMainMenu = 1;
int chosenSubmenuFunctionality = 0;

int sensorsSamplingInterval = 3;
int UltrasonicAlertThreshold = 10;
int LDRAlertThreshold = 200;

#define MILLIS_TO_SECOND 1000
unsigned long lastDistanceRead = 0;


int duration;
int distance;

#define START_OF_DISTANCE_STORRING_SPACE 230
#define NR_OF_DISTANCE_MEASUREMENTS_STORRING_SPACE 54
#define INDEX_OF_NEWEST_MEASUREMENT_STORRING_SPACE 146
#define NR_OF_BYTES_OF_INT 2
int nrOfDistanceMeasurements = 0;
#define NR_OF_DISTANCE_MEASUREMENTS 10



void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 

  printMainMenu();

}

void loop() {
  if(Serial.available() > 0){
    // alegi submeniul
    if(inMainMenu == 1){
      chosenSubmenu = Serial.parseInt();
      chooseFromMainMenu();
    }
    // alegi functionalitatea din submeniu
    else if(chosenSubmenu && chosenSubmenuFunctionality == 0){
      chosenSubmenuFunctionality = Serial.parseInt();
      switch (chosenSubmenu)
      {
      case 1:
        sensorSettings();
        break;
      
      default:
        break;
      }
    }
      // citesti input pt o functionalitate
    else if(chosenSubmenuFunctionality){
      switch(chosenSubmenu){
        case 1:
          switch(chosenSubmenuFunctionality){
              case 1:
                sensorsSamplingInterval = Serial.parseInt();
                Serial.print(sensorsSamplingInterval);
                if(sensorsSamplingInterval < 1 || sensorsSamplingInterval > 10){
                  Serial.print("Value needs to be between 1 and 10.\n");
                }
                else{
                  resetToMainMenu();
                }
                break;

              case 2:
                UltrasonicAlertThreshold = Serial.parseInt();
                if(UltrasonicAlertThreshold < 1 || UltrasonicAlertThreshold > 30){
                  Serial.print("Value needs to be between x and y.\n");
                }
                else{
                  resetToMainMenu();
                }
                break;

              case 3:
                LDRAlertThreshold = Serial.parseInt();
                if(LDRAlertThreshold < 1 || LDRAlertThreshold > 30){
                  Serial.print("Value needs to be between x and y.\n");
                }
                else{
                  resetToMainMenu();
                }
                break;

              default:
              break;
            }

      case 2:
        switch(chosenSubmenuFunctionality){
          case 1:
            Serial.print("Are you sure?\n");
            Serial.print("  1.Yes\n");
            Serial.print("  2.No\n");
            int areYouSureResponse = Serial.parseInt();
            if(areYouSureResponse == 1){
              reset();
            }
            resetToMainMenu();
            break;

        case 2:
          resetToMainMenu();
          break;
        
        default:
          Serial.print("Wrong input\n");
          break;

        }


      default:
        break;
      }
    }

  }

  if(millis() - lastDistanceRead > (MILLIS_TO_SECOND * sensorsSamplingInterval)){
    // measureDistance();
    // showDistance();
  }



}

void reset(){

}

int indexOfNewestMeasurement = -1;
void measureDistance(){
      lastDistanceRead = millis();
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      distance = duration*0.034/2;
      // Serial.print(distance);
      // Serial.println("masurat");

      EEPROM.put(START_OF_DISTANCE_STORRING_SPACE + indexOfNewestMeasurement*NR_OF_BYTES_OF_INT  ,int(distance));
      if(nrOfDistanceMeasurements < 10){
        nrOfDistanceMeasurements++;
      }
      indexOfNewestMeasurement = (indexOfNewestMeasurement+1)%(NR_OF_DISTANCE_MEASUREMENTS - 1);
      
      // Serial.println();

}

void showDistance(){
  for(byte measurement = 0; measurement < nrOfDistanceMeasurements; measurement++){
    int currentMeasurement = 0;
    currentMeasurement = EEPROM[START_OF_DISTANCE_STORRING_SPACE + (indexOfNewestMeasurement + measurement)% (NR_OF_DISTANCE_MEASUREMENTS - 1)*  NR_OF_BYTES_OF_INT];
    // Serial.print("Memory place accessed: ");
    // Serial.print(START_OF_DISTANCE_STORRING_SPACE + ((indexOfNewestMeasurement + measurement)% (NR_OF_DISTANCE_MEASUREMENTS - 1)) *  NR_OF_BYTES_OF_INT);
    Serial.print("Measurement ");
    Serial.print(measurement);
    Serial.print(" : ");
    Serial.print(currentMeasurement);
    Serial.println();
  }
  Serial.println();
}

void resetToMainMenu(){
      printMainMenu();
      inMainMenu = 1;
      chosenSubmenuFunctionality = 0;
      chosenSubmenu = 0;

}

void printMainMenu() {

    Serial.print("Main Menu\n");
    Serial.print("1. Sensor Settings\n");
    Serial.print("2. Reset Logger Data\n");
    Serial.print("3. System Status\n");
    Serial.print("4. RGB LED Control\n");
}
void chooseFromMainMenu(){
    switch (chosenSubmenu)
    {
    case 1:
      Serial.print("1. Sensor Settings\n");
      Serial.print("  1. Sensors Sampling Interval\n");
      Serial.print("  2. Ultrasonic Alert Threshold\n");
      Serial.print("  3. LDR Alert Threshold\n");
      Serial.print("  4. Back\n");
      inMainMenu = 0;
      break;
    
    case 2:
      Serial.print("2. Reset Logger Data\n");
      Serial.print("  1. Yes\n");
      Serial.print("  2. No\n");
      inMainMenu = 0;
      break;
    
    case 3:
      Serial.print("3. System Status\n");
      Serial.print("  1. Current Sensor Readings\n");
      Serial.print("  2. Current Sensor Readings\n");
      Serial.print("  3. Back\n");
      inMainMenu = 0;
      break;
    
    case 4:
      Serial.print("4. RGB LED Control\n");
      Serial.print("  1. Manual Color Control\n");
      Serial.print("  2. LED: Toggle Automatic ON/OFF\n");
      Serial.print("  3. Back\n");
      inMainMenu = 0;
      break;

    default:
      Serial.print("Wrong input.\n");
      break;
    }
}

void sensorSettings(){
    switch (chosenSubmenuFunctionality)
    {
    case 1:
      Serial.print("Sensors Sampling Interval\n");
      Serial.print("Type the desired period (seconds) for measuring with the sensors.\n");
      break;
    
    case 2:
      Serial.print("Ultrasonic Alert Threshold\n");
      break;

    case 3:
      Serial.print("LDR Alert Threshold\n");
      break;

    case 4:
      resetToMainMenu();
      break;

    default:
      break;
    }
  }


 

