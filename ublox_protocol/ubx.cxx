/*
The MIT License (MIT)
Copyright (c) 2016 SparkFun Electronics
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to
do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <cstring>

#include "ubx.h"
using namespace std;

#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()


uint8_t* calculate_checksum()
{
  uint8_t* checksum = new uint8_t[2];
  for(int i = 2; i<6+ubx_msg_length; i++)
  {
    checksum[0] = (uint8_t)(checksum[0] + received_msg[i]);
    checksum[1] = (uint8_t)(checksum[1] + checksum[0]);
  }
  return checksum;
}
uint8_t* calculate_checksum(uint8_t* input)
{
  uint8_t* checksum = new uint8_t[2];
  for(int i = 2; i<(sizeof(input)-2); i++) // does size of work ?
  {
    checksum[0] = checksum[0] + input[i];
    checksum[1] = checksum[1] + checksum[0];
  }
  return checksum;
}

void parsing_statemaschine(uint8_t incoming)
{
  if(!parsing)
  {
    if(incoming == UBX_SYNCH_1) // check for start byte 1
    {
      parsing = true;
      ubxFrameCounter = 0;
    }
    else
    {
      // unknown character, probably missed the previous start of a message
    }
  }
  if(parsing)
  {
    if((ubxFrameCounter == 0) && (incoming != UBX_SYNCH_1)) // check for start byte 1
    {
      parsing = false; // something went wrong, shouldn't be possible
    }
    else if((ubxFrameCounter == 1) && (incoming != UBX_SYNCH_2))
    {
      parsing = false; // something went wrong, shouldn't be possible
    }
    else if(ubxFrameCounter == 2)
    {
      ubx_msg_class = incoming;
      ubx_msg_ChecksumA = 0;
      ubx_msg_ChecksumB = 0;
      packet_valid = false;
    }
    else if(ubxFrameCounter == 3)
    {
      ubx_msg_id = incoming;
    }
    else if(ubxFrameCounter == 4)  //Length LSB
    {
      ubx_msg_length = incoming;
    }
    else if(ubxFrameCounter == 5) // length MSB
    {
      uint8_t lengthLSB = static_cast<uint8_t>(ubx_msg_length);
      ubx_msg_length |= incoming << 8;
      if(ubx_msg_length != 0 && ubx_msg_length < MSG_LENGTH_MAX)
      {
        ubx_msg_payload = new uint8_t[ubx_msg_length];
        cout << "msg_length = " << (ubx_msg_length);
        received_msg = new uint8_t[(6+ubx_msg_length+2)];
        received_msg[0] = UBX_SYNCH_1;
        received_msg[1] = UBX_SYNCH_2;
        received_msg[2] = ubx_msg_class;
        received_msg[3] = ubx_msg_id;
        received_msg[4] = lengthLSB;
        received_msg[5] = incoming;
      }
      else if(ubx_msg_length >= MSG_LENGTH_MAX)
      {
        cout << "over maximum msg length !!!" << endl;
        parsing = false;
      }
      else
      {
        cout << "error: msg_length = 0 " << endl;
        // abort
        // parsing = false;
        // comment the abort statement if messages with length 0 should be parsed
        //ubx_msg_payload = new uint8_t[ubx_msg_length];
        received_msg = new uint8_t[(6+ubx_msg_length+2)];
        received_msg[0] = UBX_SYNCH_1;
        received_msg[1] = UBX_SYNCH_2;
        received_msg[2] = ubx_msg_class;
        received_msg[3] = ubx_msg_id;
        received_msg[4] = lengthLSB;
        received_msg[5] = incoming;
      }
    }
    else if(ubxFrameCounter >= 6)
    {
      if(ubxFrameCounter < (6 + ubx_msg_length))
      {
        ubx_msg_payload[(ubxFrameCounter-6)] = incoming;
        received_msg[ubxFrameCounter] = incoming;
      }
      else if(ubxFrameCounter == (6+ubx_msg_length))
      {
        ubx_msg_ChecksumA = incoming;
        received_msg[ubxFrameCounter] = incoming;
      }
      else if(ubxFrameCounter == (6+ubx_msg_length+1))
      {
        ubx_msg_ChecksumB = incoming;
        received_msg[ubxFrameCounter] = incoming;
        cout << "TEST CONTENT OF ARRAY 2 !!!: " << hex << static_cast<unsigned int>(received_msg[2]) << endl;
        if(ubx_msg_length>=0)
        {
          uint8_t* checksum = calculate_checksum();
          if((checksum[0] == ubx_msg_ChecksumA) && (checksum[1] == ubx_msg_ChecksumB))
          {
            cout << "SET PACKET_VALID TRUE " << endl;
            packet_valid = true;
            parsing = false;
          }
        else
          {
            cout << "could NOT parse package correctly" << endl;
            cout << "checksum:    " << static_cast<unsigned int>(checksum[0]) << " " << static_cast<unsigned int>(checksum[1]) << endl;
            cout << "is checksum: " << static_cast<unsigned int>(ubx_msg_ChecksumA) << " " << static_cast<unsigned int>(ubx_msg_ChecksumB) << endl;
            packet_valid = false;
            parsing = false;
          }

        }
      }
      else if(ubxFrameCounter > (6+ubx_msg_length+1))
      {
        // something went realy wrong ... should not be possible !!
        cout << "counter higher than possible" << endl;
        packet_valid = false;
        parsing = false;
      }
    }
  }

  ubxFrameCounter++;
  return;
}





int main()
{

  int serial_port = open("/dev/ttyUSB0", O_RDWR);
  if (serial_port < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
  }

  struct termios tty;
  if(tcgetattr(serial_port, &tty) != 0) {
    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
  }

  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;

  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;

  tty.c_cflag &= ~CRTSCTS;

  tty.c_cflag |= CREAD | CLOCAL;

  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

  tty.c_cc[VTIME] = 2;
  tty.c_cc[VMIN] = 0;

  cfsetispeed(&tty, B9600);
  cfsetospeed(&tty, B9600);

  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
  }


  uint8_t test = 0xff ;
  int testint = static_cast<unsigned int>(test);
  cout << hex << testint << endl;
  cout <<"test this shit" << endl;

  parsing = false;
  ubx_msg_length = 0;

  //uint8_t fake_message[] = {UBX_SYNCH_1, UBX_SYNCH_2, UBX_CLASS_NAV, UBX_NAV_POSLLH, 28, 0x00,
  //                      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
  //                      0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x00, 0x00 };
  //uint8_t* checksum = calculate_checksum(fake_message);
  //fake_message[34] =  checksum[0];
  //fake_message[35] =  checksum[1];

  char read_buf[256];

  while(true)
  {
    int n = read(serial_port, &read_buf, sizeof(read_buf));
    if(n>0)
    {
      cout << "numer of bytes read: " ;
      cout << n << endl;
      for(int i = 0; i<n; i++)
      {
        cout << (unsigned int)(unsigned char)(read_buf[i]) ;
        cout << " ";
        parsing_statemaschine((unsigned char)(read_buf[i]));

        cout << ubxFrameCounter << endl;

        if(packet_valid)
        {
          cout << "received packet: " ;
          for(int j = 0; j< sizeof(received_msg); j++) // sizeof() not working ??
          {
            cout << hex << static_cast<unsigned int>(received_msg[j]) ;
            cout << " " ;
          }
          cout << endl;
        }
      }
      cout << endl;
    }
  }

  if(packet_valid)
  {
    for(int j = 0; j< 36; j++) // not okay ... why not sizeof() ??
    {
      cout << hex << static_cast<unsigned int>(received_msg[j]) ;
      cout << " " ;
    }
    cout << endl;
  }

  return 0;
}
