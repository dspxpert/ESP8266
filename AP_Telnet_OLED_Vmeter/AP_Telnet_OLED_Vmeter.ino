/*
 D7(GPIO13) : UART2 RX(from DUT TX)
 D8(GPIO15) : UART2 TX(to   DUT RX)
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif 

//U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 4, /* data=*/ 5, /* reset=*/ U8X8_PIN_NONE); 
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 5

char print_str[17];
int  updated = 0;
int  loop_toggle = 0;
int  fps = 0;
unsigned long prev_time = 0;

/* Set these to your desired credentials. */
const char *ssid = "ESPtelnet";
const char *password = "password";

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

//   192.168.4.1 

void setup() {

  u8x8.begin();
  // u8x8.setFlipMode(1);
  u8x8.setFont(u8x8_font_pressstart2p_f);    
  u8x8.clear();
  u8x8.setCursor(0,0);
  u8x8.print("ESP Volt Meter");
  
  Serial.begin(115200);
  // Serial.swap();
 
  /* You can remove the password parameter if you want the AP to be open. */
  //WiFi.softAP(ssid, password);
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();

 // start telnet server
  server.begin();
  server.setNoDelay(true);
}

void loop() {
  unsigned long current_time;
  uint8_t i;

  // check updated to send data every 1 sec. 
  current_time = millis();
  if(current_time - prev_time > 1000) {
    updated = 1;
    sprintf(print_str, "%03d", fps);
    u8x8.drawString(12,7,print_str);
    fps = 0;
    prev_time = current_time;
  }

  // A0 voltage divider R changed from 220K -> 1M, A0 input ranged 0~10.8V
  float volt = analogRead(A0)*10.8/1023;
  char logic;

  // Logic decision according to 1.8V CMOS level
  // CMOS : L = 1/3 VDD, H = 2/3 VDD
  // TTL  : L = 0.8V, H = 2V
  if (volt >= 1.2) logic = 'H';
  else if(volt <= 0.6) logic = 'L';
  else logic = '?';

  // enlarged display for Analog Pins 
  sprintf(print_str, "A:%0.3fV", volt);
  u8x8.draw2x2String(0,2,print_str);
  sprintf(print_str, "(%c)", logic);  
  u8x8.draw2x2String(4,5,print_str);
  
  // loop toggle indicator by blinking dot 
  u8x8.setCursor(15,7);
  loop_toggle ^= 1;
  fps ++;
  if(loop_toggle)  u8x8.print(".");  
  else u8x8.print(" "); 
  
  //check if there are any new clients
  if (server.hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }
  
  // send analog voltage to all client & serial port every 1 sec.
  if(updated) {
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        serverClients[i].println(volt);
      }
    }
    Serial.println(volt);
    updated = 0;
  }

  /*
  //check clients for data
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available()){
        //get data from the telnet client and push it to the UART
        while(serverClients[i].available()) Serial.write(serverClients[i].read());
      }
    }
  }
  //check UART for data
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        serverClients[i].write(sbuf, len);
        delay(1);
      }
    }
  }
  */
}
