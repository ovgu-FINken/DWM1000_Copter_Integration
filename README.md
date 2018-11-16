# DWM1000_Copter_Integration

For a starting point look into main.cpp and node_config.h. The program consists of 3 major components - ranging, multilateration and wirless communication.


## Building
* (load the right python env with pyhon2 and the mbed package installed)
* ```mbed compile --profile debug``` compiles the program

## Flashing

*  The Makefile can be used to flash a node with the black magic probe
* ```ADDR=2 make flash``` is the command
* ```BMP_PORT``` needs to be set to the correct value so the BMP is recognized
