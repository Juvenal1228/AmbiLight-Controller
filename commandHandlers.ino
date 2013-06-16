#include "commands.h"

const Command handlers[] = {
  // command code,  inSize, outSize, handler
  { CMD_PRINTLOAD, NO_DATA, NO_DATA, handle_printLoad },
  { CMD_ALLOFF,    NO_DATA, NO_DATA, handle_allOff },
  { CMD_ALLSET,          1, NO_DATA, handle_allSet },
  { CMD_ALLRGB,          3, NO_DATA, handle_allRGB },
  { CMD_CHANOFF,         1, NO_DATA, handle_chanOff },
  { CMD_CHANSET,         2, NO_DATA, handle_chanSet },
  { CMD_CHANRGB,         4, NO_DATA, handle_chanRGB },
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

int handle_allRGB(byte* inData, byte* outData) {
  byte r = inData[0];
  byte g = inData[1];
  byte b = inData[2];
  
  setAllRGBs(r, g, b);
  
  return NO_DATA;
}

int handle_chanOff(byte* inData, byte* outData) {
  int chan = inData[0];
  
  setChannel(chan, 0);
  
  return NO_DATA;
}

int handle_chanSet(byte* inData, byte* outData) {
  int chan = inData[0];
  byte val = inData[1];
  
  setChannel(chan, val);
  
  return NO_DATA;
}

int handle_chanRGB(byte* inData, byte* outData) {
  int chan = inData[0];
  byte r = inData[1];
  byte g = inData[2];
  byte b = inData[3];
  
  setRGB(chan, r, g, b);
  
  return NO_DATA;
}
