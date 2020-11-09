#include "nmea_gps.h"
using namespace std;

string start = "$" ;
string end_ = "*" ;
string rmc = "GPRMC" ;
int time_h = 0;// hh,mm,ss.ss
int time_m = 0;
int time_s = 0;
int time_ms = 0;
float base_latt = 5208.2663;
float base_long = 01138.7620;

float magic_number = 0.00000899928;


int checksum(const char *s) {
    int c = 0;

    while (*s)
        c ^= *s++;

    return c;
}

void update_time()
{
  time_ms = time_ms + (int) (1000/NMEA_UPDATE);
  time_s = (int) (time_ms/1000);
  if(time_s >= 60)
  {
    time_s = 0;
    time_m ++;
  }
  if(time_m >= 60)
  {
    time_m = 0;
    time_h ++;
  }
  if(time_h >= 24)
  {
    time_h = 0;
  }
}

string make_time_string()
{
  string timeString = "";
  if(time_h >= 10){
    string s = to_string(time_h);
    timeString = timeString + s;
  }
  else if(time_h >0){
    string s = to_string(time_h);
    timeString = timeString + "0" + s;
  }
  else{
    timeString = timeString + "00";
  }

  if(time_m >= 10){
    string s = to_string(time_m);
    timeString = timeString + s;
  }
  else if(time_m >0){
    string s = to_string(time_m);
    timeString = timeString + "0" + s;
  }
  else{
    timeString = timeString + "00";
  }

  if(time_s >= 10){
    string s = to_string(time_s);
    timeString = timeString + s;
  }
  else if(time_s >0){
    string s = to_string(time_s);
    timeString = timeString + "0" + s;
  }
  else{
    timeString = timeString + "00";
  }
  return timeString;
}

string make_latt_string(float x)
{
  // type conversion from nmea gps format to normal gps format, then addition, then back
  float base_latt_format2 = (int)base_latt/100;
  float z = base_latt - base_latt_format2 ;
  base_latt_format2 = base_latt_format2 + (z/60);

  float latt_format2 = base_latt + magic_number * x ;

  z = (int)latt_format2/100 ;
  float latt = (latt_format2 -z) * 60 + z ;

  // some magic magic

  std::string s(16, '\0');
  auto written = std::snprintf(&s[0], s.size(), "%.4f", latt);
  s.resize(written);

  return s;
}
string make_long_string(float y)
{
  // type conversion from nmea gps format to normal gps format, then addition, then back
  float base_long_format2 = (int)base_long/100;
  float z = base_long - base_long_format2 ;
  base_long_format2 = base_long_format2 + (z/60);

  float long_format2 = base_long + magic_number * y ;

  z = (int)long_format2/100 ;
  float long_ = (long_format2 -z) * 60 + z ;
  // some magic magic

  std::string s(16, '\0');
  auto written = std::snprintf(&s[0], s.size(), "%.4f", long_);
  s.resize(written);

  return s;
}

string build_nmea_msg(float x, float y, float z)
{
  // TODO: height info not used yet, different msg ?!?
  update_time();
  string msg = "";
  msg = msg + start + rmc; // start byte, type of message
  msg = msg + ",";
  // adding time_stamp
  string timeString = make_time_string();
  msg = msg + timeString;
  msg = msg + ",";
  // declaring the message as valid by adding A
  msg = msg + "A";
  msg = msg + ",";
  // lattitude
  msg = msg + make_latt_string(x);
  msg = msg + ",";
  msg = msg + "N";
  msg = msg + ",";
  // longitude
  msg = msg + make_long_string(y);
  msg = msg + ",";
  msg = msg + "E";
  msg = msg + ",";
  // no speed
  msg = msg + ",";
  // no true course
  msg = msg + ",";
  // date of fix (always the same ... easy)
  msg = msg + "010120";
  msg = msg + ",";
  // no magnetic variation
  msg = msg + ",";
  msg = msg + ",";
  msg = msg + end_ ;
  // checksum
  int checksum_ = checksum(msg.c_str());
  
  std::string s(16, '\0');
  auto written = std::snprintf(&s[0], s.size(), "%02X", checksum_);
  s.resize(written);
  msg = msg + s;
  // terminate message should be: <cr><lf>
  msg = msg + "\n";

  return msg;
}
