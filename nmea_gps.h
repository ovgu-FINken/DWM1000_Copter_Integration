#pragma once
#include <string>
#include "node_config.h"
#include <iomanip>
#include <sstream>



string start = "$" ;
string end = "*" ;
string rmc = "GPRMC" ;
int time_h = 0;// hh,mm,ss.ss
int time_m = 0;
int time_s = 0;
int time_ms = 0;
float base_latt = 5208.2663;
float base_long = 01138.7620;

float magic_number = 0.00000899928; 
// this number is magic and converts change in distance from m to deg (aprox)
