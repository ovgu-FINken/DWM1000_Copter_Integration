# DWM1000_Copter_Integration

For a starting point look into main.cpp and node_config.h. The program consists of 3 major components - ranging, multilateration and wirless communication.

## Importing the project

* ```git clone``` the repo
* create virtualenv vEnv ```virtualenv --python=python2.7 vEnv```
  * install mbed-cli module ```pip install mbed-cli```
  * install dependancies ```pip install -r mbed-os/requirements.txt```
* checkout the submodule ```git submodule update -i```

## Building
* (load the right python env with pyhon2 and the mbed package installed) ```. vEnv/bin/activate```
* Compile with ```ADDR=<Address of Node> make compile```

## Flashing

*  The Makefile can be used to flash a node with the black magic probe
* ```ADDR=<Address of Node> make flash``` is the command
* ```BMP_PORT``` needs to be set to the correct value so the BMP is recognized

## Address Space

* 0/255: Broadcast
* 1: Ground-Station
* 2: Default (should not be used in newly flashed)
* 3 - (MLAT_BASE_ADDR-1): normal nodes, that regularly measure their range with node 1 and all anchor-nodes
* MLAT_BASE_ADDR-254: anchor nodes, that do note actively start range_readings 
