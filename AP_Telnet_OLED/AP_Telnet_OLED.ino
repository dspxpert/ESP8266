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

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 4, /* data=*/ 5, /* reset=*/ U8X8_PIN_NONE); 
// U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 5

/* Set these to your desired credentials. */
const char *ssid = "ESPtelnet";
const char *password = "password";

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

//   192.168.4.1 


void setup() {
  delay(1000);
  Serial.begin(115200);
  
  Serial.swap();
  /* You can remove the password parameter if you want the AP to be open. */
  
  //WiFi.softAP(ssid, password);
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();

  u8x8.begin();
  // u8x8.setFlipMode(1);
  u8x8.setFont(u8x8_font_pressstart2p_f);    
  u8x8.clear();
  u8x8.setCursor(0,0);
  u8x8.print("Hello");
  
 // start telnet server
  server.begin();
  server.setNoDelay(true);
}

void loop() {
  uint8_t i;
  /*
  if(server.hasClient()){
    digitalWrite(led, LOW);
  } else{
    digitalWrite(led, HIGH);
  }
  */
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
}
