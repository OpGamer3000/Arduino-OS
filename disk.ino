/*
  File fixed? = false
*/

//2047 max address

//writes to disk
void writeDisk(unsigned short lba, byte data) {     //FIXED
  byte block = 0b000;
  const unsigned short lba_init = lba;
  
  while(true){
    if(lba <= 255){
      break;
    }

    lba -= 256;
    block += 0b001;
  }

  if(readDisk(lba_init) != data){
    Wire.beginTransmission((0b1010 << 3) + block);
    Wire.write(lba);
    Wire.write(data);
    Wire.endTransmission();
    delay(5);
  }

  return;
}

//reads from disk
byte readDisk(unsigned short lba) {       //FIXED
  byte data;
  byte block = 0b000;

  while(true){
    if(lba <= 255){
      break;
    }

    lba -= 256;
    block += 0b001;
  }
  
  Wire.beginTransmission((0b1010 << 3) + block);
  Wire.write(lba);
  Wire.endTransmission();
  Wire.requestFrom((0b1010 << 3) + block, 1);
  delay(1);
  if (Wire.available()) {
    data = Wire.read();
  }
  return data;
}

unsigned short fileCheck() {          //FIXED
  unsigned short addr = 0;
  byte data;

  while (true) {
    if (addr >= lbaMAX) {
      return 0;
    }

    data = readDisk(addr);

    if (data == endBit && addr != 0) {
      return addr;
    }

    addr++;
  }
}

void programEEPROM() {      //FIXED
  lcd.clear();
  lcd.home();
  delay(500);

  //function variables
  byte overwrite = 0;
  byte head;

  lcd.print("OVERWRITE code?");
  lcd.setCursor(0, 1);
  lcd.print("NO<   YES");


  lastStateCLK = digitalRead(CLK);
  while (true) {
    currentStateCLK = digitalRead(CLK);
    if(currentStateCLK != lastStateCLK){
      if(digitalRead(DT) != currentStateCLK && overwrite == 0){
        overwrite++;
        lcd.setCursor(0, 1);
        lcd.print("NO    YES<");
      } else if(digitalRead(DT) == currentStateCLK && overwrite == 1) {
        overwrite--;
        lcd.setCursor(0, 1);
        lcd.print("NO<   YES  ");
      }
      lastStateCLK = currentStateCLK;
    }

    if (digitalRead(select) == 0) {
      break;
    }
  }

  delay(10);
  
  if (overwrite == 1) {
    prog(0);
    lcd.clear();
    lcd.home();
    delay(500);
    updatePAGE();
    return;
  } else {
    head = fileCheck();
    if (head > 0) {
      prog(head);
      updatePAGE();
      return;
    } else {
      lcd.clear();
      lcd.home();
      lcd.print("Bad file format!");
      delay(1000);
      updatePAGE();
      return;
    }
  }
}

void readEEPROM() {                     //FIXED
  if(devmode){
    unsigned short i = 0;
    Serial.begin(115200);
    while(i <= lbaMAX){
      Serial.print(i);
      Serial.print(" : ");
      Serial.println(readDisk(i));
      i++;
    }
    Serial.end();
  }
  byte data = readDisk(getLBA());
  lcd.home();
  lcd.clear();
  lcd.print("DATA: ");
  lcd.setCursor(0, 1);
  lcd.print(data);
  delay(1000);
  updatePAGE();
}

void writeRAW_EEPROM(){     //FIXED
  writeDisk(getLBA(), getRAW());
  lcd.clear();
  lcd.home();
  lcd.print("Writen!");
  delay(500);
  updatePAGE();
}
