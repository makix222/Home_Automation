// Emerson Maki
// 04/01/2019
// This bit here will live on Control Station 1. 
// Theory of operation documented within README. 
// 
// Utilizes the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// Big Thanks to Mike Grusin for SparkFun's hook up guide
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide

// MPU6050 sensor reading built from examples found here:
// https://playground.arduino.cc/Main/MPU-6050/#short


#include <RFM69.h>
#include <LowPower.h>
#include <SPI.h>
#include <Wire.h>



// Radio Specific:
const int networkID = 127;  // Entire Home network
const int stationID = 1;    // Just this station ID
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

// MPU-6050 Sensor
const int MPUAddress = 0x68;
int AcX,AcY,AcZ;
int prevValues[3];

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

  // Station 1 Startup Sequence
  Wire.begin();
  Wire.beginTransmission(MPUAddress);
  Wire.write(0x6B); // Power register for MPU
  Wire.write(0);    // Wake up
  Wire.endTransmission(true);

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
  int transmitF = (int) tempF; // aaaabbbbccccdddd
  // Radio communicates using bytes. We need to convert the ints to bytes.
  input[0] = (byte) (transmitF >> 8); // 00000000aaaabbbb
  input[1] = (byte) (transmitF);      // XXXXXXXXccccdddd
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
  dryerStatus(sensorValues);
  BroadcastValues(sensorValues, valuesSize);
  
  // Delay is required. Currently set to 1 sec for debugging. 
  // Any delay less then 100 has yet to be tested but is
  // required to properly allow the serial communication
  // to complete before going to sleep. 
  delay(100);
}

void dryerStatus(byte input[]){ 
  int detectionCount = 0;
  
  if (vibrationDetection(input)){
    // Require a repeated acceleration change. 
    // reduces chances of incorrect detection
    for (int cycle = 0; cycle < 4; cycle++){
      delay(1000);
      if (vibrationDetection(input)){
        detectionCount++;  
      }
    }
  }
  if(detectionCount >= 3){
    Serial.println("Dryer Vibration Detected!");
    input[6] = 0;
    input[7] = (byte) 1;
  }
  else{
    input[6] = 0;
    input[7] = 0;
  }   
}

bool vibrationDetection(byte input[]){  
  int axesCount = 3;
  int AccelValues[axesCount];
  int sensitivity = 2000;

  readAccelSensor(AccelValues);
  int measuredDiff = accelDifference(AccelValues, prevValues);
  for(int i = 0; i < axesCount; i++){
    prevValues[i] = AccelValues[i];
  }
  input[4] = (byte) (measuredDiff >> 8);
  input[5] = (byte) (measuredDiff);
  if (measuredDiff > sensitivity){
    return true;
  }
  else{
    return false;
  } 
}

void readAccelSensor(int input[]){
  Wire.beginTransmission(MPUAddress);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPUAddress,6,true); 
  input[0] = Wire.read()<<8 | Wire.read();  // X axis
  input[1] = Wire.read()<<8 | Wire.read();  // Y axis
  input[2] = Wire.read()<<8 | Wire.read();  // Z axis
}

int accelDifference(int A[], int B[]){
  int xDiff, yDiff, zDiff;
  xDiff = abs(A[0] - B[0]);
  yDiff = abs(A[1] - B[1]);
  zDiff = abs(A[2] - B[2]);
  return (xDiff + yDiff + zDiff);
}



void blink(){
  digitalWrite(statusLED, HIGH);
  delay(20);
  digitalWrite(statusLED, LOW);
  delay(20);
}
