// ShiftPWM library configuration
const int T3ShiftPWM_latchPin = 8;
unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 100;
byte val = 0;
byte last = 0;
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

void print_hex(int v, int num_places)
{
    int mask=0, n, num_nibbles, digit;

    for (n=1; n<=num_places; n++)
    {
        mask = (mask << 1) | 0x0001;
    }
    v = v & mask; // truncate v to specified number of places

    num_nibbles = num_places / 4;
    if ((num_places % 4) != 0)
    {
        ++num_nibbles;
    }

    do
    {
        digit = ((v >> (num_nibbles-1) * 4)) & 0x0f;
        Serial.print(digit, HEX);
    } while(--num_nibbles);

}

void loop() {
  //if (val != last) {
  //  last = val;
  //  Serial.println(val, BIN);
  //}
  
  // read from RawHID with instant timeout
  int n = RawHID.recv(buffer, 0);
  
  // no packet
  if (n == 0) return;
  
  /*for (int i = 0; i < n; i++) {
    if (i % 8 == 0) {
      Serial.println();
    }
    print_hex(buffer[i], 8);
  }
  Serial.println();
  */
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
    
    int hret = handler.handler(&buffer[ptr], &outBuffer[outPtr]);
    if (hret < 0) return hret;
    outPtr += hret;
    return handler.inSize;
  }
  
  return INVALID_CMD;
}

boolean setRGB(int channel, byte r, byte g, byte b) {
  channel = channel * 3;
  
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

void setAllRGBs(byte r, byte g, byte b) {
  forEachChannel(i) {
    // skip non rgb channels
    if (!isRGBChannel(i)) continue;
    
    setRGB(i, r, g, b);
  }
}
