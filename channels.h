#ifndef channels_h_
#define channels_h_

extern const byte channels[];
extern const int num_channels;
extern const Command handlers[];
extern const int num_handlers;
extern int numShiftRegisters;

#define CHANNEL_COUNT 33

#define PINMASK 0x3F
#define ISRGB   0x40
#define ISPWM   0x80

#define PIN(p)  (p & PINMASK)
#define PWM(p)  (ISPWM | PIN(p))
#define isValidChannel(c) (c >= 0 && c < num_channels)
#define isRGBChannel(c)   ((channels[c] & ISRGB) == ISRGB)
#define isPWMChannel(c)   ((channels[c] & ISPWM) == ISPWM)
#define pwmPin(c)         PIN(channels[c])

#define forEachChannel(x)    for (int x = 0; x < num_channels; x++)


const byte channels[] = {
  // ShiftPWM Channels
  ISRGB, 0, 0, // RGB1
  ISRGB, 0, 0, // RGB2
  ISRGB, 0, 0, // RGB3
  ISRGB, 0, 0, // RGB4
  ISRGB, 0, 0, // RGB5
  ISRGB, 0, 0, // RGB6
  ISRGB, 0, 0, // RGB7
  ISRGB, 0, 0, // RGB8
  
  // PWM Channels
  ISRGB | PWM(5),  PWM(4),  PWM(3),  //RGB9
  ISRGB | PWM(10), PWM(9),  PWM(6),  //RGB10
  ISRGB | PWM(23), PWM(22), PWM(21), //RGB11
};

const int num_channels = sizeof(channels) / sizeof(byte);
int numShiftOutputs = 0;
int numShiftRegisters = 0;

void setupChannels() {
  forEachChannel(i) {
    if (isPWMChannel(i)) {
      pinMode(pwmPin(i), OUTPUT);
    } else {
      numShiftOutputs++;
    }
  }
  
  numShiftRegisters = numShiftOutputs / 8;
}

void setupPWMChannels() {
  forEachChannel(i) {
    // skip ShiftPWM channels
    if (!isPWMChannel(i)) continue;
    
    // set pins to output
    pinMode(pwmPin(i), OUTPUT);
  }
}

#endif
