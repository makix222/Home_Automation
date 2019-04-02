Home_Automation

#Theory of Operation#
5 Control stations are placed in locations around the house to collect information. 
Each control station will transmit a measurement of its sensors every minute. 
These measurements consist of periodic readings, and a record of activations.  
All control stations will transmit to a central main station. 
The main station acts as a wireless home base and will capture every stations measurement.
Once the maine station captures the information from a station, it will push the measurement to a database. 
The database will be reachable through a local web interface. 

#Control Stations#
Each control station will measure temperature and light and a unique other signal.
Unique Signals:
Station 1:
	Cycle status of Washer and Dryer
Station 2: 
	Main outside door open/closed status count since last transmission
	Count of entities that have passed through the door
Station 3:
	Soil water levels
	local humidity of plant corner. 
Station 4: 
	Light status of multiple lights located in kitchen
	Automatic Light control of night light. 
Station 5:
	Light status of multiple upstairs rooms. 	
