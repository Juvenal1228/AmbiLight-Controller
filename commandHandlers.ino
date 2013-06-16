#include "commands.h"

const Command handlers[] = {
  // command code,  inSize, outSize, handler
  { CMD_PRINTLOAD, NO_DATA, NO_DATA, handle_printLoad },
  { CMD_ALLOFF,    NO_DATA, NO_DATA, handle_allOff },
  { CMD_ALLSET,          1, NO_DATA, handle_allSet },
};

const int num_handlers = sizeof(handlers) / sizeof(Command);


int handle_printLoad(byte* inData, byte* outData) {
  T3ShiftPWM.PrintInterruptLoad();
  return NO_DATA;
}

int handle_allOff(byte* inData, byte* outData) {
  setAllChannels(0);
  return NO_DATA;
}

int handle_allSet(byte* inData, byte* outData) {
  byte val = inData[0];
  setAllChannels(val);
  return NO_DATA;
}
