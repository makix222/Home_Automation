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

// Transmission LED - Diganostic
int radioLEDPin = 9;

void setup() {
  Serial.begin(9600);
  Serial.println("Main Base online");

  pinMode(radioLEDPin, OUTPUT);
  radio.initialize(RF69_915MHZ, 244, 127);
  radio.setHighPower();

}

void loop() {
  if (radio.receiveDone()){
    byte valueArray[radio.DATALEN];
    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(": [");

    // We need to parse the received data package. 
    for (byte i = 0; i < radio.DATALEN; i++){
      byte receivedValue = (byte)radio.DATA[i];
      valueArray[i] = receivedValue;
    }
    
    int outputPackage[radio.DATALEN/2];
    int outputSize = radio.DATALEN/2;
    byteArryToIntArry(valueArray, radio.DATALEN, outputPackage, outputSize);
    
    for (int i = 0; i < outputSize; i++){
      switch (i){
        case 0:
          Serial.print(" Temp: ");
          Serial.print(outputPackage[i]);
          Serial.print(" °F");
          break;
        case 1:
          Serial.print(" Light: ");
          Serial.print(outputPackage[i]);
          Serial.print(" ");
          break;
      }
      
    }
    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);
    
  }

}

void byteArryToIntArry(byte input[], int sizeOfInput, int output[], int sizeOfOutput){
  int count = 0;
  // loop through each output array element, and for every output value, convert two bytes. 
  for (int i = 0; i < sizeOfOutput; i++){
    // Convert two bytes into one int
    output[count] = input[(2 * count) + 1] | (input [(2 * count)] << 8);
    count ++;
  }
}
