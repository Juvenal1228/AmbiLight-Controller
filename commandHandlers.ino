#include "commands.h"

const Command handlers[] = {
  // command code,  inSize, outSize, handler
  { CMD_PRINTLOAD, NO_DATA, NO_DATA, handle_printLoad },
  { CMD_ALLOFF,    NO_DATA, NO_DATA, handle_allOff },
  { CMD_ALLRGB,          3, NO_DATA, handle_allRGB },
  { CMD_CHANOFF,         1, NO_DATA, handle_chanOff },
  { CMD_CHANRGB,         4, NO_DATA, handle_chanRGB },
  { CMD_NUMCHANS,  NO_DATA,       2, handle_numChans },
};

const int num_handlers = sizeof(handlers) / sizeof(Command);


int handle_printLoad(byte* inData, byte* outData) {
  T3ShiftPWM.PrintInterruptLoad();
  
  return NO_DATA;
}

int handle_allOff(byte* inData, byte* outData) {
  setAllRGBs(0, 0, 0);
  
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

  if (!setRGB(chan, 0, 0, 0)) return CHAN_ERROR;
  
  return NO_DATA;
}

int handle_chanRGB(byte* inData, byte* outData) {
  int chan = inData[0];
  byte r = inData[1];
  byte g = inData[2];
  byte b = inData[3];

  if (!setRGB(chan, r, g, b)) return CHAN_ERROR;
  
  return NO_DATA;
}

int handle_numChans(byte* inData, byte* outData) {
  outData[0] = CMD_NUMCHANS;
  outData[1] = (byte)num_channels;
  
  return 2;
}
