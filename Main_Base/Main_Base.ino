// Emerson Maki
// 04/19/2019
// Primary control station.  
// Theory of operation documented within README. 
// 
// Utilizes the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// Big Thanks to Mike Grusin for SparkFun's hook up guide
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide

#include <RFM69.h>
#include <SPI.h>

// Radio Specific:
const int networkID = 127;    // Entire Home network
const int stationID = 244;    // Just this station ID
const int baudRate = 9600;
RFM69 radio;

// Transmission LED - Diagnostic
int statusLED = 4;
int receivedCountDaily = 0;
int dayCount = 0;

void setup() {
  Serial.begin(baudRate);
  Serial.println("Main Base online");

  pinMode(statusLED, OUTPUT);
  radio.initialize(RF69_915MHZ, stationID, networkID);
  radio.setHighPower();
}

void loop() {
  if (radio.receiveDone()){
    byte valueArray[radio.DATALEN];
    Serial.print("{Today: ");
    Serial.print(receivedCountDaily);
    Serial.print(" Day: ");
    Serial.print(dayCount);
    Serial.print("} ");
    Serial.print("Node: ");
    Serial.println(radio.SENDERID, DEC);

    // We need to parse the received data package. 
    for (byte i = 0; i < radio.DATALEN; i++){
      byte receivedValue = (byte)radio.DATA[i];
      valueArray[i] = receivedValue;
    }
    
    int outputPackage[radio.DATALEN/2];
    int outputSize = radio.DATALEN/2;
    byteArryToIntArry(valueArray, radio.DATALEN, outputPackage, outputSize);
    procesesStationMsg(outputPackage, outputSize, radio.SENDERID);
    
    Serial.print("RSSI ");
    Serial.println(radio.RSSI);
    Serial.println("");
    receivedCountDaily++;
    if (receivedCountDaily > 32400){
      // 86400 seconds in a day. 
      // Each station submits a msg every 8 seconds.
      // 10800 messages every day per station
      // 3 stations
      // 32400 messages in a day. 
      receivedCountDaily = 0;
      dayCount++;
    }
  }

}

void byteArryToIntArry(byte input[], int sizeOfInput, int output[], int sizeOfOutput){
  int count = 0;
  // loop through each output array element, and for every output value, convert two bytes. 
  for (int i = 0; i < sizeOfOutput; i++){
    // Convert two bytes into one int
    output[count] = (input [(2 * count)] << 8 | input[(2 * count) + 1]); //
    count ++;
  }
}

void procesesStationMsg(int input[], int inputSize, int stationID){
  
  Serial.print("Temp: ");
  Serial.print(input[0]);
  Serial.println(" °F");
  
  Serial.print("Light: ");
  Serial.println(input[1]);
  
  switch (stationID){
    case 1:
      Serial.print("Dryer motion: ");
      Serial.println(input[2]);
      Serial.print("Dryer State: ");
      Serial.println(input[3]);
      break;
    case 2:
      Serial.print("Door Count: ");
      Serial.println(input[2]);
      break;
    case 3:
      Serial.print("Soil Water Level: ");
      Serial.println(input[2]);
      
      Serial.print("Soil State: ");
      Serial.println(input[3]);
      break;
    case 4:
      Serial.println("Room 2 ");
      Serial.print("Temp: ");
      Serial.print(input[2]);
      Serial.println(" °F");
      Serial.print("Light: ");
      Serial.println(input[3]);
      
      Serial.println("Room 3 ");
      Serial.print("Temp: ");
      Serial.print(input[4]);
      Serial.println(" °F");
      Serial.print("Light: ");
      Serial.println(input[5]);
      break;
    default:
      Serial.println("Station is unsupported");
      break;
    }
}
