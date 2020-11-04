# include nmea_gps.h

int checksum(const char *s) {
    int c = 0;

    while (*s)
        c ^= *s++;

    return c;
}

void update_time()
{
  time_ms = time_ms + (int) (1000/NMEA_UPDATE);
  time_s = (int) (time_ms/1000)
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

string make_latt_string(int x)
{
  // type conversion from nmea gps format to normal gps format, then addition, then back
  double base_latt_format2 = (int)base_latt/100;
  double z = base_latt - base_latt_format2 ;
  base_latt_format2 = base_latt_format2 + (z/60);

  double latt_format2 = base_latt + magic_number * x ;

  double z = (int)latt_format2/100
  double latt = (latt_format2 -z) * 60 + z

  // some magic magic
  std::stringstream stream;
  stream << std::fixed << std::setprecision(8) << latt;
  std::string s = stream.str();
  return s;
}
string make_long_string(int y)
{
  // type conversion from nmea gps format to normal gps format, then addition, then back
  double base_long_format2 = (int)base_long/100;
  double z = base_long - base_long_format2 ;
  base_long_format2 = base_long_format2 + (z/60);

  double long_format2 = base_long + magic_number * y ;

  double z = (int)long_format2/100
  double long = (long_format2 -z) * 60 + z
  // some magic magic
  std::stringstream stream;
  stream << std::fixed << std::setprecision(8) << long;
  std::string s = stream.str();
  return s;
}

string build_nmea_msg(int x, int y, int z)
{
  update_time();
  string msg = "";
  msg = msg + start + rmc; // start byte, type of message
  msg = msg + ",";
  // adding time_stamp
  string timeString = make_time_string();
  msg = msg + timeString;
  msg = msg + ",";
  // declaring the message as valid by adding A
  msg = msg + "A"
  msg = msg + ",";
  // lattitude
  msg = msg + make_latt_string();
  msg = msg + ",";
  msg = msg + "N";
  msg = msg + ",";
  // longitude
  msg = msg + make_long_string();
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
  msg = msg + end
  // checksum
  string checksum = checksum(msg);
  msg = msg + checksum;

  return msg;
}
