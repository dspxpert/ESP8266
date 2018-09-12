/*
D0-16
D1-5
D2-4
D3-0
D4-2
D5-14
D6-12
D7-13
D8-15
A0-17
*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.print("D0-");
  Serial.println(D0);
  Serial.print("D1-");
  Serial.println(D1);
  Serial.print("D2-");
  Serial.println(D2);
  Serial.print("D3-");
  Serial.println(D3);
  Serial.print("D4-");
  Serial.println(D4);
  Serial.print("D5-");
  Serial.println(D5);
  Serial.print("D6-");
  Serial.println(D6);
  Serial.print("D7-");
  Serial.println(D7);
  Serial.print("D8-");
  Serial.println(D8);
  Serial.print("A0-");
  Serial.println(A0);
}

void loop() {
  // put your main code here, to run repeatedly:
}
