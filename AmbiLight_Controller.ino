// ShiftPWM library configuration
const int T3ShiftPWM_latchPin = 8;
unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 100;

#include <SPI.h>
#include <T3ShiftPWM.h>

#include "commands.h"
#include "channels.h"

// RawHID packet buffer
byte buffer[64];

/*
 * handles a command at the specified index in buffer
 * return the number of data bytes taken, or -1 for invalid command
 */
int handleCommand(byte *buffer, int ptr, int maxDataSize);

void setup() {
  Serial.begin(115200);
  setupChannels();
  T3ShiftPWM.SetRegisterCount(numShiftRegisters);
  T3ShiftPWM.SetInterruptHandler(T3ShiftPWM_InterruptHandler);
  T3ShiftPWM.Start(pwmFrequency, maxBrightness);
  delay(5000);
  Serial.println("AmbiLight Controller Active");
}

void loop() {
  // read from RawHID with instant timeout
  int n = RawHID.recv(buffer, 0);
  
  // no packet
  if (n == 0) return;
  
  // check magic bytes
  if (buffer[0] != MAGIC_BYTE0 || buffer[1] != MAGIC_BYTE1) return;
  
  // check data length
  int dataLength = buffer[2];
  if (dataLength > MAX_DATA_LENGTH) return;
  
  // check packet terminator
  if (buffer[dataLength + HEADER_LENGTH] != PACKET_TERMINATOR) return;
  
  // set pointer to start of data
  int ptr = HEADER_LENGTH;
  int dataEnd = ptr + dataLength;
  
  while (ptr < dataEnd) {
    int dataBytes = handleCommand(buffer, ptr++, dataEnd - ptr);
    if (dataBytes < 0) return; // invalid command
    ptr += dataBytes;
  }
}

int handleCommand(byte *buffer, int ptr, int maxDataSize) {
  byte commandCode = buffer[ptr++];
  
  for (int i = 0; i < num_handlers; i++) {
    // check command code
    if (handlers[i].commandCode != commandCode) continue;
    
    if (handlers[i].inSize > maxDataSize) return BUFFER_ERROR;
    
    // TODO: output buffer
    int outSize = handlers[i].handler(&buffer[ptr], &buffer[ptr]);
    return handlers[i].inSize;
  }
  
  return INVALID_CMD;
}

boolean setChannel(int channel, byte val) {
  // validate channel first
  if (!isValidChannel(channel)) return false;
  
  if (isPWMChannel(channel)) {
    analogWrite(pwmPin(channel), val);
  } else {
    T3ShiftPWM.m_values[channel] = val;
  }
  
  return true;
}

boolean setRGB(int channel, byte r, byte g, byte b) {
  // validate channel first
  if (!isValidChannel(channel)) return false;
  
  if (!isRGBChannel(channel)) return false;
  
  if (isPWMChannel(channel)) {
    analogWrite(pwmPin(channel), r);
    analogWrite(pwmPin(channel + 1), g);
    analogWrite(pwmPin(channel + 2), b);
  } else {
    T3ShiftPWM.m_values[channel] = r;
    T3ShiftPWM.m_values[channel + 1] = g;
    T3ShiftPWM.m_values[channel + 2] = b;
  }
  
  return true;
}

void setAllChannels(byte val) {
  forEachChannel(i) {
    setChannel(i, val);
  }
}

void setAllRGBs(byte r, byte g, byte b) {
  forEachChannel(i) {
    // skip non rgb channels
    if (!isRGBChannel(i)) continue;
    
    setRGB(i, r, g, b);
  }
}
