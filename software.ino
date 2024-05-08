/*
  File fixed? = false
*/

void inter() {                    //make
  byte data = 0;
  byte addr = 0;

  /*v-registers*/
  byte a = 0;
  byte b = 0;
  byte c = 0;
  char d = 0;
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

void prog(byte start) {                                 //=================main programer=====================
  byte command, data;
  byte addr = start;

  while(true){
    command = getcmd();

    if(command == 0){           // mova
      writeDisk(addr, 1);
      addr++;
      writeDisk(addr, getRAW());
      addr++;
      
    } else if(command == 1){    // movb
      writeDisk(addr, 2);
      addr++;
      writeDisk(addr, getRAW());
      addr++;
      
    } else if(command == 2){    // movc
      writeDisk(addr, 3);
      addr++;
      writeDisk(addr, getRAW());
      addr++;
      
    } else if(command == 3){    // movd
      writeDisk(addr, 4);
      addr++;
      writeDisk(addr, getDat());
      addr++;
      
    } else if(command == 4){    // jmp
      writeDisk(addr, 5);
      addr++;
      unsigned short lba = getLBA();
      byte block = 0b000;
      while(true){
        if(lba <= 255){
          break;
        }

        lba -= 256;
        block += 0b001;
      }

      writeDisk(addr, block);
      addr++;
      writeDisk(addr, (byte) lba);
      
    } else if(command == 5){  // je
      writeDisk(addr, 6);
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      unsigned short lba = getLBA();
      byte block = 0b000;
      while(true){
        if(lba <= 255){
          break;
        }

        lba -= 256;
        block += 0b001;
      }

      writeDisk(addr, block);
      addr++;
      writeDisk(addr, (byte) lba);
      addr++;
      
    } else if(command == 6){  // jne
      writeDisk(addr, 7);
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      unsigned short lba = getLBA();
      byte block = 0b000;
      while(true){
        if(lba <= 255){
          break;
        }

        lba -= 256;
        block += 0b001;
      }

      writeDisk(addr, block);
      addr++;
      writeDisk(addr, (byte) lba);
      addr++;
      
    } else if(command == 7){    // add
      writeDisk(addr, 8);
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      
    } else if(command == 8){    // sub
      writeDisk(addr, 9);
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      writeDisk(addr, getNum(4, 1));
      addr++;
      
    } else if(command == 9){   // stc
      writeDisk(addr, 10);
      addr++;
      
    } else if(command == 10){   // clc
      writeDisk(addr, 11);
      addr++;
      
    } else if(command == 11){   // jc
      writeDisk(addr, 12);
      addr++;
      unsigned short lba = getLBA();
      byte block = 0b000;
      while(true){
        if(lba <= 255){
          break;
        }

        lba -= 256;
        block += 0b001;
      }

      writeDisk(addr, block);
      addr++;
      writeDisk(addr, (byte) lba);
      
    } else if(command == 12){   // jnc
      writeDisk(addr, 13);
      addr++;
      unsigned short lba = getLBA();
      byte block = 0b000;
      while(true){
        if(lba <= 255){
          break;
        }

        lba -= 256;
        block += 0b001;
      }

      writeDisk(addr, block);
      addr++;
      writeDisk(addr, (byte) lba);
      
    } else if(command == 13){
      writeDisk(addr, 14);
      addr++;
    } else if(command == 14){
      writeDisk(addr, 255);
      addr++;
    } else if(command == 15){
      writeDisk(addr, endBit);
      return;
    } else{
      lcd.clear();
      lcd.home();
      lcd.print(F("FATAL ERROR:"));
      lcd.setCursor(0, 1);
      lcd.print(commands[command]);

      cli();        // disable interupts
      while(true){} // nothing; loop forever
    }
  }
}