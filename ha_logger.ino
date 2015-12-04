/* HA LOGGER
 * a node mcu base data logger
 * saves data via restful API
 */ 

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "DHT.h"
#include <TimeLib.h>

const char* ssid     = "wlan name";
const char* password = "pwd";
//no http:// 
const char* host     = "sub.dom.tld";
//base64(user:pwd)
const char* base64   = "BASE64STR";

const char* vapi = "t";
const int device_id = 1;

#define LED D0 // built in led
#define DHTPIN 14    // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

//--------------- time stuff -----------------------------
//its a lot of code only for getting the time, get the time from API is also a way...
// NTP Servers:
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov
const int timeZone = 1; // CET
WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets
//--------------- time stuff -----------------------------

int value = 0;

struct temphum {
  float temp;
  float hum;
};

void setup() {
  Serial.begin(115200);
  delay(10);
 
  // We start by connecting to a WiFi network
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  
  Serial.println("nodemcu DHT22 test!");
  dht.begin();
}

void loop() {
  ++value;
  
  printTime();
  String t = getTime();
  //@todo add retry here if values = 0
  temphum th = readdht();
  Serial.println("t:"+String(th.temp)+"h:"+String(th.hum));
  saveTemp(t, th.temp);
  saveHum(t, th.hum);
  blinkLED();
  delay(60000);
  
}

void saveTemp(String t, float temp)
{
  String url = "/"+ String(vapi) + "/temp/" + String(device_id) + "/"+ t +"/"+ String(temp) +"/";
  Serial.println("url: " + url);
  putApi(url);
}

void saveHum(String t, float hum)
{
  String url = "/"+ String(vapi) + "/humi/" + String(device_id) + "/"+ t +"/"+ String(hum) +"/";
  Serial.println("url: " + url);
  putApi(url);
}

void putApi(String url)
{
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Authorization: Basic " + base64 + "\r\n" +
               "Connection: close\r\n\r\n");                
               
  delay(100);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println("closing connection");
  Serial.println();
}

//trys to read 
struct temphum readdht()
{
  temphum th = {0,0};
  float h,t;
  int cnt = 0;
  
  while(cnt++ < 10)
  {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
  
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)){
      Serial.println("Failed to read from DHT sensor! try again");
      delay(5000);
      continue;  
    }else{
      th.temp = t;
      th.hum = h;
      break;
    }
  }
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println();
  
  return th;
}

//returns time as YYYYMMDD-HHMMSS
String getTime(){
  String out = String(year());
  out += formatInt(month());
  out += formatInt(day());
  out += "-";
  out += formatInt(hour());
  out += formatInt(minute());
  out += formatInt(second());
  return out;
}

void printTime(){
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year()); 
  Serial.println(); 
}

// utility for formating ints, adds leading 0
String formatInt(int digit){
  String out = String(digit);
  if(digit < 10)
    out = "0"+out;
  
  return out;
}

void blinkLED(){
  digitalWrite(LED, LOW);
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
