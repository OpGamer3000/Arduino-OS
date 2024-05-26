/*
  Support functions
*/

byte getDat(void) {
  delay(500);
  lcd.clear();
  lcd.home();
  lcd.print(F("DATA:"));
  lcd.setCursor(0, 1);
  lcd.print(F("a"));
  int count = 33;

  lastStateCLK = digitalRead(CLK);
  while (true) {
    input_counter(&count, 33, 125);
    lcd.setCursor(0, 1);

    if(count == 92){
      lcd.write(byte(0));
    } else {
      lcd.print((char)count);
    }

    lcd.print(F(" "));

    if (digitalRead(select) == 0) {
      return count;
    }
  }
}

byte getNum(byte high, byte low){
  int count = 0;
  delay(500);
  lcd.clear();
  lcd.home();
  lcd.print(F("NUM:"));
  lcd.setCursor(0, 1);
  lcd.print(low);
  
  lastStateCLK = digitalRead(CLK);
  while(true){
    input_counter(&count, low, high);
    lcd.setCursor(0, 1);
    lcd.print(count);
    lcd.print(F("   "));
    
    if(digitalRead(select) == 0){
      return count;
    }
  }
}

byte getcmd(void){
  delay(500);
  lcd.clear();
  lcd.home();
  lcd.print(F("CMD:"));
  lcd.setCursor(0, 1);
  lcd.print(commands_inter[0]);

  int count = 0;

  lastStateCLK = digitalRead(CLK);
  while(true){
    input_counter(&count, 0, commandMAX);
    lcd.setCursor(0, 1);
    lcd.print(commands_inter[count]);
    lcd.print(F("    "));

    if(digitalRead(select) == 0){
      return count;
    }
  }
}

byte getRAW(void){
  delay(500);
  lcd.clear();
  lcd.home();
  lcd.print(F("RAW:"));
  lcd.setCursor(0, 1);
  lcd.print(F("0"));

  int count = 0;

  lastStateCLK = digitalRead(CLK);
  while(true){
    input_counter(&count, 0, 255);
    lcd.setCursor(0, 1);
    lcd.print(count);
    lcd.print(F("    "));

    if(digitalRead(select) == 0){
      return count;
    }
  }
}

unsigned short getLBA(void){
  delay(500);
  lcd.clear();
  lcd.home();
  lcd.print(F("LBA: "));
  lcd.setCursor(0, 1);
  lcd.print(F("0"));
  
  int count = 0;

  lastStateCLK = digitalRead(CLK);
  while(true){
    input_counter(&count, 0, lbaMAX);
    lcd.setCursor(0, 1);
    lcd.print(count);
    lcd.print(F("    "));

    if(digitalRead(select) == 0){
      return count;
    }
  }
}

// here
void input_counter(int* previous_count, int min, int max){
  currentStateCLK = digitalRead(CLK);

  if(currentStateCLK != lastStateCLK){
    if(digitalRead(DT) != currentStateCLK){
      if(*previous_count < max) (*previous_count)++;
      else *previous_count = min;
    } else if(digitalRead(DT) == currentStateCLK){
      if(*previous_count > min) (*previous_count)--;
      else *previous_count = max;
    }

    lastStateCLK = currentStateCLK;
  }
}

void updatePAGE(void){
  char* incoming_opt_str;

  lcd.home();
  
  if((opt % 2) == 0){   // down
    incoming_opt_str = page[opt-1];
    lcd.print(incoming_opt_str);
    for(byte i = 0; i < (14 - sizeof(incoming_opt_str)); i++){
      lcd.print(F(" "));
    }

    lcd.setCursor(0, 1);
    incoming_opt_str = page[opt];
    lcd.print(incoming_opt_str);
    for(byte i = 0; i < (14 - sizeof(incoming_opt_str)); i++){
      lcd.print(F(" "));
    }

    lcd.setCursor(15, 1);
    lcd.print(F("<"));
    lcd.setCursor(15, 0);
    lcd.print(F(" "));
  } else {    // up
    incoming_opt_str = page[opt];
    lcd.print(incoming_opt_str);
    for(byte i = 0; i < (14 - sizeof(incoming_opt_str)); i++){
      lcd.print(F(" "));
    }

    lcd.setCursor(0, 1);
    if(opt != optMAX){
      incoming_opt_str = page[opt+1];
      lcd.print(incoming_opt_str);
      for(byte i = 0; i < (14 - sizeof(incoming_opt_str)); i++){
        lcd.print(F(" "));
      }
    } else {
      for(byte i = 0; i < 15; i++){
        lcd.print(F(" "));
      }
    }

    lcd.setCursor(15, 1);
    lcd.print(F(" "));
    lcd.setCursor(15, 0);
    lcd.print(F("<"));
  }
}