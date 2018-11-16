# DWM1000_Copter_Integration

For a starting point look into main.cpp and node_config.h. The program consists of 3 major components - ranging, multilateration and wirless communication.

## Importing the project

* ```git clone``` the repo
* virtualenv for the new folder
  * python2
  * install mbed-cli module
  * add virtualenv-folder to ```.mbedignore``` file
* checkout the submodule
* set target with ```mbed target ...```, set toolchain with ```mbed toolchain GCC_ARM```

## Building
* (load the right python env with pyhon2 and the mbed package installed)
* ```mbed compile --profile debug``` compiles the program

## Flashing

*  The Makefile can be used to flash a node with the black magic probe
* ```ADDR=2 make flash``` is the command
* ```BMP_PORT``` needs to be set to the correct value so the BMP is recognized

## Address Space

* 0/255: Broadcast
* 1: Ground-Station
* 2: Default (should not be used in newly flashed)
* 3 - (MLAT_BASE_ADDR-1): normal nodes, that regularly measure their range with node 1 and all anchor-nodes
* MLAT_BASE_ADDR-254: anchor nodes, that do note actively start range_readings 
