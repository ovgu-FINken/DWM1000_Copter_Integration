#include "nmea_gps.h"
using namespace std;

string start = "$" ;
string end_ = "*" ;
string rmc = "GPRMC" ;
string gga = "GPGGA" ;
int time_h = 0;// hh,mm,ss.ss
int time_m = 0;
int time_s = 0;
int time_ms = 0;
//double base_latt = 5208.2663; // 5208.261 -->  +0.0053  || 52.13777166  52.13768333
//double base_long = 01138.7620;// 01138.725 --> +0.037   || 11.64603333  11.64541666

double base_latt = 52.1378433 //52.13784504; //52.13777  52,1378433 11,6454139
double base_long = 11.6454139 //11.64542064; //11.64547  11,6454139

double magic_number_latt = 0.00000899928;
double magic_number_long = 0.00001465251;



int checksum(const char *s) {
    int c = 0;

    while (*s)
        c ^= *s++;

    return c;
}

void update_time()
{
  time_ms = time_ms + (int) (1000/NMEA_UPDATE);
  if(time_ms>=1000)
  {
    time_ms = 0;
    time_s ++;
  }
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

  if(time_ms >= 100){
  timeString = timeString + "." + to_string(time_ms);
  }
  else if(time_ms >=10){
    timeString = timeString + ".0" + to_string(time_ms);
  }
  else if(time_ms >= 1){
    timeString = timeString + ".00" + to_string(time_ms);
  }
  else{
    timeString = timeString + ".000"  ;
  }
  return timeString;
}

string make_latt_string(float y)
{
  // add distance in "normal" gps format and then convert to NMEA gps format
  double latt1 = base_latt + magic_number_latt * y ;

  double z = (int)latt1 ;
  double latt = (latt1 -z) * 60 + (z*100) ;

  // some magic magic

  std::string s(16, '\0');
  auto written = std::snprintf(&s[0], s.size(), "%.4f", latt);
  s.resize(written);

  return s;
}
string make_long_string(float x)
{
  // add distance in "normal" gps format and then convert to NMEA gps format
  double long1 = base_long + magic_number_long * x ;

  double z = (int)long1 ;
  double long_ = (long1 -z) * 60 + (z*100) ;

  // some magic magic

  std::string s(16, '\0');
  auto written = std::snprintf(&s[0], s.size(), "%.4f", long_);
  s.resize(written);

  // special for long, leading 0
  string g = "0" + s;

  return g;
}


tuple<float, float> rotate_scale_translate_point(float px, float py){

    float px_rst =  S_X * px * cos(ANGLE) + S_X * py * sin(ANGLE) + T_X;
    float py_rst =  -S_Y * px * sin(ANGLE) + S_Y * py * cos(ANGLE) + T_Y;

    tuple<float,float> returnValue (px_rst, py_rst);
    return returnValue;
}

string build_nmea_RMC(float x, float y, float z){
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
  msg = msg + make_latt_string(y);
  msg = msg + ",";
  msg = msg + "N";
  msg = msg + ",";
  // longitude
  msg = msg + make_long_string(x);
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
  msg = msg + "A" ;// not sure why, but in original msgs there always is a "A" bevor the checksum
  msg = msg + end_ ;
  // checksum
  int checksum_ = checksum(msg.c_str());

  std::string s(16, '\0');
  auto written = std::snprintf(&s[0], s.size(), "%02X", checksum_);
  s.resize(written);
  msg = msg + s;
  // terminate message should be: <cr><lf>
  msg = msg + "\r\n";

  return msg;
}
string build_nmea_GGA(float x, float y, float z){
  string msg = "";
  msg = msg + start + gga; // start byte, type of message
  msg = msg + ",";
  // adding time_stamp
  string timeString = make_time_string();
  msg = msg + timeString;
  msg = msg + ",";
  // lattitude
  msg = msg + make_latt_string(y);
  msg = msg + ",";
  msg = msg + "N";
  msg = msg + ",";
  // longitude
  msg = msg + make_long_string(x);
  msg = msg + ",";
  msg = msg + "E";
  msg = msg + ",";
  // 0 --> no fix 1 --> GPS fix --> 2 DGPS fix (however, paparazzi does not recognize this "fix")
  msg = msg + "1";
  msg = msg + ",";
  // number of "sattelites" in view (current setup has 8 modules --> so 8 ??)
  msg = msg + "08";
  msg = msg + ",";
  // Horizontal Dilution of Precision (HDOP) (1 is best, 6 is okay, 10 is to bad to be used)
  msg = msg + "0.50";
  msg = msg + ",";
  // height in meters above mean see level (in our case only height)
  std::string l(16, '\0');
  auto stuff = std::snprintf(&l[0], l.size(), "%.1f", abs(z)); // negative heigts are not valid
  l.resize(stuff);
  msg = msg + l;
  msg = msg + ",";
  msg = msg + "M";
  msg = msg + ",";
  // Height of geoid above WGS84 ellipsoid --> we also throw in the height here (just to make sure or something ?)
  // from the recordings of the gps, this should be a static value of ca. 46.6,M ... paparazzi disagrees
  msg = msg + l;
  msg = msg + "," ;
  msg = msg + "M" ;
  msg = msg + "," ;
  //Time since last DGPS update --> none
  msg = msg + "," ;
  // DGPS reference station id --> none
  // terminate sign
  msg = msg + end_ ;
  // checksum
  int checksum_ = checksum(msg.c_str());

  std::string s(16, '\0');
  auto written = std::snprintf(&s[0], s.size(), "%02X", checksum_);
  s.resize(written);
  msg = msg + s;
  // terminate message should be: <cr><lf>
  msg = msg + "\r\n";

  return msg;
}

string build_nmea_GSA(){
  // convince the gps module once more that is has a fix (of 8 sattelites which are numbered "random")
  return "$GPGSA,A,3,19,28,14,18,27,22,31,39,,,,,1.7,1.0,1.3*35\r\n";
}

string build_nmea_msg(float x, float y, float z)
{
  tuple<float, float> rotated_xy = rotate_scale_translate_point(y,x); // coordinate-switch intentional
  x = get<0>(rotated_xy);
  y = get<1>(rotated_xy);
  // TODO: height info not used yet, different msg ?!?
  update_time();
  string msg = "";
  msg = msg + build_nmea_RMC(x,y,z);
  msg = msg + build_nmea_GGA(x,y,z);
  msg = msg + build_nmea_GSA();
  return msg;
}
