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

// if write enable function is added uncomment this
#define __READONLY__
#ifndef __READONLY__
#warning "Write Protect is disabled"
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

enum{
  OFF = 0,
  ON = 1,
};

// reads the info of NAND chip
void readIDNAND();

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
  pinMode(IO_0, INPUT);
  pinMode(IO_1, INPUT);
  pinMode(IO_2, INPUT);
  pinMode(IO_3, INPUT);
  pinMode(IO_4, INPUT);
  pinMode(IO_5, INPUT);
  pinMode(IO_6, INPUT);
  pinMode(IO_7, INPUT);
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
}

void loop(){
  int choice = 0;
  Serial.println("Choose Option");
  Serial.println("1: ReadID");
  while(Serial.available() == 0){
    delayMicroseconds(500);
  }
  choice = Serial.read();
  if(choice == '1'){
    readIDNAND();
  }
}

void readIDNAND(){
  //TODO: write this function to be more verbose 
  // "ability to input ID's commands and address commands"

  Serial.println("\n\rReading ID\n\r");
  //while(!digitalRead(RB)){
  //delayMicroseconds(500);
  //}
  Serial.println("reading");
  pinMode(IO_0, OUTPUT);
  pinMode(IO_1, OUTPUT);
  pinMode(IO_2, OUTPUT);
  pinMode(IO_3, OUTPUT);
  pinMode(IO_4, OUTPUT);
  pinMode(IO_5, OUTPUT);
  pinMode(IO_6, OUTPUT);
  pinMode(IO_7, OUTPUT);


  digitalWrite(ALE,OFF);
  digitalWrite(RE,ON);
  digitalWrite(CLE,ON);
  digitalWrite(WE,OFF);
  digitalWrite(CE,OFF);

  //todo: change this to a function
  //0x90 
  digitalWrite(IO_0,OFF);
  digitalWrite(IO_1,OFF);
  digitalWrite(IO_2,OFF);
  digitalWrite(IO_3,OFF);
  digitalWrite(IO_4,ON);
  digitalWrite(IO_5,OFF);
  digitalWrite(IO_6,OFF);
  digitalWrite(IO_7,ON);
  // latch the command value
  digitalWrite(WE,ON);
  digitalWrite(CLE,OFF);

  // setup the address value
  digitalWrite(WE,OFF);
  digitalWrite(ALE,ON);

  //0x00 for main info
  digitalWrite(IO_0,OFF);
  digitalWrite(IO_1,OFF);
  digitalWrite(IO_2,OFF);
  digitalWrite(IO_3,OFF);
  digitalWrite(IO_4,OFF);
  digitalWrite(IO_5,OFF);
  digitalWrite(IO_6,OFF);
  digitalWrite(IO_7,OFF);
  delayMicroseconds(500);

  // latch the address value
  digitalWrite(WE,ON);
  digitalWrite(ALE,OFF);
  pinMode(IO_0, INPUT);
  pinMode(IO_1, INPUT);
  pinMode(IO_2, INPUT);
  pinMode(IO_3, INPUT);
  pinMode(IO_4, INPUT);
  pinMode(IO_5, INPUT);
  pinMode(IO_6, INPUT);
  pinMode(IO_7, INPUT);

  digitalWrite(RE,OFF);
  int bit0 = digitalRead(IO_0);
  int bit1 = digitalRead(IO_1);
  int bit2 = digitalRead(IO_2);
  int bit3 = digitalRead(IO_3);
  int bit4 = digitalRead(IO_4);
  int bit5 = digitalRead(IO_5);
  int bit6 = digitalRead(IO_6);
  int bit7 = digitalRead(IO_7);

  int value = bit7 << 7 | bit6 << 6 | bit5 << 5 | bit4 << 4 | bit3 << 3 | bit2 << 2 | bit1 << 1 | bit0;

  digitalWrite(RE,ON);

  Serial.println(value);

  digitalWrite(RE,OFF);
  bit0 = digitalRead(IO_0);
  bit1 = digitalRead(IO_1);
  bit2 = digitalRead(IO_2);
  bit3 = digitalRead(IO_3);
  bit4 = digitalRead(IO_4);
  bit5 = digitalRead(IO_5);
  bit6 = digitalRead(IO_6);
  bit7 = digitalRead(IO_7);

  value = bit7 << 7 | bit6 << 6 | bit5 << 5 | bit4 << 4 | bit3 << 3 | bit2 << 2 | bit1 << 1 | bit0;

  digitalWrite(RE,ON);

  Serial.println(value);

  digitalWrite(RE,OFF);
  bit0 = digitalRead(IO_0);
  bit1 = digitalRead(IO_1);
  bit2 = digitalRead(IO_2);
  bit3 = digitalRead(IO_3);
  bit4 = digitalRead(IO_4);
  bit5 = digitalRead(IO_5);
  bit6 = digitalRead(IO_6);
  bit7 = digitalRead(IO_7);

  value = bit7 << 7 | bit6 << 6 | bit5 << 5 | bit4 << 4 | bit3 << 3 | bit2 << 2 | bit1 << 1 | bit0;

  digitalWrite(RE,ON);

  Serial.println(value);

  digitalWrite(RE,OFF);
  bit0 = digitalRead(IO_0);
  bit1 = digitalRead(IO_1);
  bit2 = digitalRead(IO_2);
  bit3 = digitalRead(IO_3);
  bit4 = digitalRead(IO_4);
  bit5 = digitalRead(IO_5);
  bit6 = digitalRead(IO_6);
  bit7 = digitalRead(IO_7);

  value = bit7 << 7 | bit6 << 6 | bit5 << 5 | bit4 << 4 | bit3 << 3 | bit2 << 2 | bit1 << 1 | bit0;

  digitalWrite(RE,ON);

  Serial.println(value);

  digitalWrite(RE,OFF);
  bit0 = digitalRead(IO_0);
  bit1 = digitalRead(IO_1);
  bit2 = digitalRead(IO_2);
  bit3 = digitalRead(IO_3);
  bit4 = digitalRead(IO_4);
  bit5 = digitalRead(IO_5);
  bit6 = digitalRead(IO_6);
  bit7 = digitalRead(IO_7);

  value = bit7 << 7 | bit6 << 6 | bit5 << 5 | bit4 << 4 | bit3 << 3 | bit2 << 2 | bit1 << 1 | bit0;

  Serial.println(value);

  digitalWrite(CE,ON);
  digitalWrite(RE,ON);

  Serial.println("\n\rRead ID complete\n\r");
}



