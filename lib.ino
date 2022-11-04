/*
  File fixed!
*/

byte getDat() {     //FIXED
  delay(500);
  lcd.clear();
  lcd.home();
  lcd.print("DATA:");
  lcd.setCursor(0, 1);
  lcd.print("a");
  byte count = 0;

  lastStateCLK = digitalRead(CLK);
  while (true) {
    currentStateCLK = digitalRead(CLK);

    if(currentStateCLK != lastStateCLK){
      if(digitalRead(DT) != currentStateCLK && count < keyboardMAX){
        count++;
        lcd.setCursor(0, 1);
        lcd.print(keyboard[count]);
        lcd.print("   ");
      } else if(digitalRead(DT) == currentStateCLK && count > 0){
        count--;
        lcd.setCursor(0, 1);
        lcd.print(keyboard[count]);
        lcd.print("   ");
      }
      lastStateCLK = currentStateCLK;
    }

    if (digitalRead(select) == 0) {
      return count;
    }
  }
}

byte getNum(byte high, byte low){     //FIXED
  byte count = 0;
  delay(500);
  lcd.clear();
  lcd.home();
  lcd.print("NUM:");
  lcd.setCursor(0, 1);
  lcd.print(low);
  
  lastStateCLK = digitalRead(CLK);
  while(true){
    currentStateCLK = digitalRead(CLK);

    if(currentStateCLK != lastStateCLK){
      if(digitalRead(DT) != currentStateCLK && count < high){
        count++;
        lcd.setCursor(0, 1);
        lcd.print(count);
        lcd.print("   ");
      } else if(digitalRead(DT) == currentStateCLK && count > low){
        count--;
        lcd.setCursor(0, 1);
        lcd.print(count);
        lcd.print("   ");
      }

      lastStateCLK = currentStateCLK;
    }
    
    if(digitalRead(select) == 0){
      return count;
    }
  }
}

byte getcmd(){        //FIXED
  delay(500);
  lcd.clear();
  lcd.home();
  lcd.print("CMD:");
  lcd.setCursor(0, 1);
  lcd.print(commands[0]);

  byte count = 0;

  lastStateCLK = digitalRead(CLK);
  while(true){
    currentStateCLK = digitalRead(CLK);

    if(currentStateCLK != lastStateCLK){
      if(digitalRead(DT) != currentStateCLK && count < commandMAX){
        count++;
        lcd.setCursor(0, 1);
        lcd.print(commands[count]);
        lcd.print("   ");
      } else if(digitalRead(DT) == currentStateCLK && count > 0){
        count--;
        lcd.setCursor(0, 1);
        lcd.print(commands[count]);
        lcd.print("   ");
      }

      lastStateCLK = currentStateCLK;
    }

    if(digitalRead(select) == 0){
      return count;
    }
  }
}

byte getRAW(){      //FIXED
  delay(500);
  lcd.clear();
  lcd.home();
  lcd.print("RAW:");
  lcd.setCursor(0, 1);
  lcd.print("0");

  byte count = 0;

  lastStateCLK = digitalRead(CLK);
  while(true){
    currentStateCLK = digitalRead(CLK);

    if(currentStateCLK != lastStateCLK){
      if(digitalRead(DT) != currentStateCLK && count < 255){
        count++;
        lcd.setCursor(0, 5);
        lcd.print(count);
        lcd.print("   ");
      } else if(digitalRead(DT) == currentStateCLK && count > 0){
        count--;
        lcd.setCursor(0, 5);
        lcd.print(count);
        lcd.print("   ");
      }

      lastStateCLK = currentStateCLK;
    }

    if(digitalRead(select) == 0){
      return count;
    }
  }
}

unsigned short getLBA(){
  delay(500);
  lcd.clear();
  lcd.home();
  lcd.print("LBA: ");
  lcd.setCursor(0, 1);
  lcd.print("0");
  
  unsigned short count = 0;

  lastStateCLK = digitalRead(CLK);
  while(true){
    currentStateCLK = digitalRead(CLK);

    if(lastStateCLK != currentStateCLK){
      if(digitalRead(DT) != currentStateCLK && count < lbaMAX){
        count++;
        lcd.setCursor(0, 1);
        lcd.print(count);
        lcd.print("    ");
      } else if(digitalRead(DT) == currentStateCLK && count > 0){
        count--;
        lcd.setCursor(0, 1);
        lcd.print(count);
        lcd.print("    ");
      }

      lastStateCLK = currentStateCLK;
    }

    if(digitalRead(select) == 0){
      return count;
    }
  }
}

void updatePAGE(void){    //FIXED
    lcd.home();
    lcd.print(pageUP[opt]);
    lcd.setCursor(0, 1);
    lcd.print(pageDOWN[opt]);
}