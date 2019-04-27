Home_Automation
# Theory of Operation

5 Control stations are placed in locations around the house to collect information. 
Each control station will transmit a measurement of its sensors every minute. 
These measurements consist of periodic readings, and a record of activations.  
All control stations will transmit to a central main station. 
The main station acts as a wireless home base and will capture every stations measurement.
Once the maine station captures the information from a station, it will push the measurement to a database. 
The database will be reachable through a local web interface. 

<br/><br/>
# Control Stations

Each control station will measure temperature and light and a unique signal.


#### Unique Signals:

- Station 1:
	Cycle status of Washer and Dryer.
	Captured using accell and gyro sensor.
- Station 2: 
	Main outside door open/closed status count since last transmission.
	Count of entities that have passed through the door.
	Captured using a ultrasonic distance sensor.
- Station 3:
	Soil water levels
	local humidity of plant corner. 
	Captured using 2 hydration sensors.
- Station 4: 
	Light status of multiple lights located in kitchen.
	Automatic Light control of night light. 
- Station 5:
	Light status of multiple upstairs rooms. 	

<br/><br/>
## Hardware:

Base station IO - Arduino Nano [Information and data found here](http://avrchip.com/arduino-nano-datasheet-and-tutorial/) running ATmega328P [Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)

Wireless communication  - RFM69HCW [Datasheet](https://cdn.sparkfun.com/datasheets/Wireless/General/RFM69HCW-V1.1.pdf)
<br/><br/>
#### Sensors:

Light Sensor - PDV-P8104 [Datasheet](https://media.digikey.com/pdf/Data%20Sheets/Photonic%20Detetectors%20Inc%20PDFs/PDV-P8104.pdf)

Temp Sensor - LM35 [Datasheet](http://www.ti.com/lit/ds/symlink/lm35.pdf)

Distance Sensor - HCSR04 [Datasheet](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf)

Hydration Sensor 1 - YL69 [Information](https://www.oddwires.com/yl-69-soil-hygrometer-humidity-soil-moisture-detection-sensor/)

Hydration Sensor 2 - K-0135 [Datasheet](http://www.fecegypt.com/uploads/dataSheet/1480850810_water.pdf)

Acceleration/Gyro - GY-521 MPU6050 [Datasheet](http://www.haoyuelectronics.com/Attachment/GY-521/mpu6050.pdf) [Information](https://www.hotmcu.com/gy521-mpu6050-3axis-acceleration-gyroscope-6dof-module-p-83.html)
<br/><br/>
##### Voltage Regulators:

3.3v - LD1117A [Datasheet](https://www.st.com/content/ccc/resource/technical/document/datasheet/a5/c3/3f/c9/2b/15/40/49/CD00002116.pdf/files/CD00002116.pdf/jcr:content/translations/en.CD00002116.pdf)

5v - SCD7800 [Datasheet](https://media.digikey.com/pdf/Data%20Sheets/ON%20Semiconductor%20PDFs/SCD7800_A_Rev4_Sep2011.pdf)
<br/><br/>
### Firmware Dependencies:
From Felix @ LowPowerLabs:

[RFM69HCW library](https://github.com/LowPowerLab/RFM69)

[SPIFlash](https://github.com/LowPowerLab/SPIFlash)
<br/><br/>
From Rock Scream:

[Low Power Library](https://github.com/rocketscream/Low-Power)
<br/><br/>
Standard Arduino:

[SPI Library](https://www.arduino.cc/en/reference/SPI)

## Message Package:
Each station will send a single message that contains all the data from each sensor. 

The message data type is byte

# Station 1:
[(0,1) = Temp; (2,3) = Light; (4,5) = dryer vibration]

# Station 2:
[(0,1) = Temp; (2,3) = Light; (4,5) = Door activation count]

# Station 3:
[(0,1) = Temp; (2,3) = Light; (4,5) = soil water level; (6,7) = soil moisture; (8,9) = humidity]

# Station 4:
[(0,1) = Temp room 1; (2,3) = Light room 1; (4,5) = Temp room 2; (6,7) = Light room 2; (8,9) = Temp room 3; (10,11) = Light room 3]


## theory of message structure
Each station will transmit its message package. 
This message package will contain a collection of integers
The integers array will come from each station with their own order. 
The main station will need to contain logic to understand what station sent the packet. 
Main will then be required to decompose the message and label the data. 









