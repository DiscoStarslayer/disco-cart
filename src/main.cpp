
#include <Arduino.h>
#include "SdFat.h"
#include "main.h"
#include "RingBuf.h"

#define SPI_CLOCK SD_SCK_MHZ(50)

const size_t RING_BUF_SIZE = 400*512;

// v-smile has 22 bits for addressing
const uint8_t address [ADDRESS_BITS] = {
  9, // A0
  39,
  40,
  10,
  38,
  11, // A5
  37,
  12,
  35,
  25,
  34, // A10
  26,
  33,
  27,
  28,
  29, // A15
  8,
  36,
  24,
  30,
  31,
  32 // A21
};

// v-smile uses 16 bit wide data bus
const uint8_t data [DATA_BITS] = {
  3, // DQ0
  2,
  1,
  0,
  19,
  18, // DQ5
  17,
  16,
  7,
  6,
  5, // DQ10
  4,
  15,
  14,
  41,
  13 // DQ15
};

const int chipEnable = 23;
const int chipSelect = 22;
const int writeEnable = 21;
const int outputEnable = 20;

SdFs sd;
FsFile file;
bool sdDetected = false;

RingBuf<FsFile, RING_BUF_SIZE> ringBuf;

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

  pinMode(writeEnable, OUTPUT);
  digitalWrite(writeEnable, HIGH);

  pinMode(outputEnable, OUTPUT);
  digitalWrite(outputEnable, LOW);

  // initialize serial interface
  Serial.begin(4608000);
  delay(2);
  Serial.println("Started Serial COM");

//  if (!sd.begin(SdioConfig(FIFO_SDIO))) {
//    Serial.println("Failed to detect sd card");
//    sd.initErrorHalt(&Serial);
//  } else {
    sdDetected = false;
//  }
}

void setupWrite() {
  // initialize address bus
  for (int i = 0; i < ADDRESS_BITS; i++) {
    pinMode(address[i], OUTPUT);

    // init low
    digitalWrite(address[i], LOW);
  }

  // initialize data bus
  for (int i = 0; i < DATA_BITS; i++) {
    pinMode(data[i], OUTPUT);
    digitalWrite(data[i], LOW);
  }

  // initialize control bits
  pinMode(chipEnable, OUTPUT);
  digitalWrite(chipEnable, LOW);

  pinMode(chipSelect, OUTPUT);
  digitalWrite(chipSelect, LOW);

  pinMode(writeEnable, OUTPUT);
  digitalWrite(writeEnable, HIGH);

  pinMode(outputEnable, OUTPUT);
  digitalWrite(outputEnable, HIGH);
}


void setAddress(unsigned int addr) {
  for (int i = 0; i < ADDRESS_BITS; i++) {
    digitalWriteFast(address[i], bitRead(addr, i));
  }
}

void setData(uint16_t wrd) {
  for (int i = 0; i < DATA_BITS; i++) {
    digitalWriteFast(data[i], bitRead(wrd, i));
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

  delayNanoseconds(70);

  dataWord = readWord();

  digitalWrite(chipEnable, HIGH);
  digitalWrite(chipSelect, HIGH);

  return dataWord;
}

void writeWord(uint32_t addr, uint16_t wrd) {
  digitalWriteFast(writeEnable, HIGH);
  digitalWriteFast(outputEnable, HIGH);
  digitalWriteFast(chipEnable, LOW);

  setAddress(addr);
  setData(wrd);

  delayNanoseconds(15);

  digitalWriteFast(writeEnable, LOW);

  delayNanoseconds(40);

  digitalWriteFast(writeEnable, HIGH);

  delayNanoseconds(15);
}

void writeData(uint32_t addr, uint16_t wrd) {
  writeWord(0x555, 0xAA);
  writeWord(0x2AA, 0x55);
  writeWord(0x555, 0xA0);
  writeWord(addr, wrd);

  delayMicroseconds(10);
}

void eraseData() {
  writeWord(0x555, 0xAA);
  writeWord(0x2AA, 0x55);
  writeWord(0x555, 0x80);
  writeWord(0x555, 0xAA);
  writeWord(0x2AA, 0x55);
  writeWord(0x555, 0x10);

  delayMicroseconds(50000);
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

COMMAND stringToCommand(const String &commandString, uint32_t &address, uint16_t &wrd, char fileName[]) {
  if (commandString == String("BLOW")) {
    return BANK_LOW;
  } else if (commandString == String("BHI")) {
    return BANK_HIGH;
  } else if (commandString == String("ACK")) {
    return ACK;
  } else if (commandString == String("ERS")) {
    return ERASE;
  } else if (commandString.startsWith(String('R'))) {
    String addressString = commandString.substring(1);

    address = 0;
    for (uint32_t i = 0; i < addressString.length(); i++) {
      uint8_t hexDecimal = hexDecimalToBin(addressString.charAt(i));

      uint8_t offset = (addressString.length() - 1 - i) * 4;

      address = address + ((hexDecimal & 0xF) << offset);
    }

    address = address & 0x3FFFFF;
    return READ;
  } else if (commandString.startsWith(String('W'))) {
    String addressString = commandString.substring(1);

    address = 0;
    uint32_t splitPoint = 0;
    uint8_t decimals[7] = {0, 0, 0, 0, 0, 0, 0};
    for (; addressString.charAt(splitPoint) != ':'; splitPoint++) {
      decimals[splitPoint] = hexDecimalToBin(addressString.charAt(splitPoint));
    }

    for (uint32_t j = 0; j < splitPoint; j++) {
      uint8_t offset = (splitPoint - 1 - j) * 4;

      address = address + ((decimals[j] & 0xF) << offset);
    }

    address = address & 0x3FFFFF;

    String wordString = commandString.substring(splitPoint + 1);

    wrd = 0;
    for (uint32_t i = 0; i < wordString.length(); i++) {
      uint8_t hexDecimal = hexDecimalToBin(wordString.charAt(i));

      uint8_t offset = (wordString.length() - 1 - i) * 4;

      wrd = wrd + ((hexDecimal & 0xF) << offset);
    }

    return WRITE;
  } else if (commandString.startsWith(String('D'))) {
    String dumpString = commandString.substring(1);

    uint32_t splitPoint = 0;
    for (; dumpString.charAt(splitPoint) != ':'; splitPoint++) {
      fileName[splitPoint] = dumpString.charAt(splitPoint);
    }

    fileName[splitPoint + 1] = {'\0'};

    String limitString = commandString.substring(splitPoint + 1);

    address = 0;
    for (uint32_t i = 0; i < limitString.length(); i++) {
      uint8_t hexDecimal = hexDecimalToBin(limitString.charAt(i));
      uint8_t offset = (limitString.length() - 1 - i) * 4;

      address = address + ((hexDecimal & 0xF) << offset);
    }

    address = address & 0x3FFFFFF;

    return DUMP;
  } else {
    return UNKNOWN;
  }
}

uint32_t bank = LOW;
MODE currentMode = MODE_READ;

void switchMode(MODE mode) {
 if (mode == MODE_WRITE) {
   if (currentMode != MODE_WRITE) {
     setupWrite();
     currentMode = MODE_WRITE;

     delayMicroseconds(10);
   }
 } else {
   if (currentMode != MODE_READ) {
     setup();
     currentMode = MODE_READ;

     delayMicroseconds(10);
   }
 }
}

typedef struct {
  uint16_t wrd;
  uint32_t freq;
} differences;

uint32_t retryCount = 35;
uint16_t result[35];
differences diffs[35];
uint32_t difCnt;
uint32_t largestFreq;

void readSerialCommand(String command) {
  uint32_t address = 0;
  uint16_t wrd = 0;
  size_t maxUsed = 0;
  char fileName[FILE_NAME_LIMIT] = "";

  switch(stringToCommand(command, address, wrd, fileName)) {
    case(BANK_LOW):
      bank = LOW;
      return;
    case(BANK_HIGH):
      bank = HIGH;
      return;
    case(ERASE):
      switchMode(MODE_WRITE);

      eraseData();

      Serial.print("ACK\n");
      return;
    case(READ):
      switchMode(MODE_READ);

      for (uint32_t i = 0; i < retryCount; i++) {
        delayMicroseconds(1);
        result[i] = readData(address, bank);
      }

      difCnt = 1;
      largestFreq = 0;
      diffs[0] = {result[0], 1};

      for (uint32_t i = 0; i < retryCount; i++) {
        bool matchFound = false;
        for (uint32_t j = 0; j < difCnt; j++) {
          if (diffs[j].wrd == result[i]) {
            if (diffs[j].wrd != 0x0000) {
              diffs[j] = {result[i], diffs[j].freq + 1};
            }
            matchFound = true;
            break;
          }
        }

        if (!matchFound) {
          diffs[difCnt] = {result[i], 1};
          difCnt++;
        }
      }

      for (uint32_t i = 0; i < difCnt; i++) {
        if (diffs[i].freq > largestFreq) {
          result[0] = diffs[i].wrd;
          largestFreq = diffs[i].freq;
        }
      }

      printHex(result[0], 4);
      Serial.print("\n");
      return;
    case(WRITE):
      switchMode(MODE_WRITE);

      writeData(address, wrd);

      Serial.print("ACK\n");
      return;
    case(DUMP):
      switchMode(MODE_READ);

      if (!sdDetected) {
        Serial.println("No SD card!");
        return;
      }

      Serial.print("Writing file to ");
      Serial.print(fileName);
      Serial.print('\n');

      if (!file.open(fileName, O_RDWR | O_CREAT | O_TRUNC)) {
        Serial.println("Failed to open file!");
        return;
      }

      if (!file.preAllocate(address * 2)) {
        Serial.println("Failed to preallocate!");
        file.close();
        return;
      }

      ringBuf.begin(&file);

      for (uint32_t i = 0; i < address; i++) {
        if (i % 0xFFFF == 0) {
          Serial.print("Dumping... ");
          Serial.print((i * 100) / address);
          Serial.print("%\n");
        }

        size_t n = ringBuf.bytesUsed();

        if (n > maxUsed) {
          maxUsed = n;
        }

        if (n >= 512 && !file.isBusy()) {
          ringBuf.writeOut(512);
        }

        uint16_t data = readData(i, bank);

        ringBuf.write(&data, 2);
      }

      ringBuf.sync();
      file.truncate();
      file.close();

      Serial.print("Done! Max bytes used: ");
      Serial.print(maxUsed);
      Serial.print('\n');

      setup();
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
