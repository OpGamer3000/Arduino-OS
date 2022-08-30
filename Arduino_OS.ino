/*imports*/
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SoftwareSerial.h>

/*=====user defined stuf======*/
#define GPU_ADRR 0x27                  //the gpu/lcd communication addr
#define RX 2                           //incase u wanted to change the coms port
#define TX 3                           //...

/*======system defined stuf======*/
//input, u can edit this if u want to configure the input pins
#define left 8
#define right 9
#define select 10
#define MAXADDR 255

//disk stuf, edit only if you know what you are doing. . .
#define HARDWARE_ADDR 0b000
#define DISK (0b1010 << 3) + HARDWARE_ADDR

//others
#define endBit 254

/*defining global objs / variables*/
LiquidCrystal_I2C lcd(GPU_ADRR, 16, 2);
SoftwareSerial esp(RX, TX);
int opt = 1;
const String commands[] = {"mova", "movb", "movc", "movd", "jmp", "je", "jne", "add", "sub", "jc", "jnc", "int", "hlt", "exit"};
/*mova = 1
  movb = 2
  movc = 3
  movd = 4
  int = 5
  jmp = 6
  je = 7
  jne = 8
  add = 9
  sub = 10
  stc = 11
  clc = 12
  jc = 13
  jnc = 14
  fend = 254
  hlt = 255*/
const String keyboard[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
//unsigned const int keyboard_nums[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

unsigned const int commandMAX = (sizeof(commands)/sizeof(String)) - 1;
unsigned const int keyboardMAX = (sizeof(keyboard)/sizeof(String)) - 1;
//unsigned const int keyboard_nums_MAX = (sizeof(keyboard_nums)/sizeof(int)) - 1;

void setup() {
  Wire.begin();
  lcd.init();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Booting...");
  esp.begin(115200);
  
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
  } else if(digitalRead(left) == 0 && opt == 6){
    opt++;
    lcd.clear();
    lcd.home();
    lcd.print("Get temps[NET]<");
  } else if(digitalRead(right) == 0 && opt == 7){
    opt--;
    lcd.clear();
    lcd.home();
    lcd.print("Shutdown");
    lcd.setCursor(0, 1);
    lcd.print("Run disk<");
  }




  //function
  if (digitalRead(select) == 0) {
    if (opt == 1) {                     //opt 1 || LED ON
      digitalWrite(LED_BUILTIN, HIGH);
    } else if (opt == 2) {              //opt 2 || LED OFF
      digitalWrite(LED_BUILTIN, LOW);
    } else if (opt == 3) {              //opt 3 || program DISK
      programEEPROM();
    }  else if (opt == 4) {             //opt 4 || read DISK
      readEEPROM();
    } else if (opt == 5) {              //opt 5 || SHUTDOWN
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Shutting");
      lcd.setCursor(0, 1);
      lcd.print("down...");
      delay(5000);
      lcd.clear();
      lcd.noBacklight();
      while (true) {}
    } else if (opt == 6) {              //opt 6 || run DISK
      inter();
    } else if(opt == 7){                //opt 7 || NET stuf
      lcd.clear();
      lcd.home();
      lcd.print(getNet("PING\n"));
      delay(3500);
      lcd.clear();
      lcd.home();
      lcd.print("Get temps[NET]");
    }
  }
  delay(100);
}

void programEEPROM() {
  lcd.clear();
  lcd.home();
  delay(1000);

  //function variables
  byte overwrite = 0;
  byte head = NULL;

  lcd.print("OVERWRITE code?");
  lcd.setCursor(0, 1);
  lcd.print("NO<   YES");


  while (true) {
    if (digitalRead(right) == 0 && overwrite == 0) {
      overwrite++;
      lcd.setCursor(0, 1);
      lcd.print("NO    YES<  ");
    } else if (digitalRead(left) == 0 && overwrite == 1) {
      overwrite--;
      lcd.setCursor(0, 1);
      lcd.print("NO<   YES  ");
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
    lcd.print("Program disk<");
    lcd.setCursor(0, 1);
    lcd.print("Read disk");
    return;
  } else {
    head = fileCheck();
    if (head > 0) {
      prog(head);
      lcd.clear();
      lcd.home();
      lcd.print("Program disk<");
      lcd.setCursor(0, 1);
      lcd.print("Read disk");
      return;
    } else {
      lcd.clear();
      lcd.home();
      lcd.print("Bad file format!");
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

void inter() {                    //make
  byte data = 0;
  byte addr = 0;

  /*v-registers*/
  byte a = 0;
  byte b = 0;
  byte c = 0;
  char d = NULL;
  byte cf = 0;
  byte pointer = 0;

  /*main*/
  while(true){
    data = readDisk(addr);

    switch(data){
      case 1: //mova
      addr++;
      data = readDisk(addr);
      a = data;
      break;


      case 2: //movb
      addr++;
      data = readDisk(addr);
      b = data;
      break;


      case 3: //movc
      addr++;
      data = readDisk(addr);
      c = data;
      break;


      case 4: //movd
      addr++;
      data = readDisk(addr);
      d = data;
      break;


      case 5: //int
      if(a == 0){
        if(pointer > 15 && pointer <= 31){
          lcd.setCursor((pointer - 16), 1);
        } else if(pointer <= 15){
          lcd.setCursor(pointer, 0);
        }

        lcd.print(d);
        pointer++;
      } else if(a == 1){
        lcd.clear();
      } else if(a == 2){
        if(pointer != 0){
          byte backspace = pointer - 1;

          if(backspace > 15 && backspace <= 31){
            lcd.setCursor((backspace - 16), 1);
            lcd.print(' ');
          } else if(pointer <= 15){
            lcd.setCursor(backspace, 0);
            lcd.print(' ');
          }

          pointer--;
        }
      } else if(a == 3){
        pointer = b;
      }

      break;

      case 6: //jmp
      addr++;
      addr = readDisk(addr);

      case 7: //je
      addr++;
      if(readDisk(addr) == readDisk(addr + 1)){
        addr = readDisk(addr + 2);
      } else {
        cf = 1;
        addr += 3;
        break;
      }
    }
  }
}

void readEEPROM() {                     //correction needed
  lcd.clear();
  lcd.home();
  delay(300);
  unsigned int count = 0;
  unsigned int addr;
  lcd.print("Addr: ");
  while (true) {
    if (digitalRead(left) == 0 && count <= MAXADDR && count >= 1) {
      count--;
      lcd.setCursor(6, 0);
      lcd.print(count);
      lcd.print("   ");
      delay(100);
    } else if (digitalRead(right) == 0 && count >= 0 && count <= (MAXADDR - 1)) {
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

byte fileCheck() {
  unsigned int addr = 0;
  byte data = NULL;

  while (true) {
    if (addr >= 255) {
      return 0;
    }

    data = readDisk(addr);

    if (data == endBit && addr != 0) {
      return addr;
    }

    addr++;
  }
}

void writeDisk(byte address, byte data) {
  if(readDisk(address) != data){
    Wire.beginTransmission(DISK);
    Wire.write(address);
    Wire.write(data);
    Wire.endTransmission();
    delay(5);
  }

  return;
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

void prog(byte start) {                                 //=================main programer=====================
  String command;
  byte data = NULL;
  byte addr = start;

  while(true){
    command = getcmd();

    if(command == "mova"){
      writeDisk(addr, 1);
      addr++;
      writeDisk(addr, getRAW());
      addr++;
      break;
    } else if(command = "movb"){
      writeDisk(addr, 2);
      addr++;
      writeDisk(addr, getRAW());
      addr++;
      break;
    } else if(command == "movc"){
      writeDisk(addr, 3);
      addr++;
      writeDisk(addr, getRAW());
      addr++;
      break;
    } else if(command == "movd"){
      writeDisk(addr, 4);
      addr++;
      writeDisk(addr, getDat());
      addr++;
      break;
    } else if(command == "int"){
      writeDisk(addr, 5);
      addr++;
      break;
    } else if(command == "jmp"){
      writeDisk(addr, 6);
      addr++;
      writeDisk(addr, getRAW());
      addr++;
      break;
    } else if(command == "je"){
      writeDisk(addr, 7);
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      writeDisk(addr, getRAW());
      addr++;
      break;
    } else if(command == "jne"){
      writeDisk(addr, 7);
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      writeDisk(addr, getRAW());
      addr++;
      break;
    } else if(command == "add"){
      writeDisk(addr, 9);
      addr++;
      break;
    } else if(command == "sub"){
      writeDisk(addr, 10);
      addr++;
      break;
    } else if(command == "stc"){
      writeDisk(addr, 11);
      addr++;
      break;
    } else if(command == "clc"){
      writeDisk(addr, 12);
      addr++;
      break;
    } else if(command == "jc"){
      writeDisk(addr, 13);
      addr++;
      writeDisk(addr, getRAW());
      break;
    } else if(command == "jnc"){
      writeDisk(addr, 14);
      addr++;
      writeDisk(addr, getRAW());
      break;
    } else if(command == "hlt"){
      writeDisk(addr, 255);
      addr++;
    } else if(command == "exit"){
      writeDisk(addr, endBit);
      return;
    } else{
      lcd.clear();
      lcd.home();
      lcd.print("FATAL ERROR");
      lcd.setCursor(0, 1);
      lcd.print("STOPING...");
      lcd.noBacklight();

      while(true){} //nothing
    }
  }
}
                                                         //========================end, start of extra stuf=========================
byte getDat() {
  delay(1000);
  lcd.clear();
  lcd.home();
  lcd.print("DATA:");
  byte count = 0;

  while (true) {

    if (digitalRead(right) == 0 && count <= (keyboardMAX - 1) && count >= 0) {
      count++;
      lcd.setCursor(0, 1);
      lcd.print(keyboard[count]);
      lcd.print("   ");
      delay(100);
    } else if (digitalRead(left) == 0 && count <= keyboardMAX && count >= 1) {
      count--;
      lcd.setCursor(0, 1);
      lcd.print(keyboard[count]);
      lcd.print("   ");
      delay(100);
    }

    if (digitalRead(select) == 0) {
      return count;
    }
  }
}

byte getNum(byte high, byte low){
  byte count = 0;
  delay(1000);
  lcd.clear();
  lcd.home();
  lcd.print("NUM:");
  
  while(true){
  if(digitalRead(right) == 0 && count <= (high - 1) && count >= low){
    count++;
    lcd.setCursor(0, 1);
    lcd.print(count);
    lcd.print("   ");
    delay(100);
  }else if(digitalRead(left) == 0 && count <= high && count >= (low + 1)){
    count--;
    lcd.setCursor(0, 1);
    lcd.print(count);
    lcd.print("   ");
    delay(100);
  }

  if(digitalRead(select) == 0){
    return count;
  }
}}

String getcmd(){
  delay(1000);
  lcd.clear();
  lcd.home();
  lcd.print("CMD:");

  byte count = 0;

  while(true){
    if(digitalRead(right) == 0 && count <= (commandMAX - 1) && count >= 0){
      count++;
      lcd.setCursor(0, 1);
      lcd.print(commands[count]);
      lcd.print("   ");
      delay(100);
    } else if(digitalRead(left) == 0 && count <= commandMAX && count >= 1){
      count--;
      lcd.setCursor(0, 1);
      lcd.print(commands[count]);
      lcd.print("   ");
      delay(100);
    }

    if(digitalRead(select) == 0){
      return commands[count];
    }
  }
}

byte getRAW(){
  delay(1000);
  lcd.clear();
  lcd.home();
  lcd.print("RAW:");

  byte count = 0;

  while(true){
    if(digitalRead(right) == 0 && count <= (255-1) && count >= 0){
      count++;
      lcd.setCursor(0, 5);
      lcd.print(count);
      lcd.print("   ");
      delay(100);
    } else if(digitalRead(left) == 0 && count <= 255 && count >= 1){
      count--;
      lcd.setCursor(0, 5);
      lcd.print(count);
      lcd.print("   ");
      delay(100);
    }

    if(digitalRead(select) == 0){
      return count;
    }
  }
}

void game(){
  while (true) {
        lcd.clear();
        lcd.home();
        lcd.print("SECRET GAME!");
        delay(500);
        lcd.clear();
        lcd.home();
        lcd.print("get ready");
        byte i = 0;
        while (!i == 30) {
          if (digitalRead(left) == 0) {
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

          if (digitalRead(right) == 0) {
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
        while (true) {
          if (digitalRead(left) == 0) {
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

          if (digitalRead(right) == 0) {
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

String getNet(String data){
  esp.print(data);
  String input = esp.readStringUntil('\n');
  
  return input;
}

//yes
