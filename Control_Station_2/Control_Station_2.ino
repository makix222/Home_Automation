// Emerson Maki
// 04/01/2019
// This bit here will live on Control Station 2. 
// Theory of operation documented within README. 
// 
// Utilizes the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// Big Thanks to Mike Grusin for SparkFun's hook up guide
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide

// Front Door monitor using Ultrasound Sensor

#include <RFM69.h>
#include <LowPower.h>
#include <SPI.h>


// Radio Specific:
const int networkID = 127;  // Entire Home network
const int stationID = 2;    // Just this station ID
const int baudRate = 9600;

RFM69 radio;

int statusLED = 4;

// Main Light Sensor 
int lightPin = A7;
int lightRead = 0;

// Main Temp Sensor
int tempPin = A6;
int tempRead;
float tempC, tempF;

// Station 2, Ultrasound Specifc
int trigPin = 6;
int echoPin = 7;
long period = 0;
int distance = 0;
int doorOpenPin = 8;


void setup() {
  pinMode(statusLED, OUTPUT);
  digitalWrite(statusLED, HIGH); 
  
  // All Analog sensors are set to measure between 0v and 1.1v
  analogReference(INTERNAL);

  // Serial communication right now for simple debugging.
  Serial.begin(baudRate);
  Serial.print("Control Station ");
  Serial.print(stationID);
  Serial.println(" online");

  pinMode(lightPin, INPUT);
  pinMode(tempPin, INPUT);

  radio.initialize(RF69_915MHZ, stationID, networkID);
  radio.setHighPower();

  // Station 2
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(doorOpenPin, INPUT);

  // Status Startup Complete
  digitalWrite(statusLED, LOW);
  blink();
}

void SensorRead(byte input[]){
  // Control Station 1 purpose:
  // Log Temp, light, and 2 Analog inputs to monitor basement.
  // Analog input 1 will indicate when washer cycle finishes.
  // Analog input 2 will indicate when dryer cycle finishes.  
  lightRead = analogRead(lightPin);
  tempRead = analogRead(tempPin);
  delay(5); // maybe a bit agressive of a delay. 

  tempC = tempRead / 9.3;
  tempF = (tempC * 1.8) + 32;  

  // Serial Debugging
  Serial.println(" ");   
  Serial.print(tempF);
  Serial.println(" °F");
  Serial.print("Light Reading: ");
  Serial.println(lightRead);
  Serial.println(" ");   

  // Radio
  // Cast the temp reading from float to int. We loose precision here. 
  int transmitF = (int) tempF;
  // Radio communicates using bytes. We need to convert the ints to bytes.
  input[0] = (byte) (transmitF >> 8);
  input[1] = (byte) (transmitF);
  input[2] = (byte) (lightRead >> 8);
  input[3] = (byte) (lightRead);

}

void BroadcastValues(byte packageBuffer[], int packageLength){
  radio.send(244, packageBuffer, packageLength);
  blink();
}

void loop() {
  delay(100);
  
  int valuesSize = 6; // How many bytes to send? 
  byte sensorValues[valuesSize];
  SensorRead(sensorValues);  
  doorPassCount(sensorValues);
  BroadcastValues(sensorValues, valuesSize);
  
  delay(3000);
}

void doorPassCount(byte input[]){
  int count = 0;
  int distance = DistanceToDoor();
  int prevDistance = DistanceToDoor();
  bool doorOpen;
  do{
    delay(50);
    int distDiff = 0;
    doorOpen = digitalRead(doorOpenPin);
    distance = DistanceToDoor();
    distDiff = abs(distance - prevDistance);
    if(distDiff > 6){
      // if the measured distance changes more then 3 inches
      Serial.print("Distance :");
      Serial.println(distance);
      Serial.print("distDiff :");
      Serial.println(distDiff);
      count++;
    }    
    prevDistance = distance;
  }while(doorOpen);  // Measure the distance while the door is open
  
  input[4] = (byte) (count >> 8);
  input[5] = (byte) (count);
}

int DistanceToDoor(){
  // Transients must be supressed for a moment. 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);

  // Per datasheet: triggered with minimum 10us pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(13);
  digitalWrite(trigPin, LOW);
  
  // pulse in measures length that first value was changed for. Requires Long datatype. 
  period = pulseIn(echoPin, HIGH);

  int distance;
  distance = (period / 2) / 74;  // convert to inches
  return distance;
}

void blink(){
  digitalWrite(statusLED, HIGH);
  delay(20);
  digitalWrite(statusLED, LOW);
  delay(20);
}
