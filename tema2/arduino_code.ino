#define RedInputPin  A0
#define GreenInputPin  A1
#define BlueInputPin  A2

#define RedLedPin 6
#define GreenLedPin 5
#define BlueLedPin 3

#define MaxInputVal 1023
#define MinInputVal 0
#define MaxLedIntensity 255
#define MinLedIntensity 0


float RedInputValue = 0.0;
float GreenInputValue = 0.0;
float BlueInputValue = 0.0;

float RedValue = 0.0;
float GreenValue = 0.0;
float BlueValue = 0.0;



void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(RedLedPin, OUTPUT);
  pinMode(GreenLedPin,OUTPUT);
  pinMode(BlueLedPin, OUTPUT);


}
// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
    RedInputValue = analogRead(RedInputPin); 
    GreenInputValue = analogRead(GreenInputPin);
    BlueInputValue = analogRead(BlueInputPin);
  // print out the value you read:
    RedValue = map(RedInputValue, MinInputVal, MaxInputVal, MinLedIntensity, MaxLedIntensity);
    GreenValue = map(GreenInputValue, MinInputVal, MaxInputVal, MinLedIntensity, MaxLedIntensity);
    BlueValue = map(BlueInputValue, MinInputVal, MaxInputVal, MinLedIntensity, MaxLedIntensity);

    Serial.println(GreenValue);
  analogWrite(RedLedPin, RedValue);
  analogWrite(GreenLedPin, GreenValue);
  analogWrite(BlueLedPin, BlueValue);



  delay(1);    	// delay in between reads for stability
}
