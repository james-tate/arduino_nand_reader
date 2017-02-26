// Copyright 2017 James Tate <thejamestate@gmail.com>
//
// Flash Chip Reader
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.




// !!!Enable VERBOSE in File->Preferences for compiler output!!!

// set if the chip is capable of reading 8 or 16 channel here
// if 16 channel uncomment this 
// #define __x16__

// if write enable function is added comment this this
#define __READONLY__
#ifdef __READONLY__
#warning "Write Protect is Enabled"
#endif

#ifndef __x16__
#define __x8__
#warning "Configure for x8"
#endif

// pin setup for NAND chips
// based on http://circuits.datasheetdir.com/400/MT29F2G08AABWP-pinout.jpg
#if defined(__AVR_ATmega2560__)
#warning "Compiling for MEGA2560"
#define RB        7  
#define RE        8  
#define CE        9  
#define CLE      16  
#define ALE      17
#define WE       18
#define WP       19
#define IO_0     29
#define IO_1     30
#define IO_2     31
#define IO_3     32
#define IO_4     41
#define IO_5     42
#define IO_6     43
#define IO_7     44
#if defined(__x16__)
#warning "Configure for x16"
#define IO_8     26
#define IO_9     27
#define IO_10    28
#define IO_11    33
#define IO_12    40
#define IO_13    45
#define IO_14    46
#define IO_15    47
#endif
#else
#error "board not supported"
#endif

#define BIT0      1
#define BIT1      2
#define BIT2      4
#define BIT3      8
#define BIT4     16
#define BIT5     32
#define BIT6     64
#define BIT7    128

// todo: think of way to clarify inverted signals
enum{
  OFF = 0,
  ON = 1,
};

// reads the info of NAND chip
void readIDNAND();
// read data
void readDATANAND();
// reads data bus
int readDataBus(int base);
// sets data bus for output
void setDataBusOut();
// sets data bus for input
void setDataBusIn();
// write address to databus
void putDataBus(int i);
// get chip ready for command
void prepChip();
// reset chip
void reset();
// latch command
void latchCommand();
// set address
void setAddress();
// latch address
void latchAddress();


// set this up
void setup(){
  //setup comm to computer
  Serial.begin(115200);

  Serial.println("Welcome");

  pinMode(ALE, OUTPUT);   // Adress latch enable
  pinMode(CE, OUTPUT);    // Chip enable
  pinMode(CLE,OUTPUT);    // Command latch enable
  pinMode(RE, OUTPUT);    // Read enable
  pinMode(WE, OUTPUT);    // Write enable
  pinMode(WP, OUTPUT);    // Write protect
  pinMode(RB, INPUT);     // Read busy
  // set all IO pins to input for start
  setDataBusIn();
#if defined(__x16__)
  pinMode(IO_8, INPUT);
  pinMode(IO_9, INPUT);
  pinMode(IO_10, INPUT);
  pinMode(IO_11, INPUT);
  pinMode(IO_12, INPUT);
  pinMode(IO_13, INPUT);
  pinMode(IO_14, INPUT);
  pinMode(IO_15, INPUT);
#endif
#if defined(__READONLY__)
  digitalWrite(WP,OFF);
#endif
  // disable chip to start
  digitalWrite(CE,ON); // inverted
  digitalWrite(WE,ON); // inverted
  digitalWrite(RE,ON); // inverted
  digitalWrite(CLE,OFF);
  digitalWrite(ALE,OFF);

  Serial.println("Pin setup complete");
  reset();
}

// do this forever and ever and ever and ever and ever and ever and ever and ever .....
void loop(){
  int choice = 0;
  Serial.println("Choose Option");
  Serial.println("1: ReadID");
  Serial.println("2: Read Contents");
  while(Serial.available() == 0){
    delayMicroseconds(500);
  }
  choice = Serial.read();
  if(choice == '1'){
    readIDNAND();
  }
  if(choice == '2'){
    readDATANAND();
  }
}

void readIDNAND(){

  Serial.println("====================================");
  Serial.println("\n\rReading ID\n\r");

  setDataBusOut();

  prepChip();

  //0x90 
  putDataBus(0x90);
  // latch the command value
  latchCommand();
  // setup the address value
  setAddress();
  //0x00 for main info
  putDataBus(0x00);
  // latch the address value
  latchAddress();
  // switch data bus for intput
  setDataBusIn();

  Serial.print("ID = 0x");
  int id = 0x00;
  // read the data bus
  for(int i = 0; i <= 4; i++){
    if(i == 0){
      id = readDataBus(HEX);
    }
    else{
      readDataBus(HEX);
    }
  }
  Serial.println("\n\r");
  if(id == 0x2c) Serial.print("Found myself attached to Micron");
  else if (id == 0x98) Serial.print("Found myself attached to Toshiba");
  else if (id == 0xec) Serial.print("Found myself attached to Samsung");
  else if (id == 0x04) Serial.print("Found myself attached to Fujitsu");
  else if (id == 0x8f) Serial.print("Found myself attached to National Semiconductors");
  else if (id == 0x07) Serial.print("Found myself attached to Renesas");
  else if (id == 0x20) Serial.print("Found myself attached to ST Micro");
  else if (id == 0xad) Serial.print("Found myself attached to Hynix");
  else if (id == 0x01) Serial.print("Found myself attached to AMD");
  else if (id == 0xc2) Serial.print("Found myself attached to Macronix");
  else
    Serial.println("could not find chip ID");
  Serial.println("\n\rGo to http://www.linux-mtd.infradead.org/nand-data/nanddata.html for more information and find ID for info"); 

  digitalWrite(CE,ON);
  digitalWrite(RE,ON);

  Serial.println("\n\rRead ID complete\n\r");
  Serial.println("====================================");
}

void readDATANAND(){
  Serial.println("====================================");
  Serial.println("\n\rReading Data\n\r");
  //while(!digitalRead(RB)){
  //delayMicroseconds(500);
  //}

  setDataBusOut();

  prepChip();

  putDataBus(0x00);
  latchCommand();

  digitalWrite(ALE,ON);
  digitalWrite(WE,OFF);
  putDataBus(0x00);
  digitalWrite(WE,ON);
  digitalWrite(WE,OFF);
  putDataBus(0x00);
  digitalWrite(WE,ON);
  digitalWrite(WE,OFF);
  putDataBus(0x00);
  digitalWrite(WE,ON);
  digitalWrite(WE,OFF);
  putDataBus(0x00);
  digitalWrite(WE,ON);
  digitalWrite(WE,OFF);
  putDataBus(0x01);
  digitalWrite(WE,ON);
  digitalWrite(ALE,OFF);
  digitalWrite(CLE,ON);
  digitalWrite(WE,OFF);
  putDataBus(0x30);
  digitalWrite(WE,ON);
  digitalWrite(CLE,OFF);
  delayMicroseconds(500);

  setDataBusIn();

  for(int i = 0; i < 44; i++) readDataBus(HEX);
  Serial.println("====================================");

  setDataBusOut();

  digitalWrite(ALE,OFF);
  digitalWrite(RE,ON);
  digitalWrite(CLE,ON);
  digitalWrite(WE,OFF);
  digitalWrite(CE,OFF);
  putDataBus(0x31);
  digitalWrite(WE,ON);
  digitalWrite(CLE,OFF);
  delayMicroseconds(500);

  setDataBusIn();

  for(int i = 0; i < 44; i++) readDataBus(HEX);

  digitalWrite(CE,ON);
  digitalWrite(RE,ON);

  Serial.println("\n\rRead Data complete\n\r");
  Serial.println("====================================");

}

void reset(){
  Serial.println("Resetting");
  digitalWrite(CLE,ON);
  digitalWrite(WE,OFF);
  digitalWrite(CE,OFF);
  putDataBus(0xFF);
  digitalWrite(WE,ON); 
  digitalWrite(CE,ON); 
  digitalWrite(CLE,OFF);
  delayMicroseconds(500);
  Serial.println("Chip Reset");
}

void latchCommand(){
  digitalWrite(WE,ON);
  digitalWrite(CLE,OFF);
}

void setAddress(){
  digitalWrite(WE,OFF);
  digitalWrite(ALE,ON);
}

void latchAddress(){
  digitalWrite(WE,ON);
  digitalWrite(ALE,OFF);
}

void prepChip(){
  digitalWrite(ALE,OFF);
  digitalWrite(RE,ON);
  digitalWrite(CLE,ON);
  digitalWrite(WE,OFF);
  digitalWrite(CE,OFF);
}

int readDataBus(int base){
  // pull RE low so chip dumps contents
  digitalWrite(RE,OFF);
  int bit0 = digitalRead(IO_0);
  int bit1 = digitalRead(IO_1);
  int bit2 = digitalRead(IO_2);
  int bit3 = digitalRead(IO_3);
  int bit4 = digitalRead(IO_4);
  int bit5 = digitalRead(IO_5);
  int bit6 = digitalRead(IO_6);
  int bit7 = digitalRead(IO_7);
  // build the value
  int value = bit7 << 7 | bit6 << 6 | bit5 << 5 | bit4 << 4 | bit3 << 3 | bit2 << 2 | bit1 << 1 | bit0;
  // pull RE high to enable latch of next contents
  digitalWrite(RE,ON);
  Serial.print(value, base);
  return value;
}

void setDataBusOut(){
  pinMode(IO_0, OUTPUT);
  pinMode(IO_1, OUTPUT);
  pinMode(IO_2, OUTPUT);
  pinMode(IO_3, OUTPUT);
  pinMode(IO_4, OUTPUT);
  pinMode(IO_5, OUTPUT);
  pinMode(IO_6, OUTPUT);
  pinMode(IO_7, OUTPUT);
}

void setDataBusIn(){
  pinMode(IO_0, INPUT);
  pinMode(IO_1, INPUT);
  pinMode(IO_2, INPUT);
  pinMode(IO_3, INPUT);
  pinMode(IO_4, INPUT);
  pinMode(IO_5, INPUT);
  pinMode(IO_6, INPUT);
  pinMode(IO_7, INPUT);
}

void putDataBus(int i){
  digitalWrite(IO_0,i & BIT0);
  digitalWrite(IO_1,i & BIT1);
  digitalWrite(IO_2,i & BIT2);
  digitalWrite(IO_3,i & BIT3);
  digitalWrite(IO_4,i & BIT4);
  digitalWrite(IO_5,i & BIT5);
  digitalWrite(IO_6,i & BIT6);
  digitalWrite(IO_7,i & BIT7);
}





