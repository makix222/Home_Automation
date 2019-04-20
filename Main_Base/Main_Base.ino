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
    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(": [");
    for (byte i = 0; i < radio.DATALEN; i++){
      Serial.print((byte)radio.DATA[i]);
    }
    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);
    
    int outputPackage[radio.DATALEN/2];
    byteToInt((byte)radio.DATA, outputPackage);  
  }

}

void byteToInt(byte input[], int output[]){
  Serial.println("Entered the byteToInt Function: ");
  // Figure out how many ints we are generating
  int outputSize = sizeof(input) / 2;
  Serial.print("OutputSize: ");
  Serial.println(outputSize);
  int count = 0;
  //Convert two bytes into one int
  for (int i = 0; i < outputSize; i++){
    Serial.print("Count value: ");
    Serial.println(count);
    output[count] = input[(2 * count)] | input [1 + (2 * count)] << 8;
    Serial.print("Output Int Value: ");
    Serial.println(output[count]);
    count ++;
  }
}
