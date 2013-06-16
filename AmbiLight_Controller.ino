// ShiftPWM library configuration
const int T3ShiftPWM_latchPin = 8;
unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 100;

#include <SPI.h>
#include <T3ShiftPWM.h>

#include "commands.h"
#include "channels.h"

// RawHID packet buffers
byte buffer[64];
byte outBuffer[MAX_DATA_LENGTH];
int outPtr = 0;

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
    if (dataBytes < 0) {
      Serial.print("Error Processing Packet: ");
      Serial.println(dataBytes);
      break;
    }
    ptr += dataBytes;
  }
  
  if (outPtr > 0) {
    int ret = sendRawHIDPacket(outBuffer, outPtr);
    if (ret > 0) {
      memset(outBuffer, 0, sizeof(outBuffer));
      outPtr = 0;
    }
  }
}

int sendRawHIDPacket(byte* data, int dataSize) {
  if (dataSize > MAX_DATA_LENGTH) return BUFFER_ERROR;
  
  byte buf[64] = {0};
  
  buf[0] = MAGIC_BYTE0;
  buf[1] = MAGIC_BYTE1;
  buf[2] = dataSize;
  
  memcpy(&buf[3], data, dataSize);
  
  return RawHID.send(buf, 0);
}

int handleCommand(byte *buffer, int ptr, int maxDataSize) {
  byte commandCode = buffer[ptr++];
  Command handler;
  
  for (int i = 0; i < num_handlers; i++) {
    handler = handlers[i];
    // check command code
    if (handler.commandCode != commandCode) continue;
    
    if (handler.inSize > maxDataSize) return BUFFER_ERROR;
    
    // variable size output commands need to be the only in the chain
    if (handler.outSize < 0 && outPtr != 0) {
      return CHAIN_ERROR;
    }
    
    // flush output if packet is full
    if (handler.outSize > 0 && handler.outSize + outPtr > MAX_DATA_LENGTH) {
      int ret = sendRawHIDPacket(outBuffer, outPtr);
      if (ret <= -1) return TRANSMIT_ERROR;
      if (ret <= 0) return TIMEOUT_ERROR;
      
      memset(outBuffer, 0, sizeof(outBuffer));
      outPtr = 0;
    }
    
    outPtr += handler.handler(&buffer[ptr], &outBuffer[outPtr]);
    return handler.inSize;
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
