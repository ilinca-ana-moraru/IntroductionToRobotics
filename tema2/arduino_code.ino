
#define RedInputPin  A0
#define GreenInputPin  A1
#define BlueInputPin  A2

#define RedLedPin 6
#define GreenLedPin 5
#define BlueLedPin 3


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
    RedValue = map(RedInputValue, 0, 1023, 0, 255);
    GreenValue = map(GreenInputValue, 0, 1023, 0, 255);
    BlueValue = map(BlueInputValue, 0, 1023, 0, 255);

    Serial.println(GreenValue);
  analogWrite(RedLedPin, RedValue);
  analogWrite(GreenLedPin, GreenValue);
  analogWrite(BlueLedPin, BlueValue);



  delay(1);    	// delay in between reads for stability
}