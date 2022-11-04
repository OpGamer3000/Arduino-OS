/*
  File fixed!
*/

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
#define DT 8
#define CLK 9
#define select 10

//disk stuf. . .
#define lbaMAX 2047    //max stable address for the disk


//others
#define endBit 254
#define devPIN 7

/*defining global objs / variables*/
LiquidCrystal_I2C lcd(GPU_ADRR, 16, 2);
SoftwareSerial esp(RX, TX);
unsigned int opt = 1;
byte lastStateCLK, currentStateCLK;
bool devmode = false;

const char *pageUP[] = {"", "LED on<         ", "LED on          ", "Program disk<   ", "Program disk    ", "Shutdown<       ", "Shutdown        ", "Get temps[NET]< ", "Get temps[NET]  "};
const char *pageDOWN[] = {"", "LED off         ", "LED off<        ", "Read disk       ", "Read disk<      ", "Run disk        ", "Run disk<       ", "Write RAW       ", "Write RAW<     "};
const char *commands[] = {"mova", "movb", "movc", "movd", "jmp", "je", "jne", "add", "sub", "jc", "jnc", "int", "hlt", "exit"};
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
const char *keyboard[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

unsigned const int commandMAX = (sizeof(commands)/sizeof(char*)) - 1;
unsigned const int keyboardMAX = (sizeof(keyboard)/sizeof(char*)) - 1;
unsigned const int optMAX = (sizeof(pageUP)/sizeof(char*)) - 1;

//init
void setup() {
  pinMode(devPIN, INPUT_PULLUP);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.home();
  if(!digitalRead(devPIN)){
    lcd.print("Enable DEV mode?");
    lcd.setCursor(0, 1);
    lcd.print("YES/NO (Serial)");
    while(true){
      if(Serial.available()){
        char* input = Serial.readStringUntil('\n');
        if(input == "YES"){
          devmode = true;
        }
      }
    }
  }
  lcd.print("Booting...");

  esp.begin(115200);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(select, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.clear();
  lcd.home();
  lcd.print("LED on<");
  lcd.setCursor(0, 1);
  lcd.print("LED off");

  lastStateCLK = digitalRead(CLK);
}

/*
TODO: remake GUI and GUI logic
*/

//main loop for the kernel
void loop(){
  currentStateCLK = digitalRead(CLK);

  if(lastStateCLK != currentStateCLK){
    if(digitalRead(DT) != currentStateCLK && opt < optMAX){
      opt++;
    } else if(digitalRead(DT) == currentStateCLK && opt > 1){
      opt--;
    }

    updatePAGE();

    lastStateCLK = digitalRead(CLK);
  }

  if(digitalRead(select) == 0){
    switch(opt){
      case 1:
      digitalWrite(LED_BUILTIN, HIGH);
      break;

      case 2:
      digitalWrite(LED_BUILTIN, LOW);
      break;

      case 3:
      programEEPROM();
      break;

      case 4:
      readEEPROM();
      break;

      case 5:
      lcd.clear();
      lcd.home();
      lcd.print("Shutting");
      lcd.setCursor(0, 1);
      lcd.print("down...");
      lcd.noBacklight();
      lcd.noDisplay();
      lcd.off();
      while (true) {}
      
      case 6:
      inter();
      break;

      case 7:
      lcd.clear();
      lcd.home();
      lcd.print(getNet("PING\n"));
      delay(3500);
      updatePAGE();
      break;

      case 8:
      writeRAW_EEPROM();
    }
  }
}
