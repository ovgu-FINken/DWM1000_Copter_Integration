
#ifndef NMEA_GPS_H
#define NMEA_GPS_H

#define ANGLE 0// 0.214730608 // in radians
#define T_X 0
#define T_Y 0
#define S_X 1 //1.44306186
#define S_Y 1 //1.28812618
#define PI 3.14159265

#pragma once
#include "node_config.h"
#include <iomanip>
#include <sstream>
#include <string>
#include <tuple>
#include <math.h>

extern std::string start;
extern std::string end_ ;
extern std::string rmc ;
extern std::string gga;
extern int time_h ;// hh,mm,ss.ss
extern int time_m ;
extern int time_s ;
extern int time_ms ;
extern double base_latt;
extern double base_long;

extern double magic_number;

std::string build_nmea_msg(float x, float y, float z);
// this number is magic and converts change in distance from m to deg (aprox)

#endif
