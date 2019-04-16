// Emerson Maki
// 04/16/2019
// Example code just to verify communication with the RFM69 modules
// Based upon the example code found here:
// https://github.com/LowPowerLab/RFM69/blob/master/Examples/Struct_send/Struct_send.ino
// Partial copyright: Felix Rusu 2018, http://www.LowPowerLab.com/contact
// Felix comments will be labled as such. 

#include RFM69.h
#include RFM69_ATC.h
#include SPIFlash.h
#include SPI.h


//********************************************
// Important Configs. 
// Todo: Will need to make these configs modified based upon station select
//********************************************
#define NODEID      99
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "NoThisIsNotWhatIUsed" // This is changed later. 
#define ENABLE_ATC //Felix: Auto Transmission Control

#define SERIAL_BAUD 9600  // Can be increased. Just a simple throwback. 

#ifdef ENABLE_ATC
  RFM69_ATC radio;  // I removed a #else here since this is going to be hardcoded
#endif
//*********************************************

int TRANSMITPERIOD = 300; //transmit a packet to gateway so often (in ms)
byte sendSize=0;
boolean requestACK = false;
long lastPeriod = -1;

typedef struct {
  int           nodeId; //Felix: store this nodeId
  unsigned long uptime; //Felix: uptime in ms
  float         temp;   //Felix: temperature maybe?
} Payload;
Payload theData;  // I am confused about this. Todo: Learn what this is.

void setup() {
  
 Serial.begin(SERIAL_BAUD);
 radio.initialize(FREQUENCY,NODEID,NETWORKID);
 radio.setHighPower(); //Felix: RFM69HW/HCW only. 
 radio.encrypt(ENCRYPTKEY);
 char buff[50];
 sprintf(buff, "\nTransmitting at 915 Mhz");
 Serial.println(buff);
 
 if (flash.initialize())
   Serial.println("SPI Flash Init OK!");
 else
   Serial.println("SPI Flash Init FAIL! (is chip present?)");
    
}

void loop() {
  //process any serial input
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input >= 48 && input <= 57) //[0,9]
    {
      TRANSMITPERIOD = 100 * (input-48);
      if (TRANSMITPERIOD == 0) TRANSMITPERIOD = 1000;
      Serial.print("\nChanging delay to ");
      Serial.print(TRANSMITPERIOD);
      Serial.println("ms\n");
    }

    // Looks like this stuff is simple serial commands to do magic stuff.
    // Todo: Figure this out later.
    if (input == 'r') //d=dump register values
      radio.readAllRegs();
    
    if (input == 'd') //d=dump flash area
    {
      Serial.println("Flash content:");
      int counter = 0;

      while(counter<=256){
        Serial.print(flash.readByte(counter++), HEX);
        Serial.print('.');
      }
      while(flash.busy());
      Serial.println();
    }
    if (input == 'e')
    {
      Serial.print("Erasing Flash chip ... ");
      flash.chipErase();
      while(flash.busy());
      Serial.println("DONE");
    }
    if (input == 'i')
    {
      Serial.print("DeviceID: ");
      word jedecid = flash.readDeviceId();
      Serial.println(jedecid, HEX);
    }
  }

  //check for any received packets
  if (radio.receiveDone())
  {
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    Serial.print("   [RX_RSSI:");Serial.print(radio.readRSSI());Serial.print("]");

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.print(" - ACK sent");
      delay(10);
    }
    Serial.println();
  }
  
  int currPeriod = millis()/TRANSMITPERIOD;
  if (currPeriod != lastPeriod)
  {
    //fill in the struct with new values
    theData.nodeId = NODEID;
    theData.uptime = millis();
    theData.temp = 91.23; //it's hot!
    
    Serial.print("Sending struct (");
    Serial.print(sizeof(theData));
    Serial.print(" bytes) ... ");
    if (radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData)))
      Serial.print(" ok!");
    else Serial.print(" nothing...");
    Serial.println();
    lastPeriod=currPeriod;
  }
}
