#include <Arduino.h>
#include "main.h"

// v-smile has 22 bits for addressing
// my teensy is only wired for 21 bits, 22 is unused on test cartridge
// Alphabet Park Adventure, 80-092000(US) Rev. 2
uint8_t address [ADDRESS_BITS] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
};

// v-smile uses 16 bit wide data bus
uint8_t data [DATA_BITS] = {
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  32,
  33,
  34,
  35,
  36
};

int chipEnable = 37;
int chipSelect = 38;

void setup() {
  // initialize address bus
  for (int i = 0; i < ADDRESS_BITS; i++) {
    pinMode(address[i], OUTPUT);

    // init low
    digitalWrite(address[i], LOW);
  }

  // initialize data bus
  for (int i = 0; i < DATA_BITS; i++) {
    pinMode(data[i], INPUT_PULLDOWN);
  }

  // initialize control bits
  pinMode(chipEnable, OUTPUT);
  digitalWrite(chipEnable, LOW);

  pinMode(chipSelect, OUTPUT);
  digitalWrite(chipSelect, LOW);

  // initialize serial interface
  Serial.begin(9600);
  delay(2);
  Serial.println("Started Serial COM");
}


void setAddress(unsigned int addr) {
  for (int i = 0; i < ADDRESS_BITS; i++) {
    digitalWriteFast(address[i], bitRead(addr, i));
  }
}

word readWord() {
  word dataWord = 0;

  for (int i = 0; i < DATA_BITS; i++) {
    bitWrite(dataWord, i, digitalReadFast(data[i]));
  }

  return dataWord;
}

word readData(unsigned int addr, int bank) {
  word dataWord;

  digitalWriteFast(chipEnable, HIGH);
  digitalWriteFast(chipSelect, HIGH);

  setAddress(addr);

  // Enable correct chip depending on if high or low bank is being read
  // chip enable and chip select are inverted, low = on
  if (bank == LOW) {
    digitalWriteFast(chipEnable, LOW);
  } else {
    digitalWriteFast(chipSelect, LOW);
  }

  // $TODO: wait TRC is 70ns, this overshoots timing, can be improved
  delayNanoseconds(80);

  dataWord = readWord();

  digitalWrite(chipEnable, HIGH);
  digitalWrite(chipSelect, HIGH);

  return dataWord;
}


// Tool to print an integer in a hexadecimal representation
void printHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%.%dX", precision);

  sprintf(tmp, format, num);
  Serial.print(tmp);
}

uint8_t hexDecimalToBin(char decimal) {
  switch(decimal) {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'a':
      return 10;
    case 'b':
      return 11;
    case 'c':
      return 12;
    case 'd':
      return 13;
    case 'e':
      return 14;
    case 'f':
      return 15;
    default:
      return 0;
  }
}

COMMAND stringToCommand(const String &commandString, uint32_t &address) {
  if (commandString == String("BLOW")) {
    return BANK_LOW;
  } else if (commandString == String("BHI")) {
    return BANK_HIGH;
  } else if (commandString == String("ACK")) {
    return ACK;
  } else if (commandString.startsWith(String('R'))) {
    String addressString = commandString.substring(1);

    address = 0;
    for (uint i = 0; i < addressString.length(); i++) {
      uint8_t hexDecimal = hexDecimalToBin(addressString.charAt(i));

      uint8_t offset = (addressString.length() - 1 - i) * 4;

      address = address + ((hexDecimal & 0xF) << offset);
    }

    address = address & 0x3FFFFF;
    return READ;
  } else {
    return UNKNOWN;
  }
}

uint bank = LOW;

void readSerialCommand(String command) {
  uint32_t address = 0;

  switch(stringToCommand(command, address)) {
    case(BANK_LOW):
      bank = LOW;
      return;
    case(BANK_HIGH):
      bank = HIGH;
      return;
    case(READ):
      printHex(readData(address, bank), 4);
      Serial.print("\n");
      return;
    case(ACK):
    default:
      Serial.println("Unused");
      return;
  }
}

void loop() {
  // Loop, waiting for serial commands
  if (Serial.available()) {
    readSerialCommand(Serial.readStringUntil('%'));
  }
}