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
#define CLK 8
#define DT 9
#define select 10

//disk settings
#define lbaMAX 2047    //max stable address for the disk


//others
#define endBit 254
#define devPIN 7

// global objs / variables
LiquidCrystal_I2C lcd(GPU_ADRR, 16, 2);
SoftwareSerial esp(RX, TX);

unsigned int opt = 1;
byte lastStateCLK, currentStateCLK;
bool devmode = false;

const char* const commands[] = {"mova", "movb", "movc", "movd", "jmp", "je", "jne", "add", "sub", "stc", "clc", "jc", "jnc", "int", "hlt", "exit"};
const char* const page[] = {NULL, "LED on", "LED off", "Program disk", "Read disk", "Shutdown", "Run disk", "Get temps[NET]", "write RAW"};   // 15 chars MAX

const byte backslash[] = {
  B00000,
  B10000,
  B01000,
  B00100,
  B00010,
  B00001,
  B00000,
  B00000
};
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
//const char* const keyboard[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};

#define commandMAX ((sizeof(commands)/sizeof(char*)) - 1)
#define optMAX ((sizeof(page)/sizeof(char*)) - 1)


//init
void setup() {
  pinMode(devPIN, INPUT_PULLUP);
  Wire.begin();
  lcd.init();
  lcd.createChar(byte(0), backslash);
  lcd.backlight();
  if(!digitalRead(devPIN)){
    Serial.begin(9600);
    lcd.home();
    lcd.print(F("Enable DEV mode?"));
    lcd.setCursor(0, 1);
    lcd.print(F("YES/NO (Serial)"));
    while(true){
      if(Serial.available()){
        String input = Serial.readStringUntil('\n');
        if(input == "YES"){
          devmode = true;
        } else{
          Serial.end();
        }
        break;
      }
    }
  }
  lcd.clear();
  lcd.home();
  lcd.print(F("Booting..."));

  esp.begin(115200);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(select, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.clear();
  updatePAGE();

  lastStateCLK = digitalRead(CLK);
}

//main loop for the kernel
void loop(){
  currentStateCLK = digitalRead(CLK);

  if(lastStateCLK != currentStateCLK){
    if(digitalRead(DT) != currentStateCLK && opt < optMAX){
      opt++;updatePAGE();
    } else if(digitalRead(DT) == currentStateCLK && opt > 1){
      opt--;updatePAGE();
    }

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
      lcd.print(F("Shutting"));
      lcd.setCursor(0, 1);
      lcd.print(F("down..."));
      lcd.noBacklight();
      lcd.noDisplay();
      Serial.end();
      esp.end();
      cli();          // clear interupts
      while (true);   // halt; loop forever...
      
      case 6:
      inter();
      break;

      case 7:
      lcd.clear();
      lcd.home();
      lcd.print(getNet(F("PING")));
      delay(3500);
      updatePAGE();
      break;

      case 8:
      writeRAW_EEPROM();
    }
  }
}
