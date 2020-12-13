#ifndef VSMILE_DUMPER_MAIN_H
#define VSMILE_DUMPER_MAIN_H

const int ADDRESS_BITS = 21; // Only 21 wired in test device, V-smile has 22-bit bus
const int DATA_BITS = 16;

/**
 * Commands
 * Rxxxxxx: Read 14 bits in from a given 22bit hex address
 * BLOW: Set chip select + chip enable to read from first bank (default)
 * BHI: Set chip select + chip enable to read from second bank
 * ACK: Acknowledge byte transfer, after dumper sends data it awaits for an ACK for 18ms
 *
 * Terminator character: %
 **/
enum COMMAND {
  READ,
  BANK_LOW,
  BANK_HIGH,
  ACK,
  UNKNOWN
};

const char TERMINATOR = '%';

#endif //VSMILE_DUMPER_MAIN_H
