/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */

// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
// give it a name:
int a0 = 0;
int a1 = 1;
int a2 = 2;
int a3 = 3;
int a4 = 4;
int a5 = 5;
int a6 = 6;
int a7 = 7;
int a8 = 8;
int a9 = 9;
int a10 = 10;
int a11 = 11;
int a12 = 12;
int a13 = 13;
int a14 = 14;
int a15 = 15;
int a16 = 16;
int a17 = 17;
int a18 = 18;
int a19 = 19;
int a20 = 20;
int d0 = 21;
int d1 = 22;
int d2 = 23;
int d3 = 24;
int d4 = 25;
int d5 = 26;
int d6 = 27;
int d7 = 28;
int d8 = 29;
int d9 = 30;
int d10 = 31;
int d11 = 32;
int d12 = 33;
int d13 = 34;
int d14 = 35;
int d15 = 36;

int ce = 37;
int csb = 38;

int led = 13;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);

  // initalize address bus
  pinMode(a0, OUTPUT);
  pinMode(a1, OUTPUT);
  pinMode(a2, OUTPUT);
  pinMode(a3, OUTPUT);
  pinMode(a4, OUTPUT);
  pinMode(a5, OUTPUT);
  pinMode(a6, OUTPUT);
  pinMode(a7, OUTPUT);
  pinMode(a8, OUTPUT);
  pinMode(a9, OUTPUT);
  pinMode(a10, OUTPUT);
  pinMode(a11, OUTPUT);
  pinMode(a12, OUTPUT);
  pinMode(a13, OUTPUT);
  pinMode(a14, OUTPUT);
  pinMode(a15, OUTPUT);
  pinMode(a16, OUTPUT);
  pinMode(a17, OUTPUT);
  pinMode(a18, OUTPUT);
  pinMode(a19, OUTPUT);
  pinMode(a20, OUTPUT);

  // initialize data bus
  int input = INPUT_PULLDOWN;
  pinMode(d0, input);
  pinMode(d1, input);
  pinMode(d2, input);
  pinMode(d3, input);
  pinMode(d4, input);
  pinMode(d5, input);
  pinMode(d6, input);
  pinMode(d7, input);
  pinMode(d8, input);
  pinMode(d9, input);
  pinMode(d10, input);
  pinMode(d11, input);
  pinMode(d12, input);
  pinMode(d13, input);
  pinMode(d14, input);
  pinMode(d15, input);


  // initialize control bits
  pinMode(ce, OUTPUT);
  pinMode(csb, OUTPUT);

  digitalWrite(a0, LOW);
  digitalWrite(a1, LOW);
  digitalWrite(a2, LOW);
  digitalWrite(a3, LOW);
  digitalWrite(a4, LOW);
  digitalWrite(a5, LOW);
  digitalWrite(a6, LOW);
  digitalWrite(a7, LOW);
  digitalWrite(a8, LOW);
  digitalWrite(a9, LOW);
  digitalWrite(a10, LOW);
  digitalWrite(a11, LOW);
  digitalWrite(a12, LOW);
  digitalWrite(a13, LOW);
  digitalWrite(a14, LOW);
  digitalWrite(a15, LOW);
  digitalWrite(a16, LOW);
  digitalWrite(a17, LOW);
  digitalWrite(a18, LOW);
  digitalWrite(a19, LOW);
  digitalWrite(a20, LOW);

  digitalWrite(ce, HIGH);
  digitalWrite(csb, LOW);

  Serial.begin(9600);
  delay(2);
  Serial.println("Started Serial COM");
}

void setAddress(unsigned int addr) {
  digitalWrite(a0, bitRead(addr, 0));
  digitalWrite(a1, bitRead(addr, 1));
  digitalWrite(a2, bitRead(addr, 2));
  digitalWrite(a3, bitRead(addr, 3));
  digitalWrite(a4, bitRead(addr, 4));
  digitalWrite(a5, bitRead(addr, 5));
  digitalWrite(a6, bitRead(addr, 6));
  digitalWrite(a7, bitRead(addr, 7));
  digitalWrite(a8, bitRead(addr, 8));
  digitalWrite(a9, bitRead(addr, 9));
  digitalWrite(a10, bitRead(addr, 10));
  digitalWrite(a11, bitRead(addr, 11));
  digitalWrite(a12, bitRead(addr, 12));
  digitalWrite(a13, bitRead(addr, 13));
  digitalWrite(a14, bitRead(addr, 14));
  digitalWrite(a15, bitRead(addr, 15));
  digitalWrite(a16, bitRead(addr, 16));
  digitalWrite(a17, bitRead(addr, 17));
  digitalWrite(a18, bitRead(addr, 18));
  digitalWrite(a19, bitRead(addr, 19));
  digitalWrite(a20, bitRead(addr, 20));
}

word readWord() {
  word dataWord = 0;

  bitWrite(dataWord, 0, digitalRead(d0));
  bitWrite(dataWord, 1, digitalRead(d1));
  bitWrite(dataWord, 2, digitalRead(d2));
  bitWrite(dataWord, 3, digitalRead(d3));
  bitWrite(dataWord, 4, digitalRead(d4));
  bitWrite(dataWord, 5, digitalRead(d5));
  bitWrite(dataWord, 6, digitalRead(d6));
  bitWrite(dataWord, 7, digitalRead(d7));
  bitWrite(dataWord, 8, digitalRead(d8));
  bitWrite(dataWord, 9, digitalRead(d9));
  bitWrite(dataWord, 10, digitalRead(d10));
  bitWrite(dataWord, 11, digitalRead(d11));
  bitWrite(dataWord, 12, digitalRead(d12));
  bitWrite(dataWord, 13, digitalRead(d13));
  bitWrite(dataWord, 14, digitalRead(d14));
  bitWrite(dataWord, 15, digitalRead(d15));

  return dataWord;
}

word readData(unsigned int addr, int bank) {
  word dataWord;
  digitalWrite(ce, HIGH);
  digitalWrite(csb, HIGH);

  setAddress(addr);

  if (bank == LOW) {
    digitalWrite(ce, LOW);
  } else {
    digitalWrite(csb, LOW);
  }
  delayMicroseconds(1);

  dataWord = readWord();

  digitalWrite(ce, HIGH);
  digitalWrite(csb, HIGH);

  return dataWord;
}


void printHex(int num, int precision) {
     char tmp[16];
     char format[128];

     sprintf(format, "%%.%dX", precision);

     sprintf(tmp, format, num);
     Serial.print(tmp);
}

void readSerialCommand(char in) {
  switch(in) {
    case 'R' :
      Serial.println("CMD: R");
      for (unsigned int count = 0; count <= 0x1FFFFF; count = count + 16) {
        printHex(count, 6);
        Serial.print(":\t\t");
        
        for (int i = 0; i < 16; i++) {
          printHex(readData(count + i, LOW), 4);
          Serial.print("\t");
        }

        Serial.print("\n");
        delay(5);
      }

      for (unsigned int count = 0; count <= 0x1FFFFF; count = count + 16) {
        printHex(count + 0x200000, 6);
        Serial.print(":\t\t");
        
        for (int i = 0; i < 16; i++) {
          printHex(readData(count + i, HIGH), 4);
          Serial.print("\t");
        }

        Serial.print("\n");
        delay(5);
      }
      break;
    case 'L' :
      Serial.println("CMD: R");
      printHex(readData(0x8003, LOW), 4); Serial.print("\n");
      break;
    default:
      Serial.println("Invalid Command");
      break;
  }
}

// the loop routine runs over and over again forever:
void loop() {
  if (Serial.available()) {
    readSerialCommand(Serial.read());
  }
}
