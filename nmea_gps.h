
#ifndef NMEA_GPS_H
#define NMEA_GPS_H

#pragma once
#include "node_config.h"
#include <iomanip>
#include <sstream>
#include <string>


extern std::string start;
extern std::string end_ ;
extern std::string rmc ;
extern int time_h ;// hh,mm,ss.ss
extern int time_m ;
extern int time_s ;
extern int time_ms ;
extern float base_latt;
extern float base_long;

extern float magic_number;

std::string build_nmea_msg(float x, float y, float z);
// this number is magic and converts change in distance from m to deg (aprox)

#endif
