#include <ESP8266WiFi.h>
#include <WiFiClient.h> 

int leftA=D5;  //14; //D5
int leftB=D0;  //16; //D0
int rightA=D7; //13; //D7
int rightB=D6; //12; //D6


//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 5

/* Set these to your desired credentials. */
const char *ssid = "ESPtelnetRC";
const char *password = "password";

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

//   192.168.4.1 

void forward(int speed) {
  // put your main code here, to run repeatedly:
  int writeVal = map(speed, 0, 100, 0, 1023);
  digitalWrite(leftB, LOW);
  analogWrite(leftA, writeVal);
  digitalWrite(rightB, LOW);
  analogWrite(rightA, writeVal);
}

void reverse(int speed) {
  // put your main code here, to run repeatedly:
  int writeVal = map(speed, 0, 100, 0, 1023);
  digitalWrite(leftA, LOW);
  analogWrite(leftB, writeVal);
  digitalWrite(rightA, LOW);
  analogWrite(rightB, writeVal);
}

void turnLeft(int speed) {
  // put your main code here, to run repeatedly:
  int writeVal = map(speed, 0, 100, 0, 1023);
  digitalWrite(leftA, LOW);
  analogWrite(leftB, LOW);
  digitalWrite(rightB, LOW);
  analogWrite(rightA, writeVal);
}

void turnRight(int speed) {
  // put your main code here, to run repeatedly:
  int writeVal = map(speed, 0, 100, 0, 1023);
  digitalWrite(leftB, LOW);
  analogWrite(leftA, writeVal);
  digitalWrite(rightB, LOW);
  analogWrite(rightA, LOW);
}

void setup() {
	delay(1000);
  
  pinMode(leftA, OUTPUT);
  pinMode(leftB, OUTPUT);
  pinMode(rightA, OUTPUT);
  pinMode(rightB, OUTPUT);
	
	Serial.begin(115200);

	/* You can remove the password parameter if you want the AP to be open. */
	//WiFi.softAP(ssid, password);
  WiFi.softAP(ssid);

	IPAddress myIP = WiFi.softAPIP();
 
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
        while(serverClients[i].available()) {
          byte data = (serverClients[i].read());
          //Serial.write(serverClients[i].read());
          Serial.write(data);

          /*
          if(data == '+') forward(100);
          if(data == '-') reverse(100);
          if(data == 'B') forward(0);
          if(data == 'L') turnLeft(100);
          if(data == 'R') turnRight(100);     
          */
            
          if(data == '8') forward(100);
          if(data == '2') reverse(100);
          if(data == '5') forward(0);
          if(data == '4') turnLeft(100);
          if(data == '6') turnRight(100);
        }
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
