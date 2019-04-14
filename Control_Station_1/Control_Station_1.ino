// Emerson Maki
// 04/01/2019
// This bit here will live on Control Station 1. 
// Theory of operation documented within README. 
// 

// Low Power for the sleep states. 
#include "LowPower.h"

// To allow a single firmware to be flashed to any station
// we define three pins to act as a binary count. 
int SelPin[] = {3, 4, 5};
int StationSelect = 0;

// Main Light Sensor 
int lightPin = A7;
float lightRead = 0;

// Main Temp Sensor
int tempPin = A5;
int tempRead;
float tempC, tempF;

void setup() {
  // All Analog sensors are set to measure between 0v and 1.1v
  analogReference(INTERNAL);

  // Serial communication right now for simple debugging.
  Serial.begin(9600);
  Serial.print("Begin");

  
  pinMode(lightPin, INPUT);
  pinMode(tempPin, INPUT);

  // Does the setup, reading, and StationSelect bit math. 
  // This is what defines what station the chip should
  // act like. 
  for (int i = 0; i < 3; i++){
    pinMode(SelPin[i], INPUT);
    int temp = digitalRead(SelPin[i]);
    StationSelect = temp << 1;
  }
}

void StationOne(){
  // Control Station 1 purpose:
  // Log Temp, light, and 2 Analog inputs to monitor basement.
  // Analog input 1 will indicate when washer cycle finishes.
  // Analog input 2 will indicate when dryer cycle finishes.
  
  Serial.print("Station 1 Reporting!");
  
  lightRead = analogRead(lightPin);
  tempRead = analogRead(tempPin);
  delay(5); // maybe a bit agressive of a delay. 

  tempC = tempRead / 9.3;
  tempF = (tempC * 1.8) + 32;
  Serial.print(tempF);
  Serial.println(" °F");
  
  Serial.print(" Light Reading: ");
  Serial.println(lightRead);
  Serial.println(" "); // Debugging Print Statment. 
  
}
void StationTwo(){
  // todo: Write up Sations Operation
  Serial.print("Station 2 Reporting!");
}
void StationThree(){
  // todo: Write up Sations Operation
  Serial.print("Station 3 Reporting!");
}
void StationFour(){
  // todo: Write up Sations Operation
  Serial.print("Station 4 Reporting!");
}
void StationFive(){
  // todo: Write up Sations Operation
  Serial.print("Station 5 Reporting!");
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

  switch (StationSelect) {
    case 0:
      // Todo: Add a warning message that is sent to the 
      // home station to indicate that a chip was incorrectly
      // set up. 
      Serial.print("Incorrect Station!");
      break;
    case 1:
      StationOne();     
      break;
    case 2:
      StationTwo();
      break;
    case 3:
      StationThree();
      break;
    case 4:
      StationFour();
      break;
    case 5:
      StationFive();
      break;
  }

  // Delay is required. Currently set to 1 sec for debugging. 
  // Any delay less then 100 has yet to be tested but is
  // required to properly allow the serial communication
  // to complete before going to sleep. 
  delay(1000);

  // Todo: Clever loops to check on state of Serial communication. 
  

}
