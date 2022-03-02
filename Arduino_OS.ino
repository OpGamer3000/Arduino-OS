//imports
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//=====user defined stuf======
#define GPU_ADRR 0x27     //the gpu/lcd communication addr
#define HARDWARE_ADDR 0b000

//======system defined stuf======
#define left 8
#define right 9
#define select 10
#define DISK (0b1010 << 3) + HARDWARE_ADDR   //disk stuf

//defining objs / variables
LiquidCrystal_I2C lcd(GPU_ADRR, 16, 2);
int opt = 1;

void setup() {
  Wire.begin();
  lcd.init();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Booting...");
  delay(512);
  lcd.backlight();
  pinMode(left, INPUT_PULLUP);
  pinMode(right, INPUT_PULLUP);
  pinMode(select, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  delay(50);
  lcd.clear();
  lcd.home();
  lcd.print("LED on<");
  lcd.setCursor(0, 1);
  lcd.print("LED off");
}

void loop() {
  //pages
  if (digitalRead(left) == 0 && opt == 1) {
    opt++;
    lcd.clear();
    lcd.home();
    lcd.print("LED on");
    lcd.setCursor(0, 1);
    lcd.print("LED off<");
  } else if (digitalRead(right) == 0 && opt == 2) {
    opt--;
    lcd.clear();
    lcd.home();
    lcd.print("LED on<");
    lcd.setCursor(0, 1);
    lcd.print("LED off");
  } else if (digitalRead(left) == 0 && opt == 2) {
    opt++;
    lcd.clear();
    lcd.home();
    lcd.print("Program disk<");
    lcd.setCursor(0, 1);
    lcd.print("Read disk");
  } else if (digitalRead(right) == 0 && opt == 3) {
    opt--;
    lcd.clear();
    lcd.home();
    lcd.print("LED on");
    lcd.setCursor(0, 1);
    lcd.print("LED off<");
  } else if (digitalRead(left) == 0 && opt == 3) {
    opt++;
    lcd.clear();
    lcd.home();
    lcd.print("Program disk");
    lcd.setCursor(0, 1);
    lcd.print("Read disk<");
  } else if (digitalRead(right) == 0 && opt == 4) {
    opt--;
    lcd.clear();
    lcd.home();
    lcd.print("Program disk<");
    lcd.setCursor(0, 1);
    lcd.print("Read disk");
  } else if (digitalRead(left) == 0 && opt == 4) {
    opt++;
    lcd.clear();
    lcd.home();
    lcd.print("Shutdown<");
    lcd.setCursor(0, 1);
    lcd.print("Run disk");
  } else if (digitalRead(right) == 0 && opt == 5) {
    opt--;
    lcd.clear();
    lcd.home();
    lcd.print("Program disk");
    lcd.setCursor(0, 1);
    lcd.print("Read disk<");
  } else if (digitalRead(left) == 0 && opt == 5) {
    opt++;
    lcd.clear();
    lcd.home();
    lcd.print("Shutdown");
    lcd.setCursor(0, 1);
    lcd.print("Run disk<");
  } else if (digitalRead(right) == 0 && opt == 6) {
    opt--;
    lcd.clear();
    lcd.home();
    lcd.print("Shutdown<");
    lcd.setCursor(0, 1);
    lcd.print("Run disk");
  }




  //function
  if (digitalRead(select) == 0) {
    if (opt == 1) {                    //opt 1======
      digitalWrite(LED_BUILTIN, HIGH);
    } else if (opt == 2) {              //opt 2====
      digitalWrite(LED_BUILTIN, LOW);
    } else if (opt == 3) {              //opt 3======
      programEEPROM();
    }  else if (opt == 4) {
      readEEPROM();
    } else if (opt == 5) {        //option 4===================
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Shutting");
      lcd.setCursor(0, 1);
      lcd.print("down...");
      delay(5000);
      lcd.clear();
      lcd.noBacklight();
      while (true) {}
    } else if(opt == 6){
      inter();
    }
  }
  delay(100);
}

void programEEPROM() {
  byte addr, data = NULL;
  byte count = 0;
  lcd.clear();
  lcd.home();
  lcd.print("Starting disk");
  lcd.setCursor(0, 1);
  lcd.print("programing...");
  delay(1000);
  lcd.clear();
  lcd.home();
  lcd.print("addr: ");
  lcd.setCursor(0, 1);
  lcd.print("data:");
  while (true) {
    if (digitalRead(right) == 0 && count >= 0 && count <= 254) {
      count++;
      lcd.setCursor(7, 0);
      lcd.print(count);
      lcd.print("   ");
      delay(100);
    } else if (digitalRead(left) == 0 && count <= 255 && count >= 1) {
      count--;
      lcd.setCursor(7, 0);
      lcd.print(count);
      lcd.print("   ");
      delay(100);
    }
    if (digitalRead(select) == 0) {
      delay(1000);
      addr = count;
      count = 0;
      while (true) {
        if (digitalRead(right) == 0 && count >= 0 && count <= 254) {
          count++;
          lcd.setCursor(7, 1);
          lcd.print(count);
          lcd.print("   ");
          delay(100);
        } else if (digitalRead(left) == 0 && count <= 255 && count >= 1) {
          count--;
          lcd.setCursor(7, 1);
          lcd.print(count);
          lcd.print("   ");
          delay(100);
        }

        if (digitalRead(select) == 0) {
          data = count;
          lcd.clear();
          lcd.home();
          lcd.print("Writing to disk");
          writeDisk(addr, data);
          delay(1000);
          lcd.clear();
          lcd.home();
          lcd.print("Program disk<");
          lcd.setCursor(0, 1);
          lcd.print("Read disk");
          return;
        }
      }
    }
  }
}

void inter() {
  byte addr = 0;
  byte data = 0;

  lcd.clear();
  lcd.home();

  while(true){
    data = readDisk(addr);

    if(data == 1){
      while(true){
        addr++;

        data = readDisk(addr);

        if(data == 255){
          break;
        } else if(data == 0){
          lcd.print("a");
        } else if(data == 1){
          lcd.print("b");
        } else if(data == 2){
          lcd.print("c");
        } else if(data == 3){
          lcd.print("d");
        } else if(data == 4){
          lcd.print("e");
        } else if(data == 5){
          lcd.print("f");
        } else if(data == 6){
          lcd.print("g");
        } else if(data == 7){
          lcd.print("0");
        } else if(data == 8){
          lcd.print("1");
        } else if(data == 9){
          lcd.print("2");
        } else if(data == 10){
          lcd.print("3");
        } else if(data == 11){
          lcd.print("4");
        } else if(data == 12){
          lcd.print("5");
        } else if(data == 13){
          lcd.print("6");
        } else if(data == 14){
          lcd.print("7");
        } else if(data == 15){
          lcd.print("8");
        } else if(data ==16){
          lcd.print("9");
        }
      }
    } else if(data == 255){
      return;
    } else if(data == 111){
      while(true){
        lcd.clear();
        lcd.home();
        lcd.print("SECRET GAME!");
        delay(500);
        lcd.clear();
        lcd.home();
        lcd.print("get ready");
        byte i = 0;
        while(!i == 30){
          if(digitalRead(left) == 0){
            lcd.clear();
            lcd.home();
            lcd.print("left player lost");
            delay(1500);
            lcd.clear();
            lcd.home();
            lcd.setCursor(0, 1);
            lcd.print("EXIT: 0");
            return;
          }

          if(digitalRead(right) == 0){
            lcd.clear();
            lcd.home();
            lcd.print("right player");
            lcd.setCursor(0, 1);
            lcd.print("lost");
            delay(1500);
            lcd.clear();
            lcd.home();
            lcd.setCursor(0, 1);
            lcd.print("EXIT: 0");
            return;
          }
          delay(100);
          i++;
        }
        lcd.clear();
        lcd.home();
        lcd.print("CLICK!");
        while(true){
          if(digitalRead(left) == 0){
            lcd.clear();
            lcd.home();
            lcd.print("left player WON");
            delay(1500);
            lcd.clear();
            lcd.home();
            lcd.setCursor(0, 1);
            lcd.print("EXIT: 0");
            return;
          }

          if(digitalRead(right) == 0){
            lcd.clear();
            lcd.home();
            lcd.print("right player");
            lcd.setCursor(0, 1);
            lcd.print("WON");
            delay(1500);
            lcd.clear();
            lcd.home();
            lcd.setCursor(0, 1);
            lcd.print("EXIT: 0");
            return;
          }
        }
      }
    }

    addr++;
  }
}

void readEEPROM() {
  delay(300);
  int count = 0;
  byte addr;
  lcd.clear();
  lcd.home();
  lcd.print("Addr: ");
  while (true) {
    if (digitalRead(left) == 0 && count <= 255 && count >= 1) {
      count--;
      lcd.setCursor(6, 0);
      lcd.print(count);
      lcd.print("   ");
      delay(100);
    } else if (digitalRead(right) == 0 && count >= 0 && count <= 254) {
      count++;
      lcd.setCursor(6, 0);
      lcd.print(count);
      lcd.print("   ");
      delay(100);
    }

    if (digitalRead(select) == 0) {
      addr = count;
      lcd.clear();
      lcd.home();
      lcd.print("Reading from");
      lcd.setCursor(0, 1);
      lcd.print("disk");
      byte data = readDisk(addr);
      delay(1000);
      lcd.clear();
      lcd.print("DATA:");
      lcd.setCursor(0, 1);
      lcd.print(data);
      delay(3000);
      lcd.clear();
      lcd.home();
      lcd.print("Program disk");
      lcd.setCursor(0, 1);
      lcd.print("Read disk<");
      return;
    }
  }
}

void writeDisk(byte address, byte data) {
  Wire.beginTransmission(DISK);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
  delay(5);
}

byte readDisk(byte address) {
  byte data = NULL;
  Wire.beginTransmission(DISK);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(DISK, 1);
  delay(1);
  if (Wire.available()) {
    data = Wire.read();
  }
  return data;
}
