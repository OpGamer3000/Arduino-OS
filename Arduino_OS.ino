/*imports*/
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SoftwareSerial.h>

/*===== user defined stuf ======*/
#define GPU_ADRR 0x27                  // the gpu/lcd communication addr
#define RX 2                           // COM port for ESP-01
#define TX 3                           // ...

/*====== system defined stuf ======*/
// Rotatory encoder input config
#define CLK 8
#define DT 9
#define select 10

// Disk config
#define lbaMAX 2047    // max stable address for the disk


// other things
#define endBit 254
#define devPIN 7

// global objs / variables
LiquidCrystal_I2C lcd(GPU_ADRR, 16, 2);
SoftwareSerial esp(RX, TX);

unsigned int opt = 1;
byte lastStateCLK, currentStateCLK;
bool devmode = false;

const char* const commands_inter[] = {"mova", "movb", "movc", "movd", "jmp", "je", "jne", "add", "sub", "stc", "clc", "jc", "jnc", "int", "hlt", "exit"};
const char* const page[] = {NULL,
  "LED on", "LED off",
  "Program disk", "Read disk",
  "Shutdown", "Run disk",
  "Get temps[NET]", "Write RAW",
  "sus"
};  // 15 chars MAX | add label for your application

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

#define commandMAX ((sizeof(commands_inter)/sizeof(char*)) - 1)
#define optMAX ((sizeof(page)/sizeof(char*)) - 1)

/* ==================== FUNCTION WRAPPER ==================== */

#define commands_MAX sizeof(commands) / sizeof(commands[0])

typedef void (*function_pointer)();

typedef struct {
  function_pointer run;
  char* name;
} function_container;

void cmd_LED_ON(){
  digitalWrite(LED_BUILTIN, HIGH);
}

void cmd_LED_OFF(){
  digitalWrite(LED_BUILTIN, LOW);
}

void cmd_program_disk(){
  programEEPROM();
}

void cmd_read_disk(){
  readEEPROM();
}

void cmd_shutdown(){
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
}

void cmd_run_disk(){
  inter();
}

void cmd_get_temps(){
  lcd.clear();
  lcd.home();
  lcd.print(getNet(F("PING")));
  delay(3500);
  updatePAGE();
}

void cmd_write_raw(){
  writeRAW_EEPROM();
}

// This need not be in order
const function_container commands[] = {
  {cmd_LED_ON, "LED on"},
  {cmd_LED_OFF, "LED off"},
  {cmd_program_disk, "Program disk"},
  {cmd_read_disk, "Read disk"},
  {cmd_shutdown, "Shutdown"},
  {cmd_run_disk, "Run disk"},
  {cmd_get_temps, "Get temps[NET]"},
  {cmd_write_raw, "Write RAW"}
};


/* ==================== MAIN ==================== */

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

// main loop for the kernel
void loop(){
  currentStateCLK = digitalRead(CLK);

  if(lastStateCLK != currentStateCLK){
    if(digitalRead(DT) != currentStateCLK && opt < optMAX){
      opt++;
      updatePAGE();
    } else if(digitalRead(DT) == currentStateCLK && opt > 1){
      opt--;
      updatePAGE();
    }

    lastStateCLK = digitalRead(CLK);
  }

  // cmd executer
  if(!digitalRead(select)){
    bool found = false;
    for(unsigned int i = 0; i < commands_MAX; i++){
      if(!strcmp(commands[i].name, page[opt])){
        commands[i].run();
        found = true;
        break;
      }
    }

    if(!found){
      lcd.clear();
      lcd.home();
      lcd.print(F("CMD not assigned"));
      delay(1500);
      updatePAGE();
    }
  }
}
