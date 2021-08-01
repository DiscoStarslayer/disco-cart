#ifndef VSMILE_DUMPER_MAIN_H
#define VSMILE_DUMPER_MAIN_H

#define FILE_NAME_LIMIT 20

const int ADDRESS_BITS = 22; // V-smile has 22-bit addressing width
const int DATA_BITS = 16; // 16 bit data bus

/**
 * Commands
 * Rxxxxxx: Read word in from a given 22bit hex address
 * Wxxxxxx:xxxx: Write a hex word to a given 22bit hex address
 * BLOW: Set chip select + chip enable to read from first bank (default)
 * BHI: Set chip select + chip enable to read from second bank
 * ACK: Acknowledge byte transfer, after dumper sends data it awaits for an ACK for 18ms
 *
 * Dxxxx:ssss Dumps attached cartridge to sd-card with filename x and size s
 * Fxxxx: Flashes rom from sd-card to cartridge
 *
 * Terminator character: %
 **/
enum COMMAND {
  READ,
  WRITE,
  ERASE,
  BANK_LOW,
  BANK_HIGH,
  ACK,
  DUMP,
  UNKNOWN
};

enum MODE {
  MODE_READ,
  MODE_WRITE
};

const char TERMINATOR = '%';

#endif //VSMILE_DUMPER_MAIN_H
