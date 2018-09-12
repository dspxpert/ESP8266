/*
  WiFiTelnetToSerial - Example Transparent UART to Telnet Server for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WiFi library for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <ESP8266WiFi.h>

int leftA=14; //D5
int leftB=16; //D0
int rightA=13; //D7
int rightB=12; //D6

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 1
const char* ssid = "Songpa";
const char* password = "11110000";

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

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

  pinMode(leftA, OUTPUT);
  pinMode(leftB, OUTPUT);
  pinMode(rightA, OUTPUT);
  pinMode(rightB, OUTPUT);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to "); Serial.println(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) {
    delay(500);
  }
  if (i == 21) {
    Serial.print("Could not connect to"); Serial.println(ssid);
    while (1) {
      delay(500);
    }
  }
  //start UART and the server
  //Serial.begin(115200);
  server.begin();
  server.setNoDelay(true);

  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect");
}

void loop() {
  uint8_t i;
  //check if there are any new clients
  if (server.hasClient()) {
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()) {
        if (serverClients[i]) {
          serverClients[i].stop();
        }
        serverClients[i] = server.available();
        Serial.print("New client: "); Serial.print(i);
        break;
      }
    }
    //no free/disconnected spot so reject
    if (i == MAX_SRV_CLIENTS) {
      WiFiClient serverClient = server.available();
      serverClient.stop();
      Serial.println("Connection rejected ");
    }
  }
  //check clients for data
  for (i = 0; i < MAX_SRV_CLIENTS; i++) {
    if (serverClients[i] && serverClients[i].connected()) {
      if (serverClients[i].available()) {
        //get data from the telnet client and push it to the UART
        while (serverClients[i].available()) {
          byte data = (serverClients[i].read());
          //Serial.write(serverClients[i].read());
          Serial.write(data);

          /*
          switch(data) {
            case '1': forward(100); break();
            case '2': reverse(100); break();
            case '3': forward(0);   break();
            case '4': turnLeft(100); break();
            case '5': turnRight(100); break();
            default:  Serial.println("invalid");
          }
          */

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
  if (Serial.available()) {
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      if (serverClients[i] && serverClients[i].connected()) {
        serverClients[i].write(sbuf, len);
        delay(1);
      }
    }
  }
}
