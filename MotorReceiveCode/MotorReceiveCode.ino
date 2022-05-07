#include <Servo.h>
#include <SPI.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <string.h>


const int m1s = 0;
const int m2s = 4;
const int m3s = 8;
const int m4s = 12;

const int m1d = 1;
const int m2d = 5;
const int m3d = 9;
const int m4d = 24;
  
#define UDP_TX_PACKET_MAX_SIZE 860 //increase UDP size
#define PING_TIME 3000 //watchdog check time

//Error checking booleans
bool ethernetError = false;
bool parseError = false;

//Watchdog variables
unsigned long prevMillis = millis();
unsigned long lastPacketMs;
bool hasWatchdogPassed = false;

//Packet/ethernet
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 100);
unsigned int localPort = 8080;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
EthernetUDP Udp;

Servo motors[8];

void setup() 
{
  // PINS - Change as needed...
  pinMode(m1s, OUTPUT);
  pinMode(m2s, OUTPUT);
  pinMode(m3s, OUTPUT);
  pinMode(m4s, OUTPUT);

  pinMode(m1d, OUTPUT);
  pinMode(m2d, OUTPUT);
  pinMode(m3d, OUTPUT);
  pinMode(m4d, OUTPUT);

  Ethernet.init(33);
  Ethernet.begin(mac, ip);

  //Checking for hardware
  if (Ethernet.hardwareStatus() == EthernetNoHardware) 
  {
    ethernetError = true;
  }
  if (Ethernet.linkStatus() == LinkOFF) 
  {
    ethernetError = true;
  }

  Udp.begin(localPort);
}

void loop() 
{
  if (!ethernetError) 
  {
    //Watchdog test
    if ((unsigned long)(millis() - prevMillis) >= PING_TIME) 
    {
      prevMillis = millis();
      if (!hasWatchdogPassed) 
      {
        kill();
      }
      hasWatchdogPassed = false; //resetting
    }

    int packetSize = Udp.parsePacket();
    if (packetSize) 
    {
      // update time when packet was received
      lastPacketMs = millis();

      // read the packet into packetBufffer
      Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);

      //handle data
      parseError = false;
      String str = convertToString(packetBuffer);
      parse(str);
    }
  } 
  else 
  {
    kill();
  }

}

void parse(String cmd) 
{
  char *cmds[22]; // an array of pointers to the pieces of the above array after strtok()
  char *ptr = NULL;
  byte i = 0;

  char c[cmd.length() + 1];
  cmd.toCharArray(c, cmd.length() + 1);

  ptr = strtok(c, "_");  // delimiter
  while (ptr != NULL)
  {
    cmds[i] = ptr;
    i++;
    ptr = strtok(NULL, "_");
  }

  if (cmd[0] == '?') 
  {
    hasWatchdogPassed = true;
  }
  else if (cmd[0] == 'M') 
  {
    //m1
    if (atoi(cmds[1]) == 1) 
    {
      if(strcmp(cmds[sizeof(cmd) - 2], "+")) //issue because cmds is a pointer and '+' is integer under ASCII so string compare
      {
        digitalWrite(m1d, HIGH);
      }
      else 
      {
        digitalWrite(m1d, LOW);
      }
      spin(m1s, atof(cmds[sizeof(cmd) - 1]));
    }
    //------------------------------------------------------

    //m2
    if (atoi(cmds[1]) == 2) {
      if(strcmp(cmds[sizeof(cmd) - 2], "+"))
      {
        digitalWrite(m2d, HIGH);
      }
      else {
        digitalWrite(m2d, LOW);
      }
      spin(m2s, atof(cmds[sizeof(cmd) - 1]));
    }
    //------------------------------------------------------

    //m3
    if (atoi(cmds[1]) == 3) 
    {
      if(strcmp(cmds[sizeof(cmd) - 2], "+")) 
      {
        digitalWrite(m3d, HIGH);
      }
      else 
      {
        digitalWrite(m3d, LOW);
      }
      spin(m3s, atof(cmds[sizeof(cmd) - 1]));
    }
    //------------------------------------------------------

    //m4
    if (atoi(cmds[1]) == 4) 
    {
      if(strcmp(cmds[sizeof(cmd) - 2], "+"))
      {
        digitalWrite(m4d, HIGH);
      }
      else {
        digitalWrite(m4d, LOW);
      }
      spin(m4s, atof(cmds[sizeof(cmd) - 1]));
    }
  }
}

String convertToString(char* packets)
{
  int i;
  String str = "";
  for (i = 0; i < (int)strlen(packets); i++) 
  {
    str = str + packets[i];
  }
  return str;
}

void kill() 
{
  digitalWrite(m1s, LOW);
  digitalWrite(m2s, LOW);
  digitalWrite(m3s, LOW);
  digitalWrite(m4s, LOW);
}

void spin(int pin, float speed) 
{
  digitalWrite(pin, HIGH);
  delay(map(speed, 0, 100, 1, 100));
  digitalWrite(pin, LOW);
  delay(map(speed, 0, 100, 100, 500));

}
