// Emerson Maki
// 04/01/2019
// This bit here will live on Control Station 1. 
// Theory of operation documented within README. 
// 
// Utilizes the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// Big Thanks to Mike Grusin for SparkFun's hook up guide
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide


#include <RFM69.h>
#include <SPI.h>


// Radio Specific:
const int networkID = 127;  // Entire Home network
const int stationID = 1;    // Just this station ID
const int baudRate = 9600;

#define Encrypt_Key "NotTheRealKey"    // Note To Self: Change this before GIT push
RFM69 radio;

// Transmission LED - Diganostic
int radioLEDPin = 9;

// Main Light Sensor 
int lightPin = A7;
int lightRead = 0;

// Main Temp Sensor
int tempPin = A6;
int tempRead;
float tempC, tempF;

void setup() {
  // All Analog sensors are set to measure between 0v and 1.1v
  analogReference(INTERNAL);

  // Serial communication right now for simple debugging.
  Serial.begin(9600);
  Serial.println("Control Station 1 online");

  pinMode(radioLEDPin, OUTPUT);
  pinMode(lightPin, INPUT);
  pinMode(tempPin, INPUT);

  digitalWrite(radioLEDPin, LOW);

  radio.initialize(RF69_915MHZ, 1, 127);
  radio.setHighPower();
  //radio.encrypt(Encrypt_Key);
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
}

void loop() {
  // Comment only for debugging. Remove both '//' marks to have
  // chip sleep for 8 seconds and defined by first parameter. 
  //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
  //              SPI_OFF, USART0_OFF, TWI_OFF);

  // This delay and the last delay are vital for correct
  // operation after waking up. Must give time for serial
  // communication to wake up and work correctly. 
  delay(100);

  
  int valuesSize = 4; // How many bytes to send? 
  byte sensorValues[valuesSize];
  //for (int a = 0; a < valuesSize; a++){
  //  sensorValues[a] = (byte) 0;
  //}
  SensorRead(sensorValues);  
  BroadcastValues(sensorValues, valuesSize);
  
  
  // Delay is required. Currently set to 1 sec for debugging. 
  // Any delay less then 100 has yet to be tested but is
  // required to properly allow the serial communication
  // to complete before going to sleep. 
  delay(10000);
}
