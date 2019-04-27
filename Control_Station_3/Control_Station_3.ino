// Emerson Maki
// 04/01/2019
// This bit here will live on Control Station 3. 
// Theory of operation documented within README. 
// 
// Utilizes the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// Big Thanks to Mike Grusin for SparkFun's hook up guide
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide


#include <RFM69.h>
#include <LowPower.h>
#include <SPI.h>


// Radio Specific:
const int networkID = 127;  // Entire Home network
const int stationID = 3;    // Just this station ID
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

int waterPin = A5;
const int soilWet = 300;
const int soilDamp = 400;
const int soilDry = 500;
const int soilBare = 600;

void setup() {
  pinMode(statusLED, OUTPUT);
  digitalWrite(statusLED, HIGH); 
  
  // All Analog sensors are set to measure between 0v and 1.1v
  analogReference(INTERNAL);

  // Serial communication for simple debugging.
  Serial.begin(baudRate);
  Serial.print("Control Station ");
  Serial.print(stationID);
  Serial.println(" online");
  
  pinMode(lightPin, INPUT);
  pinMode(tempPin, INPUT);

  radio.initialize(RF69_915MHZ, stationID, networkID);
  radio.setHighPower();

  // Station 3
  pinMode(waterPin, INPUT);

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
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
                SPI_OFF, USART0_OFF, TWI_OFF);

  // This delay and the last delay are vital for correct
  // operation after waking up. Must give time for serial
  // communication to wake up and work correctly. 
  delay(100);

  
  int valuesSize = 8; // How many bytes to send? 
  byte sensorValues[valuesSize];
  SensorRead(sensorValues);  
  PlantWaterNeeds(sensorValues);
  BroadcastValues(sensorValues, valuesSize);
  
  
  // Delay is required. Currently set to 1 sec for debugging. 
  // Any delay less then 100 has yet to be tested but is
  // required to properly allow the serial communication
  // to complete before going to sleep. 
  delay(100);
}

void PlantWaterNeeds(byte input[]){
  int soilLevel = SoilDetect(input);\
  input[6] = 0; // int bytes stuffing
  if(soilLevel > soilBare){
    input[7] = (byte) 1;
  }
  else if(soilLevel > soilDry){
    input[7] = (byte) 2;
  }
  else if(soilLevel > soilDamp){
    input[7] = (byte) 3;
  }
  else if(soilLevel > soilWet){
    input[7] = (byte) 4;
  }
  else{
    input[7] = 0;
  }
  
}

int SoilDetect(byte input[]){
  int value;
  value = analogRead(waterPin);
  delay(2);
  Serial.print("Water Measure: ");
  Serial.println(value);
  input[4] = (byte) (value >> 8);
  input[5] = (byte) (value);
}

void blink(){
  digitalWrite(statusLED, HIGH);
  delay(20);
  digitalWrite(statusLED, LOW);
  delay(20);
}
