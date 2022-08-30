String input;

void setup(){
  Serial.begin(115200);
}

void loop(){
  if(Serial.available()){
    input = Serial.readStringUntil('\n');
    if(input == "PING"){
      Serial.print("OK\n");
    } else {
      Serial.print("ERROR\n");
    }
  }
}
