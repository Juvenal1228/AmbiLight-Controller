/*
 * RawHID Packets are formatted as such
 * +-----+-------+-------------+---------+------------+
 * | magic bytes | data length | payload | terminator |
 * +-----+-------+-------------+---------+------------+
 *
 * data length must not be greater than 60 bytes
 * the payload must be data length bytes long
 * and follow the format
 * +----------+-------+-----+
 * | cmd code | data? | ... |
 * +----------+-------+-----+
 *
 * data is optional, and depends on the command
 * any number of commands can be issued in a single payload
 */

#ifndef commands_h_
#define commands_h_

/*
 * negative return indicates error
 * positive return indicates output size
 * zero return indicates no output
 */
typedef int (*CommandHandler)(byte* inData, byte* outData);

typedef struct {
  byte commandCode;
  
  // size of input data required, negative for variable, magnitude defines max
  int inSize;
  
  // size of output data required, negative for variable, magnitude defines max
  int outSize;
  
  // handler to run
  CommandHandler handler;
} Command;

#define MAGIC_BYTE0 'a'
#define MAGIC_BYTE1 'l'
#define MAGIC_BYTES { MAGIC_BYTE0, MAGIC_BYTE1 }

#define PACKET_LENGTH 64
#define HEADER_LENGTH 3
#define MAX_DATA_LENGTH PACKET_LENGTH - HEADER_LENGTH - 1 // 1 for terminator
#define PACKET_TERMINATOR 0

#define NO_DATA 0
#define INVALID_CMD -1
#define BUFFER_ERROR -2
#define TRANSMIT_ERROR -3
#define TIMEOUT_ERROR -4
#define CHAIN_ERROR -5

/*
 * Print interrrupt load over serial
 * No Input Data
 * No Output Data
 * TODO: Should return load instead of writing over serial
 */
#define CMD_PRINTLOAD   0x01

/*
 * Turns off all output channels
 * No Input Data
 * No Output Data
 */
#define CMD_ALLOFF      0x02

/*
 * Sets all output channels to the same value
 * Input Data:
 *  +-------+
 *  | Value |
 *  +-------+
 *
 * No Output Data
 */
#define CMD_ALLSET      0x03

/*
 * Set all RGB groups to an RGB value
 * Input Data:
 *  +---+---+---+
 *  | R | G | B |
 *  +---+---+---+
 *
 * No Output Data
 */
#define CMD_ALLRGB      0x04

/*
 * Turns off a specified channel
 * Input Data:
 *  +---------+
 *  | Channel |
 *  +---------+
 *
 * No Output Data
 */
#define CMD_CHANOFF     0x05

/*
 * Set a specified channel value
 * Input Data:
 *  +---------+-------+
 *  | Channel | Value |
 *  +---------+-------+
 *
 * No Output Data
 */
#define CMD_CHANSET     0x06

/*
 * Set a specified RGB group to an RGB value
 * Input Data:
 *  +---------+---+---+---+
 *  | Channel | R | G | B |
 *  +---------+---+---+---+
 *
 * No Output Data
 */
#define CMD_CHANRGB     0x07

/*
 * Returns the number of channels
 * No Input Data
 * Output Data:
 *  +--------------------+
 *  | Number Of Channels |
 *  +--------------------+
 */
#define CMD_NUMCHANS  0x08
#endif
