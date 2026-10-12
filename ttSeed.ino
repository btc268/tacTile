

#include <DaisyDuino.h>

#include "utility/gpio.h"

#include <seesaw_neopixel.h>

DaisyHardware hw;
#define MAX_SIZE (48000 * 60 * 2)  // 2 minutes loop time
Metro tick;
size_t num_channels;

HardwareSerial hwSerial(1, 2);  // ESP32 handles PN532 reads + external sensors and reports results over serial



#define PIN 0
// ATtiny1616 drives neopixels, talks to Seed over i2c
seesaw_NeoPixel strip = seesaw_NeoPixel(36, PIN, NEO_GRB + NEO_KHZ800);

#define CALIBRATIONMODE 0


#define NUMCHORDS 8
#define NUMVOICESPERCHORD 3
#define NUMZONES 9
#define NUMMAINVOICES 24
#define NUMARPVOICES 8

float mainVoiceLevelModifier[NUMMAINVOICES];
float arpVoiceLevelModifier[NUMARPVOICES];


// MAIN VOICES
static Oscillator osc0;
static Oscillator osc1;
static Oscillator osc2;
static Oscillator osc3;

static Oscillator osc4;
static Oscillator osc5;
static Oscillator osc6;
static Oscillator osc7;

static Oscillator osc8;
static Oscillator osc9;
static Oscillator osc10;
static Oscillator osc11;

static Oscillator osc12;
static Oscillator osc13;
static Oscillator osc14;
static Oscillator osc15;

static Oscillator osc16;
static Oscillator osc17;
static Oscillator osc18;
static Oscillator osc19;

static Oscillator osc20;
static Oscillator osc21;
static Oscillator osc22;
static Oscillator osc23;


Oscillator *mainVoiceOscillator[NUMMAINVOICES] = {
  &osc0,
  &osc1,
  &osc2,
  &osc3,
  &osc4,
  &osc5,
  &osc6,
  &osc7,
  &osc8,
  &osc9,
  &osc10,
  &osc11,
  &osc12,
  &osc13,
  &osc14,
  &osc15,
  &osc16,
  &osc17,
  &osc18,
  &osc19,
  &osc20,
  &osc21,
  &osc22,
  &osc23,
};



//ARP VOICES
static Oscillator osc32;
static Oscillator osc33;
static Oscillator osc34;
static Oscillator osc35;

static Oscillator osc36;
static Oscillator osc37;
static Oscillator osc38;
static Oscillator osc39;

Oscillator *arpVoiceOscillator[NUMARPVOICES] = {
  &osc32,
  &osc33,
  &osc34,
  &osc35,
  &osc36,
  &osc37,
  &osc38,
  &osc39,
};



//AA VOICES
static Oscillator osc41;
static Oscillator osc42;
static Oscillator osc43;


Oscillator *aaVoiceOscillator[3] = {
  &osc41,
  &osc42,
  &osc43,
};




// DRUM VOICES
// kick
static Oscillator osc44;
static Oscillator osc45;
static Oscillator osc46;

Oscillator *kickOscillator[3] = {
  &osc44,
  &osc45,
  &osc46,
};


// snare
static WhiteNoise snareNoise;


// hihat
static WhiteNoise hihatNoise;


// MAIN VOICE ENVELOPES
static Adsr env0;
static Adsr env1;
static Adsr env2;
static Adsr env3;
static Adsr env4;
static Adsr env5;
static Adsr env6;
static Adsr env7;
static Adsr env8;
static Adsr env9;
static Adsr env10;
static Adsr env11;
static Adsr env12;
static Adsr env13;
static Adsr env14;
static Adsr env15;
static Adsr env16;
static Adsr env17;
static Adsr env18;
static Adsr env19;
static Adsr env20;
static Adsr env21;
static Adsr env22;
static Adsr env23;
static Adsr env24;
static Adsr env25;
static Adsr env26;
static Adsr env27;
static Adsr env28;
static Adsr env29;
static Adsr env30;
static Adsr env31;
static Adsr env32;

Adsr *mainVoiceEnvelope[33] = {
  &env0,
  &env1,
  &env2,
  &env3,
  &env4,
  &env5,
  &env6,
  &env7,
  &env8,
  &env9,
  &env10,
  &env11,
  &env12,
  &env13,
  &env14,
  &env15,
  &env16,
  &env17,
  &env18,
  &env19,
  &env20,
  &env21,
  &env22,
  &env23,
  &env24,
  &env25,
  &env26,
  &env27,
  &env28,
  &env29,
  &env30,
  &env31,
  &env32,
};


bool mainVoiceGate[NUMMAINVOICES];

//ARP ENVELOPE
static Adsr env33;
static Adsr env34;
static Adsr env35;
static Adsr env36;
static Adsr env37;
static Adsr env38;
static Adsr env39;
static Adsr env40;

Adsr *arpVoiceEnvelope[NUMARPVOICES] = {
  &env33,
  &env34,
  &env35,
  &env36,
  &env37,
  &env38,
  &env39,
  &env40,
};

bool arpVoiceGate[NUMARPVOICES];


//AA ENVELOPE
static Adsr env41;
static Adsr env42;
static Adsr env43;


Adsr *aaVoiceEnvelope[3] = {
  &env33,
  &env34,
  &env35,
};

bool aaVoiceGate[3];


// DRUM ENVELOPE
static Adsr kickEnv;
bool kickGate = 0;

static Adsr snareEnv;
bool snareGate = 0;

static Adsr hihatEnv;
bool hihatGate = 0;


// MAIN VOICE FILTER
static MoogLadder filter0;
static MoogLadder filter1;
static MoogLadder filter2;
static MoogLadder filter3;
static MoogLadder filter4;
static MoogLadder filter5;
static MoogLadder filter6;
static MoogLadder filter7;

MoogLadder *mainVoiceFilter[NUMCHORDS] = {
  &filter0,
  &filter1,
  &filter2,
  &filter3,
  &filter4,
  &filter5,
  &filter6,
  &filter7,
};


// ARP FILTER
Svf arpVoiceFilter;

// AA FILTER
static MoogLadder filter41;
static MoogLadder filter42;
static MoogLadder filter43;


MoogLadder *aaVoiceFilter[3] = {
  &filter41,
  &filter42,
  &filter43,
};


// DRUM FILTER
// Svf snareHighPass;
// Svf snareLowPass;
// Svf hihatHighPass;
// Svf hihatLowPass;

static Tone aaDrumFilter;


// TREMOLO
static Tremolo tremolo0;
static Tremolo tremolo1;
static Tremolo tremolo2;
static Tremolo tremolo3;
static Tremolo tremolo4;
static Tremolo tremolo5;
static Tremolo tremolo6;
static Tremolo tremolo7;

Tremolo *mainVoiceTremolo[NUMCHORDS] = {
  &tremolo0,
  &tremolo1,
  &tremolo2,
  &tremolo3,
  &tremolo4,
  &tremolo5,
  &tremolo6,
  &tremolo7,
};

// BITCRUSH
static Decimator decimator;

//Cmaj
int chordNote[8][4] = {
  { 36, 52, 43, 59 },
  { 38, 53, 45, 60 },
  { 40, 55, 47, 62 },
  { 41, 57, 48, 64 },
  { 43, 59, 50, 65 },
  { 45, 60, 52, 67 },
  { 47, 62, 53, 69 },
  { 48, 64, 55, 71 },
};

int arpNote[8][12] = {
  { 36, 38, 40, 43, 45, 48, 50, 52, 55, 57, 60, 62 },
  { 38, 41, 43, 45, 48, 50, 53, 55, 57, 60, 62, 65 },
  { 40, 43, 45, 47, 50, 52, 55, 57, 59, 62, 64, 67 },
  { 41, 43, 45, 47, 50, 53, 55, 57, 60, 62, 65, 67 },
  { 43, 45, 47, 50, 52, 55, 57, 59, 62, 64, 67, 69 },
  { 45, 48, 50, 52, 55, 57, 60, 62, 64, 67, 69, 72 },
  { 47, 50, 52, 53, 57, 59, 62, 64, 65, 69, 71, 74 },
  { 48, 50, 52, 55, 57, 60, 62, 64, 67, 69, 72, 74 },
};

int orderedNeopixel[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 28, 29, 30, 31, 8, 9, 10, 11, 24, 25, 26, 27, 12, 13, 14, 15, 20, 21, 22, 23, 16, 17, 18, 19 };

int zoneSensorPin[9] = { A0, A1, A2, A3, A4, A5, A6, A7, A8 };

int lastSensorReading[9] = {};
int sensorReading[9] = {};

int zoneReading[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float floatZoneReading[9];
int lastZoneReading[9] = {};

int zoneReadingForCallback[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// max brightness divided by 60
int colorBase[8][3] = {
  { 0, 4, 0 },
  { 1, 4, 0 },
  { 4, 4, 0 },
  { 4, 0, 0 },
  { 3, 0, 4 },
  { 0, 1, 4 },
  { 0, 4, 4 },
  { 4, 4, 4 },
};

int buttonPin = D26;
int ledPin = D27;
int irRxPin = D7;
int irTxPin = D10;
int octaveJoystickSelectPin = D8;
int relayPin = D28;
int ampMutePin = D5;
int jackInsertSwitchPin = D6;

bool lastArpActiveState = false;
bool arpActiveState = false;

bool irRxState = false;
bool lastIrRxState = false;
bool irTxState = false;
bool lastIrTxState = false;

bool hallState = HIGH;
bool lastHallState = HIGH;

bool octaveJoystickSelectState = HIGH;
bool lastOctaveJoystickSelectState = HIGH;
bool octaveLocked = false;
bool lastOctaveLocked = false;
bool octaveLockDirection = LOW;

int octavePixelMultiplier = 0;
int lastOctavePixelMultiplier = 0;

bool buttonState = LOW;
bool lastButtonState = LOW;
unsigned long lastButtonPressTime = 0;
int resetPressDuration = 4000;
int cardOverridePressDuration = 5000;
float patchPitchModifier = 1.0;
int activeThreshold[8] = { 15, 15, 25, 15, 15, 15, 15, 15 };
float octaveMultiplier = 1.0;
bool chordActive[NUMCHORDS] = { false, false, false, false, false, false, false, false };


bool lastChordActive[NUMCHORDS] = { false, false, false, false, false, false, false, false };

int lastNumActive = 1;  // initialize as 1 so that we check NFC on first loop
int numActive = 0;

int lastLooperModeCounter = 0;
int looperModeCounter = 0;

bool first = true;
bool looperRecording = false;
bool looperPlaying = false;
bool looperPassthrough = true;

int pos = 0;
float DSY_SDRAM_BSS buf[MAX_SIZE];
int mod = MAX_SIZE;
int len = 0;
float drywet = 0;
bool res = false;

unsigned long lastButtonLedFlashTime = 0;
int buttonLedFlashInterval = 300;
bool buttonLedState = LOW;

unsigned long lastArpStepTime = 0;

int currentArpScaleStep[NUMCHORDS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int lastArpScaleStep[NUMCHORDS] = { 0, 0, 0, 0, 0, 0, 0, 0 };

uint8_t data[16];

uint8_t lastPatch = 0;
uint8_t currentPatch = 0;

uint8_t keyModifier = 0;
uint8_t lastKeyModifier = 0;

int keySigOctaveModifier = 24;

uint8_t program = 0;

unsigned long lastNFCCheckTime = 0;
int nfcCheckInterval = 1500;
unsigned long lastActiveTime = 0;

float halfStepModifier = 1.0;
float upMappedHalfStepJoystickReadingFloat;
float downMappedHalfStepJoystickReadingFloat;

float arpLevel = 0.0;

int halfStepPixelMultiplier = 0;
int lastHalfStepPixelMultiplier = 0;
bool halfStepPixelReset = false;
float octaveModifier = 1.0;
float upMappedOctaveJoystickReadingFloat;
float downMappedOctaveJoystickReadingFloat;

unsigned long lastResetTime = 0;
int minimumResetInterval = 500;

int pixelMappedZoneReading[NUMZONES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int pixelZoneReading[NUMZONES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int lastPixelMappedZoneReading[NUMZONES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int cvMappedZoneReading[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte cvZoneReading[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };

unsigned long(lastCVSendTime) = 0;

int cvSendInterval = 10;



bool irToggleState = 1;
bool firstPress = true;
bool lightsActive = true;
unsigned long lastHallCheckTime = 0;
int hallCheckInterval = 100;
bool melodyMode = false;
int song = 0;
int lastSequenceStep = -1;
int sequenceStep = 0;
bool lastAaState = false;
bool aaState = false;
unsigned long lastAaSequenceStepTime = 0;
int currentAaSequenceStep = 0;
bool analogReadEnabled = true;
unsigned long lastOffTime = 0;
int nfcWaitInterval = 1000;
bool noteSet = false;
bool newPadPlaced = false;
int pulseCounter = 0;
unsigned long firstPulseTime = 0;
unsigned long lastGateResetTime = 0;
unsigned long lastPixelOnTime[32];
int smoothedZoneReading[NUMZONES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int lastSmoothedZoneReading[NUMZONES] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int patchThresholdModifier = 0;
int pixelZoneReadingDifference[NUMCHORDS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
bool potentialIrMessageIncoming = false;

#define NUMARPRIPPLESTEPS 8
int lastArpRippleModIndex = 0;
int arpRippleModIndex = 0;
// float arpRippleModifier[8] = { 0.75, 0.875, 1.0, 0.875, 0.75, 0.625, 0.5, 0.625 };
// float arpRippleModifier[2][NUMARPRIPPLESTEPS]{
//   { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
//   { 0.5, 0.75, 1.0, 0.75, 0.5, 0.25, 0.0, 0.25 },
// };
//TEMPORARY BYPASS ^^
float arpRippleModifier[2][NUMARPRIPPLESTEPS]{
  { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
  { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
};




//MELODY MODE
// Somewhere Over the Rainbow
// Ode to Joy
// The Way You Look Tonight
// Swan Lake
// You Are My Sunshine

int numSequenceSteps[5] = { 23, 39, 33, 16, 46 };

int melodyNote[5][48]{
  { 0, 7, 6, 4, 5, 6, 7, 0, 5, 4, 5, 3, 2, 0, 1, 2, 3, 1, 6, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 2, 9, 2, 3, 4, 9, 4, 3, 2, 1, 0, 9, 0, 1, 2, 9, 2, 1, 9, 1, 2, 9, 2, 3, 4, 9, 4, 3, 2, 1, 0, 9, 0, 1, 2, 1, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 4, 9, 4, 9, 4, 0, 1, 2, 3, 2, 1, 2, 3, 4, 3, 2, 3, 4, 5, 4, 3, 4, 5, 6, 7, 0, 1, 2, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 5, 1, 2, 3, 4, 5, 3, 5, 3, 5, 1, 3, 1, 5, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 9, 0, 1, 2, 9, 2, 9, 2, 1, 2, 0, 9, 0, 9, 0, 1, 2, 3, 5, 9, 5, 4, 3, 2, 0, 1, 2, 3, 5, 9, 5, 4, 3, 2, 0, 9, 0, 1, 2, 3, 1, 9, 1, 2, 0, 0, 0 },
};


int melodyNoteOctaveModifier[5][48]{
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};


int melodyNoteAccidentalModifier[5][48]{
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

bool tickProcess;
void resetBuffer();
void looperProcess(float &looperOutput, float *looperInput, size_t i);

int aaStepDuration = 110;
int aanNumChords = 2;
int aaChord[64];  // theoretically we could have 64 changes, a chord change every 16th note, but this is incredibly unlikely
int aaChordEventStep[64];

int aaChordNote[4][7]{
  { 36, 38, 40, 41, 43, 45, 47 },
  { 40, 41, 43, 45, 47, 48, 50 },
  { 43, 45, 47, 48, 50, 52, 53 },
  { 47, 48, 50, 52, 53, 55, 57 },
};


int aaVoiceNote[3][64];

bool aaDrumSequenceStep[3][64]{
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 },
};

int aaQuarterSequenceStep[32] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124 };
int aaSixteenthOneSequenceStep[32] = { 1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125 };
int aaEighthSequenceStep[32] = { 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126 };
int aaSixteenthTwoSequenceStep[32] = { 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63, 67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127 };

int halfStepJoystickReading = 0;
int octaveJoystickReading = 0;
int lastOctaveJoystickReading = 0;
bool buttonDisabled = false;
unsigned long lastPulseTime = 0;
float patchMultiplier = 1.0;
byte serialReading[8];
bool externalMode = false;
int nanoRead = 0;
bool waitingForSensorReadings = false;

bool cvMode = false;

int zoneReadingModifier[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };


int octReadSum = 0;
int numOctReads = 0;


byte calibrationByte[21];


int halfStepLow;
int halfStepZero;
int halfStepHigh;

int octaveLow;
int octaveZero;
int octaveHigh;


bool buttonReleased = true;



void setup() {

  delay(25);

  float samplerate;
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  samplerate = DAISY.get_samplerate();
  tick.Init(1.0f, samplerate);
  resetBuffer();


  //MAIN VOICES
  for (int i = 0; i < NUMMAINVOICES; i++) {
    mainVoiceOscillator[i]->Init(samplerate);
    mainVoiceFilter[i]->Init(samplerate);
  }

  for (int i = 0; i < 32; i++) {
    mainVoiceEnvelope[i]->Init(samplerate);
  }

  for (int i = 0; i < NUMCHORDS; i++) {
    mainVoiceFilter[i]->Init(samplerate);
    mainVoiceTremolo[i]->Init(samplerate);
  }



  //ARP VOICES
  for (int i = 0; i < NUMARPVOICES; i++) {
    arpVoiceOscillator[i]->Init(samplerate);
    arpVoiceEnvelope[i]->Init(samplerate);
  }
  arpVoiceFilter.Init(samplerate);



  //AA VOICES
  for (int i = 0; i < 3; i++) {
    aaVoiceOscillator[i]->Init(samplerate);
    aaVoiceEnvelope[i]->Init(samplerate);
  }



  // DRUM VOICES
  kickOscillator[0]->Init(samplerate);
  kickOscillator[0]->SetWaveform(kickOscillator[0]->WAVE_TRI);

  kickOscillator[0]->SetFreq(60.0);

  snareNoise.Init();

  kickEnv.Init(samplerate);
  snareEnv.Init(samplerate);
  hihatEnv.Init(samplerate);

  kickEnv.SetAttackTime(0.025);
  kickEnv.SetDecayTime(0.125);
  kickEnv.SetSustainLevel(0.0);
  kickEnv.SetReleaseTime(0.05);

  snareEnv.SetAttackTime(0.025);
  snareEnv.SetDecayTime(0.125);
  snareEnv.SetSustainLevel(0.0);
  snareEnv.SetReleaseTime(0.1);

  hihatEnv.SetAttackTime(0.025);
  hihatEnv.SetDecayTime(0.05);
  hihatEnv.SetSustainLevel(0.0);
  hihatEnv.SetReleaseTime(0.001);

  aaDrumFilter.Init(samplerate);

  float aaDrumFilterFrequency = 5000.0;

  aaDrumFilter.SetFreq(aaDrumFilterFrequency);

  decimator.Init();

  delay(25);

  Serial.begin(9600);
  hwSerial.begin(9600);


  delay(25);


  while (!strip.begin()) {
    delay(50);
  }
  strip.show();

  strip.setBrightness(200);


  delay(25);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(irRxPin, INPUT_PULLUP);
  pinMode(irTxPin, OUTPUT);
  pinMode(octaveJoystickSelectPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  pinMode(ampMutePin, OUTPUT);
  pinMode(jackInsertSwitchPin, INPUT_PULLUP);

  digitalWrite(relayPin, LOW);
  digitalWrite(irTxPin, LOW);
  digitalWrite(ampMutePin, HIGH);


  currentPatch = 0;
  updatePatch();


  DAISY.SetAudioBlockSize(32);

  DAISY.begin(MyCallback);



  for (int i = 0; i < NUMMAINVOICES; i++) {
    mainVoiceGate[i] = 0;
  }

  for (int i = 0; i < 3; i++) {
    aaVoiceGate[i] = 0;
  }

  for (int i = 0; i < 32; i++) {
    lastPixelOnTime[i] = 0;
  }


  buttonDisabled = false;



  //////////////////////


  delay(150);

  while (!hwSerial.available()) {  // wait for start byte (also signifies version number)
  }

  int espRead = hwSerial.read();

  if (espRead == 240) {   // firmware version
    hwSerial.write(239);  // send Ack
  }

  while (hwSerial.available() < 21) {  // wait for all calibration bytes
  }

  hwSerial.readBytes(calibrationByte, sizeof(calibrationByte));

  halfStepZero = (calibrationByte[0] * 3) + calibrationByte[1];
  halfStepLow = (calibrationByte[2] * 3) + calibrationByte[3];
  halfStepHigh = (calibrationByte[4] * 3) + calibrationByte[5];

  octaveZero = (calibrationByte[6] * 3) + calibrationByte[7];
  octaveLow = (calibrationByte[8] * 3) + calibrationByte[9];
  octaveHigh = (calibrationByte[10] * 3) + calibrationByte[11];


  zoneReadingModifier[0] = calibrationByte[12] - 120;
  zoneReadingModifier[1] = calibrationByte[13] - 120;
  zoneReadingModifier[2] = calibrationByte[14] - 120;
  zoneReadingModifier[3] = calibrationByte[15] - 120;
  zoneReadingModifier[4] = calibrationByte[16] - 120;
  zoneReadingModifier[5] = calibrationByte[17] - 120;
  zoneReadingModifier[6] = calibrationByte[18] - 120;
  zoneReadingModifier[7] = calibrationByte[19] - 120;
  zoneReadingModifier[8] = calibrationByte[20] - 120;


  hwSerial.write(238);  // Ack completition of calibration packet


  //////////////////////

  delay(50);

  digitalWrite(relayPin, HIGH);

  delay(50);

  digitalWrite(ampMutePin, LOW);


  rainbowChaseSlow();

  keyModifier = 9;
}




float aaVoiceOscProcess[3];
float kickVoiceProcess[3];
float sumKickOscillators = 0.0;
float snareNoiseProcess = 0.0;
float hihatNoiseProcess = 0.0;
float aaVoiceFilterProcess[3];
// float snareHighPassProcess = 0.0;
float snareLowPassProcess = 0.0;
float hihatHighPassProcess = 0.0;
// float hihatLowPassProcess = 0.0;
float aaDrumFilterProcess = 0.0;
float mainVoiceEnvProcess[NUMMAINVOICES];
float aaVoiceEnvProcess[3];
float kickEnvProcess = 0.0;
float snareEnvProcess = 0.0;
float hihatEnvProcess = 0.0;
float kickOutput = 0.0;
float snareOutput = 0.0;
float hihatOutput = 0.0;
float mainVoiceOutput[NUMMAINVOICES];
float arpVoiceOutput[NUMARPVOICES];
float aaVoiceOutput[3];
float sumMainVoices = 0.0;
float sumArpVoices = 0.0;
float sumAaVoices = 0.0;
float sumAaDrums = 0.0;
float looperInput = 0.0;
float looperOutput = 0.0;
float finalOutput = 0.0;




void MyCallback(float **in, float **out, size_t size) {



  if (currentPatch == 10) {  // drum patch
    for (int j = 0; j < 8; j++) {
      mainVoiceLevelModifier[j] = 0.5;
    }
  } else {  // not drum patch
    for (int j = 0; j < 8; j++) {
      int dif = zoneReadingForCallback[j] - smoothedZoneReading[j];
      dif = constrain(dif, -2, 2);  // prevent large jumps that causes audible pops
      lastSmoothedZoneReading[j] = smoothedZoneReading[j];
      smoothedZoneReading[j] = lastSmoothedZoneReading[j] + dif;
      smoothedZoneReading[j] = constrain(smoothedZoneReading[j], 0, 800);
      mainVoiceLevelModifier[j] = pow((smoothedZoneReading[j] / 1023.0), 2.0) + 0.05;
    }
  }


  tickProcess = tick.Process();

  if (tickProcess) {

    for (int i = 0; i < 3; i++) {
      if (aaVoiceNote[i][currentAaSequenceStep] == 100) {
        // do nothing
      } else if (aaVoiceNote[i][currentAaSequenceStep] == 99) {
        aaVoiceGate[i] = 0;
      } else {
        aaVoiceOscillator[i]->SetFreq((mtof(aaVoiceNote[i][currentAaSequenceStep] + keyModifier) * 1.0));
        aaVoiceGate[i] = 1;
      }
    }

    kickGate = aaDrumSequenceStep[0][currentAaSequenceStep];
    snareGate = aaDrumSequenceStep[1][currentAaSequenceStep];
    hihatGate = aaDrumSequenceStep[2][currentAaSequenceStep];

    currentAaSequenceStep++;

    if (currentAaSequenceStep >= 64) {
      currentAaSequenceStep = 0;
    }
  }


  for (size_t i = 0; i < size; i++) {

    for (int j = 0; j < NUMCHORDS; j++) {
      mainVoiceEnvProcess[j] = mainVoiceEnvelope[j]->Process(mainVoiceGate[j]);
      mainVoiceOutput[j] = mainVoiceOscillator[j]->Process();
      mainVoiceOutput[j] = mainVoiceFilter[j]->Process(mainVoiceOutput[j]);
      mainVoiceOutput[j] = mainVoiceOutput[j] * mainVoiceEnvProcess[j] * mainVoiceLevelModifier[j];
    }
    sumMainVoices = 0;
    for (int j = 0; j < NUMCHORDS; j++) {
      sumMainVoices = sumMainVoices + mainVoiceOutput[j];
    }

    for (int j = 0; j < 3; j++) {
      arpVoiceOutput[j] = arpVoiceOscillator[j]->Process();
      arpVoiceOutput[j] = arpVoiceOutput[j] * arpVoiceLevelModifier[j];
    }
    sumArpVoices = 0;
    for (int j = 0; j < 3; j++) {
      sumArpVoices = sumArpVoices + arpVoiceOutput[j];
    }
    arpVoiceFilter.Process(sumArpVoices);
    sumArpVoices = arpVoiceFilter.Low();


    looperInput = sumMainVoices + (sumArpVoices * 0.5);

    if (currentPatch == 2) {
      looperInput = decimator.Process(looperInput);
    }

    looperInput = mainVoiceTremolo[0]->Process(looperInput);





    // AUTO-ACCOMPANIST
    if (aaState == true) {

      // AA #nofilter
      for (int j = 0; j < 3; j++) {
        aaVoiceEnvProcess[j] = aaVoiceEnvelope[j]->Process(aaVoiceGate[j]);
        aaVoiceOscProcess[j] = aaVoiceOscillator[j]->Process();
        aaVoiceFilterProcess[j] = aaVoiceOscProcess[j];  // bypass filter
        aaVoiceOutput[j] = aaVoiceFilterProcess[j] * aaVoiceEnvProcess[j];
      }
      sumAaVoices = 0;
      for (int j = 0; j < 3; j++) {
        sumAaVoices = sumAaVoices + aaVoiceOutput[j];
      }
      sumAaVoices = sumAaVoices * 0.02;


      // DRUMS
      sumKickOscillators = kickOscillator[0]->Process();
      kickEnvProcess = kickEnv.Process(kickGate);
      kickOutput = sumKickOscillators * kickEnvProcess;

      snareNoiseProcess = snareNoise.Process();
      snareEnvProcess = snareEnv.Process(snareGate);

      snareLowPassProcess = aaDrumFilter.Process(snareNoiseProcess);
      snareOutput = snareEnvProcess * snareLowPassProcess;


      hihatNoiseProcess = snareNoiseProcess;
      hihatEnvProcess = hihatEnv.Process(hihatGate);

      hihatHighPassProcess = hihatNoiseProcess - snareLowPassProcess;
      hihatOutput = hihatEnvProcess * hihatHighPassProcess;
    }

    sumAaDrums = (kickOutput * 0.3) + (snareOutput * 0.02) + (hihatOutput * 0.01);

    looperProcess(looperOutput, looperInput, i);

    finalOutput = (looperOutput * 1.0) + (sumAaVoices * 1.1) + (sumAaDrums * 1.1);


    out[0][i] = finalOutput * 2.0 * patchMultiplier;
  }
}






void loop() {



  if (digitalRead(jackInsertSwitchPin) == LOW) {
    digitalWrite(ampMutePin, HIGH);
  } else {
    digitalWrite(ampMutePin, LOW);
  }




  if (hwSerial.available() > 0 && waitingForSensorReadings == false) {  // ESP32 has a message for us, and we're not waiting on an external sensor packet

    nanoRead = hwSerial.read();

    if (nanoRead == 201) {  // read request recieved
      analogReadEnabled = false;
      hwSerial.write(1);  // approve
    }


    else if (nanoRead == 203 && externalMode == true) {  // External Mode serial sensor readings (203 is start byte)
      hwSerial.write(2);                                 // acknowledge
      waitingForSensorReadings = true;
    }


    else {  // recieved card data from nano

      lastPatch = currentPatch;
      lastKeyModifier = keyModifier;

      // key card parameter types are stored in different blocks, as defined below
      if (nanoRead >= 64 && nanoRead <= 76) {
        keyModifier = nanoRead - 64;
      } else if (nanoRead >= 77 && nanoRead <= 89) {
        currentPatch = nanoRead - 77;
      } else if (nanoRead >= 90 && nanoRead <= 99) {
        melodyMode = true;
        song = nanoRead - 90;
      } else if (nanoRead == 100) {
        lightsActive = false;
      } else if (nanoRead == 101) {
        generateAutoAccompanyment();
        rainbowChaseFast();
      } else if (nanoRead == 102) {
        lastAaState = aaState;
        rainbowChaseFast();
        aaState = !aaState;
      } else if (nanoRead == 2) {
        buttonDisabled = true;
        rainbowChaseFast();
      } else if (nanoRead == 103) {
        externalMode = !externalMode;
        rainbowChaseFast();
      } else if (nanoRead == 104) {
        cvMode = !cvMode;
        if (cvMode == true) {
          DAISY.StopAudio();
        } else {
          DAISY.StartAudio(MyCallback);
        }
        rainbowChaseFast();
      }

      if (currentPatch != lastPatch) {
        updatePatch();
        rainbowChaseFast();
      }
      if (keyModifier != lastKeyModifier) {
        rainbowChaseFast();
      }
      analogReadEnabled = true;  // enable analogRead
    }
  }


  if (waitingForSensorReadings == true && hwSerial.available() >= 8) {
    hwSerial.readBytes(serialReading, sizeof(serialReading));
    hwSerial.write(3);  // acknowledge completion of external sensor packet
    waitingForSensorReadings = false;
  }





  //ANALOG READS
  if (analogReadEnabled == true) {  // analog reads are disabled while PN532 is scanning, because of noise


    //SEMITONE JOYSTICK
    halfStepJoystickReading = analogRead(A9);

    int upMappedHalfStepJoystickReading = map(halfStepJoystickReading, halfStepZero, halfStepHigh, 1000, 1059);
    upMappedHalfStepJoystickReading = constrain(upMappedHalfStepJoystickReading, 1000, 1059);
    upMappedHalfStepJoystickReadingFloat = upMappedHalfStepJoystickReading / 1000.0;

    int downMappedHalfStepJoystickReading = map(halfStepJoystickReading, halfStepLow, halfStepZero, 944, 1000);
    downMappedHalfStepJoystickReading = constrain(downMappedHalfStepJoystickReading, 944, 1000);
    downMappedHalfStepJoystickReadingFloat = downMappedHalfStepJoystickReading / 1000.0;

    lastHalfStepPixelMultiplier = halfStepPixelMultiplier;
    halfStepPixelMultiplier = map(abs(halfStepJoystickReading - halfStepZero), 0, 200, 0, 30);


    if (halfStepJoystickReading > (halfStepZero + 20)) {
      halfStepModifier = downMappedHalfStepJoystickReadingFloat;
      if (halfStepPixelMultiplier != lastHalfStepPixelMultiplier) {
        int hue = map(downMappedHalfStepJoystickReading, 944, 1000, 0, 255);
        uint32_t color = Wheel(hue);
        strip.setPixelColor(34, color);
        strip.setPixelColor(35, 0, 0, 0);
        if (lightsActive) {
          strip.show();
        }
      }
      halfStepPixelReset = false;
    } else if (halfStepJoystickReading < (halfStepZero - 20)) {
      halfStepModifier = upMappedHalfStepJoystickReadingFloat;
      if (halfStepPixelMultiplier != lastHalfStepPixelMultiplier) {
        int hue = map(upMappedHalfStepJoystickReading, 1000, 1059, 0, 255);
        uint32_t color = Wheel(hue);
        strip.setPixelColor(35, color);
        strip.setPixelColor(34, 0, 0, 0);
        if (lightsActive) {
          strip.show();
        }
      }
      halfStepPixelReset = false;
    } else {
      halfStepModifier = 1.0;
      if (halfStepPixelReset == false) {
        strip.setPixelColor(34, 0, 0, 0);
        strip.setPixelColor(35, 0, 0, 0);
        if (lightsActive) {
          strip.show();
        }
        halfStepPixelReset = true;
      }
    }



    //OCTAVE JOYSTICK
    lastOctaveLocked = octaveLocked;

    lastOctaveJoystickSelectState = octaveJoystickSelectState;
    octaveJoystickSelectState = digitalRead(octaveJoystickSelectPin);



    int singleOctRead = analogRead(A10);
    octReadSum = octReadSum + singleOctRead;
    numOctReads++;

    if (numOctReads >= 2) {
      octaveJoystickReading = octReadSum / 2;
      octReadSum = 0;
      numOctReads = 0;
    }





    int upMappedOctaveJoystickReading = map(octaveJoystickReading, octaveZero, octaveHigh, 1000, 2000);
    upMappedOctaveJoystickReading = constrain(upMappedOctaveJoystickReading, 1000, 2000);
    upMappedOctaveJoystickReadingFloat = upMappedOctaveJoystickReading / 1000.0;

    int downMappedOctaveJoystickReading = map(octaveJoystickReading, octaveLow, octaveZero, 500, 1000);
    downMappedOctaveJoystickReading = constrain(downMappedOctaveJoystickReading, 500, 2000);
    downMappedOctaveJoystickReadingFloat = downMappedOctaveJoystickReading / 1000.0;

    lastOctavePixelMultiplier = octavePixelMultiplier;
    octavePixelMultiplier = map(abs(octaveJoystickReading - octaveZero), 0, 200, 0, 30);

    if (octaveJoystickReading > (octaveZero + 30)) {
      octaveModifier = downMappedOctaveJoystickReadingFloat;
      if (octavePixelMultiplier != lastOctavePixelMultiplier && octaveLocked == false) {
        int hue = map(downMappedOctaveJoystickReading, 500, 1000, 0, 255);
        uint32_t color = Wheel(hue);
        strip.setPixelColor(32, color);
        strip.setPixelColor(33, 0, 0, 0);

        if (lightsActive) {
          strip.show();
        }
      }
      if (lastOctaveJoystickSelectState == HIGH && octaveJoystickSelectState == LOW) {
        delay(5);
        octaveLocked = !octaveLocked;
        octaveLockDirection = LOW;
      }
      if (lastOctaveJoystickSelectState == LOW && octaveJoystickSelectState == HIGH) {
        delay(5);
      }


    } else if (octaveJoystickReading < (octaveZero - 30)) {
      octaveModifier = upMappedOctaveJoystickReadingFloat;
      if (octavePixelMultiplier != lastOctavePixelMultiplier && octaveLocked == false) {
        int hue = map(upMappedOctaveJoystickReading, 1000, 2000, 0, 255);
        uint32_t color = Wheel(hue);
        strip.setPixelColor(33, color);
        strip.setPixelColor(32, 0, 0, 0);
        if (lightsActive) {
          strip.show();
        }
      }
      if (lastOctaveJoystickSelectState == HIGH && octaveJoystickSelectState == LOW) {
        delay(5);
        octaveLocked = !octaveLocked;
        octaveLockDirection = HIGH;
      }
      if (lastOctaveJoystickSelectState == LOW && octaveJoystickSelectState == HIGH) {
        delay(5);
      }


    } else {
      octaveModifier = 1.0;
      if (octavePixelMultiplier != lastOctavePixelMultiplier && octaveLocked == false) {
        strip.setPixelColor(32, 0, 0, 0);
        strip.setPixelColor(33, 0, 0, 0);
        if (lightsActive) {
          strip.show();
        }
      }
      if (lastOctaveJoystickSelectState == HIGH && octaveJoystickSelectState == LOW) {
        delay(5);
        if (octaveLocked == true) {
          octaveLocked = false;
        }
      }
      if (lastOctaveJoystickSelectState == LOW && octaveJoystickSelectState == HIGH) {
        delay(5);
      }
    }

    if (octaveLocked == true) {
      if (octaveLockDirection == HIGH) {
        octaveModifier = 2.0;
        if (lastOctaveLocked == false) {
          strip.setPixelColor(33, 200, 200, 200);
          strip.setPixelColor(32, 0, 0, 0);
          if (lightsActive) {
            strip.show();
          }
        }

      } else if (octaveLockDirection == LOW) {
        octaveModifier = 0.5;
        if (lastOctaveLocked == false) {
          strip.setPixelColor(32, 200, 200, 200);
          strip.setPixelColor(33, 0, 0, 0);
          if (lightsActive) {
            strip.show();
          }
        }
      }
    }




    if (externalMode == false) {
      //  READ FSRs
      for (int i = 0; i < NUMZONES; i++) {
        lastZoneReading[i] = zoneReading[i];
        zoneReading[i] = analogRead(zoneSensorPin[i]) + zoneReadingModifier[i];
        zoneReading[i] = constrain(zoneReading[i], 0, 1024);
        zoneReadingForCallback[i] = zoneReading[i];
        floatZoneReading[i] = zoneReading[i];
      }



      // READ IN EXT SENSOR DATA
    } else if (externalMode == true) {
      for (int i = 0; i < NUMZONES; i++) {
        lastZoneReading[i] = zoneReading[i];
      }
      zoneReading[0] = serialReading[4];
      floatZoneReading[0] = serialReading[4];

      zoneReading[1] = serialReading[5];
      floatZoneReading[1] = serialReading[5];

      zoneReading[2] = serialReading[3];
      floatZoneReading[2] = serialReading[3];


      zoneReading[3] = serialReading[6];
      floatZoneReading[3] = serialReading[6];


      zoneReading[4] = serialReading[2];
      floatZoneReading[4] = serialReading[2];


      zoneReading[5] = serialReading[7];
      floatZoneReading[5] = serialReading[7];


      zoneReading[6] = serialReading[1];
      floatZoneReading[6] = serialReading[1];



      zoneReading[7] = serialReading[0];
      floatZoneReading[7] = serialReading[0];
    }



    if (cvMode == true) {

      for (int i = 0; i < 9; i++) {
        cvMappedZoneReading[i] = map(zoneReading[i], 0, 1024, 1, 255);
        cvMappedZoneReading[i] = constrain(cvMappedZoneReading[i], 1, 255);

        cvZoneReading[i] = cvMappedZoneReading[i];
      }

      int bytesAvailable = hwSerial.availableForWrite();

      if (millis() > lastCVSendTime + cvSendInterval && bytesAvailable > 40) {
        lastCVSendTime = millis();
        hwSerial.write(0);
        // delay(5);
        for (int i = 0; i < 9; i++) {
          hwSerial.write(cvZoneReading[i]);
          // delay(5);
        }
      }
    }




    if (CALIBRATIONMODE == 1) {
      Serial.println("ACC:");
      Serial.println(halfStepJoystickReading);
      Serial.println("");
      Serial.println("");

      Serial.println("OCT:");
      Serial.println(octaveJoystickReading);
      Serial.println("");
      Serial.println("");

      // for (int i = 0; i < NUMZONES; i++) {
      //   Serial.println(i);
      //   Serial.println(zoneReading[i]);
      //   Serial.println("");
      //   Serial.println("");
      // }
      delay(100);
    }



    activeThreshold[0] = 30 + patchThresholdModifier;
    activeThreshold[1] = 30 + patchThresholdModifier;
    activeThreshold[2] = 30 + patchThresholdModifier;
    activeThreshold[3] = 30 + patchThresholdModifier;

    activeThreshold[4] = 30 + patchThresholdModifier;

    activeThreshold[5] = 30 + patchThresholdModifier;
    activeThreshold[6] = 30 + patchThresholdModifier;
    activeThreshold[7] = 30 + patchThresholdModifier;

    lastArpActiveState = arpActiveState;

    if (zoneReading[8] > 30) {
      arpActiveState = true;
    } else {
      arpActiveState = false;
    }


    for (int i = 0; i < NUMCHORDS; i++) {
      lastChordActive[i] = chordActive[i];
    }


    for (int i = 0; i < NUMCHORDS; i++) {
      if (zoneReading[i] > activeThreshold[i]) {
        chordActive[i] = true;
      } else {
        chordActive[i] = false;
      }
    }


    lastNumActive = numActive;
    numActive = 0;
    for (int i = 0; i < NUMCHORDS; i++) {
      numActive = numActive + chordActive[i];
    }

    if (numActive != 0) {
      lastActiveTime = millis();
    }

    if (numActive == 0 && lastNumActive != 0) {
      lastOffTime = millis();
    }
  }





  //MAIN VOICES

  float mainVoiceFilterCutoff[NUMMAINVOICES];

  if (currentPatch == 10) {  //drums
    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilterCutoff[i] = 1800;
    }
  } else {  // not drums
    for (int i = 0; i < NUMCHORDS; i++) {
      // int constrainedCutoff = (zoneReading[i] * 17) - 800;
      float cutoff = (pow((floatZoneReading[i] / 6.0), 2.0)) + 250.0;
      int constrainedCutoff = cutoff;
      constrainedCutoff = constrain(constrainedCutoff, 250, 15000);
      mainVoiceFilterCutoff[i] = constrainedCutoff;
    }
  }


  // patch-specific filter cutoff modifiers (todo: clean this up)
  if (currentPatch == 4) {
    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilterCutoff[i] = mainVoiceFilterCutoff[i] / 2;
    }
  }
  if (currentPatch == 9 || currentPatch == 8) {
    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilterCutoff[i] = mainVoiceFilterCutoff[i] / 2;
    }
  }

  ///////////





  for (int i = 0; i < NUMCHORDS; i++) {
    mainVoiceFilter[i]->SetFreq(mainVoiceFilterCutoff[i]);
    // mainVoiceFilter[i]->SetFreq(400);
  }



  for (int i = 0; i < NUMCHORDS; i++) {
    if (lastChordActive[i] == false && chordActive[i] == true) {
      mainVoiceGate[i] = 1;

    } else if (lastChordActive[i] == true && chordActive[i] == false) {
      mainVoiceGate[i] = 0;
    }
  }





  if (keyModifier >= 7) {  // jump down octave at + above Gmaj
    keySigOctaveModifier = 12;
  } else {
    keySigOctaveModifier = 24;
  }


  for (int i = 0; i < NUMCHORDS; i++) {
    mainVoiceOscillator[i]->SetFreq((mtof(chordNote[i][0] + keyModifier + keySigOctaveModifier)) * halfStepModifier * octaveModifier * patchPitchModifier);
  }




  // Tremolo
  int sumZoneReadings = 0;
  for (int i = 0; i < NUMCHORDS; i++) {
    sumZoneReadings = sumZoneReadings + zoneReading[i];
  }

  int mappedSumZoneReadings = map(sumZoneReadings, 0, 8192, 15, 1500);
  float tremoloRate = mappedSumZoneReadings / 10.0;
  for (int j = 0; j < NUMCHORDS; j++) {
    mainVoiceTremolo[j]->SetFreq(tremoloRate);
  }







  // ARP VOICES
  int arpStepDuration = map(zoneReading[8], 100, 1024, 200, 5);
  arpStepDuration = constrain(arpStepDuration, 5, 200);

  if (millis() > lastArpStepTime + arpStepDuration) {
    lastArpStepTime = millis();

    for (int i = 0; i < NUMCHORDS; i++) {
      lastArpScaleStep[i] = currentArpScaleStep[i];
      currentArpScaleStep[i] = lastArpScaleStep[i] + random(-2, 3);
      if (currentArpScaleStep[i] < 0) {
        currentArpScaleStep[i] = random(1, 2);
      } else if (currentArpScaleStep[i] > 11) {
        currentArpScaleStep[i] = random(8, 11);
      }
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceOscillator[i]->SetFreq((mtof(arpNote[i][currentArpScaleStep[i]] + keyModifier + 24) * 2.0) * halfStepModifier * octaveModifier);
    }

    lastArpRippleModIndex = arpRippleModIndex;
    arpRippleModIndex++;

    if (arpRippleModIndex > 7) {
      arpRippleModIndex = 0;
    }
  }

  float arpVoiceFilterCutoff = zoneReading[8] * 2.5;
  arpVoiceFilter.SetFreq(arpVoiceFilterCutoff);

  if (arpActiveState == true) {
    int arpExponent = 1;
    arpLevel = zoneReading[8] / 1023.0;
    arpLevel = pow(arpLevel, arpExponent);
    arpLevel = arpLevel / 2.5;
  } else {
    arpLevel = 0.0;
  }


  for (int i = 0; i < NUMCHORDS; i++) {
    arpVoiceLevelModifier[i] = arpLevel * 0.5;
  }

  if (currentPatch == 3) {
    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceLevelModifier[i] = arpVoiceLevelModifier[i] / 2.0;
    }
  }

  if (currentPatch == 8) {
    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceLevelModifier[i] = arpVoiceLevelModifier[i] / 6.0;
    }
  }


  if (arpActiveState != lastArpActiveState) {
    for (int i = 0; i < 3; i++) {
      arpVoiceGate[i] = !arpVoiceGate;
    }
  }







  // NEOPIXELS
  if (melodyMode == false) {

    for (int i = 0; i < NUMCHORDS; i++) {
      lastPixelMappedZoneReading[i] = pixelMappedZoneReading[i];
      pixelMappedZoneReading[i] = map(zoneReading[i], 0, 700, 0, 60);
      pixelMappedZoneReading[i] = constrain(pixelMappedZoneReading[i], 0, 59);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      if ((chordActive[i] == true && pixelMappedZoneReading[i] % 2 == 0) || (lastChordActive[i] == false && chordActive[i] == true)) {
        for (int j = 0; j < 4; j++) {
          strip.setPixelColor(orderedNeopixel[(i * 4) + j], (colorBase[i][0] * (pixelMappedZoneReading[i] + 1)) * arpRippleModifier[arpActiveState][(arpRippleModIndex + j) % NUMARPRIPPLESTEPS], (colorBase[i][1] * (pixelMappedZoneReading[i] + 1)) * arpRippleModifier[arpActiveState][(arpRippleModIndex + j) % NUMARPRIPPLESTEPS], (colorBase[i][2] * (pixelMappedZoneReading[i] + 1)) * arpRippleModifier[arpActiveState][(arpRippleModIndex + j) % NUMARPRIPPLESTEPS]);
        }
        if (lightsActive) {
          strip.show();
        }
      } else if (lastChordActive[i] == true && chordActive[i] == false) {
        for (int j = 0; j < 4; j++) {
          strip.setPixelColor(orderedNeopixel[(i * 4) + j], 0, 0, 0);
        }
        if (lightsActive) {
          strip.show();
        }
      }
    }
  }







  // MELODY MODE
  if (melodyMode == true) {

    int highestZoneReading = 0;

    int strongestZone = 0;

    for (int i = 0; i < NUMZONES; i++) {
      if (zoneReading[i] > highestZoneReading) {
        highestZoneReading = zoneReading[i];
        strongestZone = i;
      }
    }

    if (noteSet == false) {
      if (melodyNote[song][sequenceStep] == 9) {
        for (int i = 0; i < 36; i++) {
          strip.setPixelColor(i, 0, 0, 0);
        }
      } else {
        for (int i = 0; i < 4; i++) {
          strip.setPixelColor(orderedNeopixel[(melodyNote[song][sequenceStep] * 4) + i], colorBase[melodyNote[song][sequenceStep]][0] * 25, colorBase[melodyNote[song][sequenceStep]][1] * 25, colorBase[melodyNote[song][sequenceStep]][2] * 25);
        }
      }

      if (melodyNoteOctaveModifier[song][sequenceStep] == 2) {
        strip.setPixelColor(33, 210, 210, 210);
      } else {
        strip.setPixelColor(33, 0, 0, 0);
      }

      if (melodyNoteOctaveModifier[song][sequenceStep] == 1) {
        strip.setPixelColor(32, 210, 210, 210);
      } else {
        strip.setPixelColor(32, 0, 0, 0);
      }

      if (melodyNoteAccidentalModifier[song][sequenceStep] == 2) {
        strip.setPixelColor(35, 210, 210, 210);
      } else {
        strip.setPixelColor(35, 0, 0, 0);
      }

      if (melodyNoteAccidentalModifier[song][sequenceStep] == 1) {
        strip.setPixelColor(34, 210, 210, 210);
      } else {
        strip.setPixelColor(34, 0, 0, 0);
      }
      strip.show();
      noteSet = true;
    }


    if (melodyNote[song][sequenceStep] == 9) {
      if (numActive == 0) {
        for (int i = 0; i < 36; i++) {
          strip.setPixelColor(i, 0, 0, 0);
        }
        sequenceStep++;
        noteSet = false;
      }
    } else if (chordActive[melodyNote[song][sequenceStep]] == true && strongestZone == melodyNote[song][sequenceStep]) {
      for (int i = 0; i < 36; i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
      sequenceStep++;
      noteSet = false;
    }


    if (sequenceStep >= numSequenceSteps[song]) {
      sequenceStep = 0;
      melodyMode = false;
    }
  }






  // AUTO ACCOMPANIST VOICE SHUTOFF
  if (lastAaState == true && aaState == false) {
    for (int i = 0; i < 3; i++) {
      aaVoiceGate[i] = 0;
    }
  }






  // LOOPER
  if (aaState == false) {

    lastButtonState = buttonState;
    buttonState = digitalRead(buttonPin);
    lastLooperModeCounter = looperModeCounter;

    if (buttonState == LOW) {
      if (lastButtonState == HIGH && buttonDisabled == false) {
        lastButtonPressTime = millis();
        delay(8);
        looperModeCounter++;
        if (looperModeCounter == 2) {
          first = false;
          mod = len;
          len = 0;
          res = true;
        }
        if (looperModeCounter == 1 && firstPress == false) {
          resetBuffer();
          res = false;
        }
        if (looperModeCounter > 2) {
          looperModeCounter = 0;
          firstPress = false;
          digitalWrite(irTxPin, LOW);
        }
      }

      if (millis() > lastButtonPressTime + resetPressDuration && halfStepJoystickReading < halfStepHigh && octaveJoystickReading > octaveLow) {
        resetToBootLoader();
      }


      if (millis() > lastButtonPressTime + cardOverridePressDuration && buttonReleased == true) {  // force card read
        hwSerial.write(253);
        buttonReleased = false;
        looperModeCounter = 0;
        for (int i = 0; i < 4; i++) {
          strip.setPixelColor(orderedNeopixel[i], colorBase[0][0] * 25, colorBase[0][1] * 25, colorBase[0][2] * 25);
        }
        strip.show();
      }




    } else if (lastButtonState == LOW && buttonState == HIGH) {
      buttonReleased = true;
      delay(8);
    }

    if (looperModeCounter == 0) {
      looperPassthrough = true;
      looperRecording = false;
      looperPlaying = false;
    } else if (looperModeCounter == 1) {
      if (numActive >= 1) {  // WAIT FOR FIRST NOTE TO BEGIN RECORDING
        looperPassthrough = false;
        looperRecording = true;
        looperPlaying = false;
      }
    } else if (looperModeCounter == 2) {
      looperPassthrough = false;
      looperRecording = false;
      looperPlaying = true;
    }
  }





  // IR WRITE
  static int pulseInterval = 5;
  if (pos < 12000 && looperPlaying == true) {
    if (millis() > lastPulseTime + pulseInterval) {
      lastPulseTime = millis();
      irTxState = !irTxState;
      digitalWrite(irTxPin, irTxState);
    }
  }




  // IR READ
  static int irRxMessageThreshold = 5;
  static int irRxTimeoutDuration = 300;

  lastIrRxState = irRxState;
  irRxState = digitalRead(irRxPin);

  if (irRxState != lastIrRxState) {
    pulseCounter++;
  }

  if (pulseCounter == 2) {  // beginning of what is likely a message, or we've accumulated enough erroneous pulses that it's time to begin a timeout clock to reset pulse counter
    firstPulseTime = millis();
    potentialIrMessageIncoming = true;
  }

  if (pulseCounter > irRxMessageThreshold && potentialIrMessageIncoming == true && millis() > lastResetTime + minimumResetInterval) {
    lastResetTime = millis();
    pos = 0;  // reset looper playback
    pulseCounter = 0;
    potentialIrMessageIncoming = false;
  }

  if (millis() > firstPulseTime + irRxTimeoutDuration && potentialIrMessageIncoming == true) {
    pulseCounter = 0;
    potentialIrMessageIncoming = false;
  }






  // BUTTON LED
  if (lightsActive) {
    if (looperModeCounter == 0) {
      digitalWrite(ledPin, LOW);
    } else if (looperModeCounter == 1) {
      if (millis() > lastButtonLedFlashTime + buttonLedFlashInterval) {
        lastButtonLedFlashTime = millis();
        if (buttonLedState == HIGH) {
          digitalWrite(ledPin, LOW);
          buttonLedState = LOW;
        } else {
          digitalWrite(ledPin, HIGH);
          buttonLedState = HIGH;
        }
      }
    } else if (looperModeCounter == 2) {
      digitalWrite(ledPin, HIGH);
    }
  }
}
// end of main loop






void resetBuffer() {
  first = true;
  pos = 0;
  len = 0;
  for (int i = 0; i < mod; i++) {
    buf[i] = 0;
  }
  mod = MAX_SIZE;
}



void writeBuffer(float looperInput, size_t i) {
  buf[pos] = buf[pos] * 0.5 + looperInput * 0.5;
  if (first) {
    len++;
  }
}



void looperProcess(float &looperOutput, float looperInput, size_t i) {
  if (looperRecording) {
    writeBuffer(looperInput, i);
    looperOutput = looperInput;
  }

  // automatic looptime
  if (len >= MAX_SIZE) {
    first = false;
    mod = MAX_SIZE;
    len = 0;
  }

  if (!looperPassthrough) {
    pos++;
    pos %= mod;
  }


  if (looperPlaying) {
    if (pos < 20) {
      float fadeFactor = pos / 20.0;
      looperOutput = (buf[pos] * fadeFactor) + looperInput;
    } else if ((mod - pos) < 20) {
      float fadeFactor = (mod - pos) / 20.0;
      looperOutput = (buf[pos] * fadeFactor) + looperInput;
    } else {
      looperOutput = buf[pos] + looperInput;
    }
  }


  if (looperPassthrough) {
    looperOutput = looperInput;
  }
}







void updatePatch() {

  //DEFAULT
  if (currentPatch == 0) {
    //MAIN VOICES
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_POLYBLEP_SAW);
    }


    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.005);
      mainVoiceEnvelope[i]->SetReleaseTime(0.025);
      mainVoiceEnvelope[i]->SetSustainLevel(1.0);
    }


    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.25);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(1.0);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(1500);
    }

    decimator.SetDownsampleFactor(0.0);
    mainVoiceTremolo[0]->SetDepth(0.0);

    patchMultiplier = 1.0;
    patchPitchModifier = 1.0;
    patchThresholdModifier = 0;
  }





  // SMOOTH
  if (currentPatch == 1) {
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_SIN);
    }

    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.005);
      mainVoiceEnvelope[i]->SetReleaseTime(0.025);
      mainVoiceEnvelope[i]->SetSustainLevel(1.0);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.1);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(1.0);
    }
    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(1500);
    }
    decimator.SetDownsampleFactor(0.0);
    mainVoiceTremolo[0]->SetDepth(0.0);

    patchMultiplier = 1.0;
    patchPitchModifier = 1.0;
    patchThresholdModifier = 0;
  }




  // WAVY
  if (currentPatch == 3) {
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);
    }

    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.005);
      mainVoiceEnvelope[i]->SetReleaseTime(0.025);
      mainVoiceEnvelope[i]->SetSustainLevel(1.0);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.1);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_SIN);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(1.0);
    }
    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(500);
    }
    decimator.SetDownsampleFactor(0.0);
    mainVoiceTremolo[0]->SetDepth(1.0);

    patchMultiplier = 1.0;
    patchPitchModifier = 1.0;
    patchThresholdModifier = 0;
  }



  //ROUGH
  if (currentPatch == 2) {
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_SAW);
    }

    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.005);
      mainVoiceEnvelope[i]->SetReleaseTime(0.025);
      mainVoiceEnvelope[i]->SetSustainLevel(1.0);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.1);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(1.0);
    }
    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(1500);
    }
    decimator.SetDownsampleFactor(0.5);
    decimator.SetBitcrushFactor(0.6);
    decimator.SetBitsToCrush(6);

    mainVoiceTremolo[0]->SetDepth(0.0);

    patchMultiplier = 1.0;
    patchPitchModifier = 1.0;
    patchThresholdModifier = 0;
  }



  if (currentPatch == 4) {
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_SAW);
    }

    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.005);
      mainVoiceEnvelope[i]->SetReleaseTime(1.000);
      mainVoiceEnvelope[i]->SetSustainLevel(1.0);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.1);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(1.0);
    }
    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(1500);
    }
    decimator.SetDownsampleFactor(0.0);
    mainVoiceTremolo[0]->SetDepth(0.0);

    patchMultiplier = 1.0;
    patchPitchModifier = 1.0;
    patchThresholdModifier = 0;
  }





  // PLAYDOUGH PATCH
  if (currentPatch == 8) {
    //MAIN VOICES
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);
    }


    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.01);
      mainVoiceEnvelope[i]->SetDecayTime(0.2);
      mainVoiceEnvelope[i]->SetReleaseTime(0.5);
      mainVoiceEnvelope[i]->SetSustainLevel(0.1);
    }


    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.5);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(0.2);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(900);
    }

    decimator.SetDownsampleFactor(0.0);
    mainVoiceTremolo[0]->SetDepth(0.0);

    patchMultiplier = 6.0;
    patchPitchModifier = 0.5;
    patchThresholdModifier = 40;
  }





  // POM POM PATCH
  if (currentPatch == 9) {
    //MAIN VOICES
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_SQUARE);
    }


    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.005);
      mainVoiceEnvelope[i]->SetReleaseTime(0.5);
      mainVoiceEnvelope[i]->SetSustainLevel(1.0);
    }


    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.7);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_POLYBLEP_SQUARE);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(1.0);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(1500);
    }

    decimator.SetDownsampleFactor(0.0);
    mainVoiceTremolo[0]->SetDepth(0.0);

    patchMultiplier = 1.0;
    patchPitchModifier = 1.0;
    patchThresholdModifier = 0;
  }






  // DRUM PATCH
  if (currentPatch == 10) {
    //MAIN VOICES
    for (int i = 0; i < NUMMAINVOICES; i++) {
      mainVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_POLYBLEP_SAW);
    }


    for (int i = 0; i < 33; i++) {
      mainVoiceEnvelope[i]->SetAttackTime(0.005);
      mainVoiceEnvelope[i]->SetReleaseTime(0.25);
      mainVoiceEnvelope[i]->SetSustainLevel(1.0);
    }


    for (int i = 0; i < NUMCHORDS; i++) {
      mainVoiceFilter[i]->SetRes(0.25);
      mainVoiceFilter[i]->SetFreq(1500);
    }



    //ARP VOICES
    for (int i = 0; i < NUMARPVOICES; i++) {
      arpVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);

      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
      arpVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
      arpVoiceEnvelope[i]->SetSustainLevel(1.0);
    }

    for (int i = 0; i < NUMCHORDS; i++) {
      arpVoiceFilter.SetRes(0.1);
      arpVoiceFilter.SetFreq(1500);
    }

    decimator.SetDownsampleFactor(0.0);
    mainVoiceTremolo[0]->SetDepth(0.0);

    patchMultiplier = 0.8;
    patchPitchModifier = 1.0;
    patchThresholdModifier = 40;
  }





  //AA VOICES
  for (int i = 0; i < 3; i++) {
    aaVoiceOscillator[i]->SetWaveform(mainVoiceOscillator[i]->WAVE_TRI);

    aaVoiceEnvelope[i]->SetTime(ADSR_SEG_ATTACK, .05);
    aaVoiceEnvelope[i]->SetTime(ADSR_SEG_DECAY, .1);
    aaVoiceEnvelope[i]->SetTime(ADSR_SEG_RELEASE, .1);
    aaVoiceEnvelope[i]->SetSustainLevel(1.0);
  }
}





void resetToBootLoader() {
  dsy_gpio_pin bootpin = { DSY_GPIOG, 3 };
  dsy_gpio pin;
  pin.mode = DSY_GPIO_MODE_OUTPUT_PP;
  pin.pin = bootpin;
  dsy_gpio_init(&pin);
  dsy_gpio_write(&pin, 1);
  delay(10);
  HAL_NVIC_SystemReset();
}





void rainbowChaseFast() {

  int nextPixelInterval = 7;
  int holdOnDuration = 150;
  bool pixelOn[32];
  bool allClear = false;

  for (int i = 0; i < 32; i++) {
    pixelOn[i] = false;
  }

  for (int i = 0; i < 32; i++) {
    strip.setPixelColor(orderedNeopixel[i], colorBase[i / 4][0] * 2, colorBase[i / 4][1] * 2, colorBase[i / 4][2] * 2);
    lastPixelOnTime[i] = millis();
    pixelOn[i] = true;
    for (int j = 0; j < 32; j++) {
      if (millis() > lastPixelOnTime[j] + holdOnDuration && pixelOn[j] == true) {
        strip.setPixelColor(orderedNeopixel[j], 0, 0, 0);
        pixelOn[j] = false;
      }
    }
    delay(nextPixelInterval);
    strip.show();
  }

  while (!allClear) {
    allClear = true;
    for (int j = 0; j < 32; j++) {
      if (pixelOn[j] == true) {
        allClear = false;
        if (millis() > lastPixelOnTime[j] + holdOnDuration) {
          strip.setPixelColor(orderedNeopixel[j], 0, 0, 0);
          strip.show();
          pixelOn[j] = false;
        }
      }
    }
  }
}




void rainbowChaseSlow() {

  int nextPixelInterval = 28;
  int holdOnDuration = 375;
  bool pixelOn[32];
  bool allClear = false;

  for (int i = 0; i < 32; i++) {
    pixelOn[i] = false;
  }


  for (int i = 0; i < 32; i++) {
    strip.setPixelColor(orderedNeopixel[i], colorBase[i / 4][0] * 3, colorBase[i / 4][1] * 3, colorBase[i / 4][2] * 3);
    lastPixelOnTime[i] = millis();
    pixelOn[i] = true;
    for (int j = 0; j < 32; j++) {
      if (millis() > lastPixelOnTime[j] + holdOnDuration && pixelOn[j] == true) {
        strip.setPixelColor(orderedNeopixel[j], 0, 0, 0);
        pixelOn[j] = false;
      }
    }
    delay(nextPixelInterval);
    strip.show();
  }

  while (!allClear) {
    allClear = true;
    for (int j = 0; j < 32; j++) {
      if (pixelOn[j] == true) {
        allClear = false;
        if (millis() > lastPixelOnTime[j] + holdOnDuration) {
          strip.setPixelColor(orderedNeopixel[j], 0, 0, 0);
          strip.show();
          pixelOn[j] = false;
        }
      }
    }
  }
}





void generateAutoAccompanyment() {


  // // initialize random number generator
  int randomSeedValue = millis() % 112798;
  randomSeed(randomSeedValue);


  int tempoVariation = random(-175, 175);

  float tickFrequency = 350.0 + tempoVariation;

  tick.SetFreq(tickFrequency);


  // generate when the chords change, and how many chords there are
  aaChordEventStep[0] = 0;  // first chord always happens on beat 1
  int aaNumChords = 0;
  int previousEventStep = 0;
  int eventStep = 0;
  int maxNumBeats = 8;  // next chord must occur within this number of steps

  for (int i = 1; eventStep < 64; i++) {  // i = 1: start with 2nd chord
    previousEventStep = eventStep;

    int randomNumber = random(0, 100);

    if (randomNumber < 0) {
      aaChordEventStep[i] = aaSixteenthOneSequenceStep[random((previousEventStep / 4) + 1, (previousEventStep / 4) + maxNumBeats)];
      eventStep = aaChordEventStep[i];
      aaNumChords++;

    } else if (randomNumber >= 0 && randomNumber < 0) {
      aaChordEventStep[i] = aaSixteenthTwoSequenceStep[random((previousEventStep / 4) + 1, (previousEventStep / 4) + maxNumBeats)];
      eventStep = aaChordEventStep[i];
      aaNumChords++;
    }

    else if (randomNumber >= 15 && randomNumber < 25) {
      aaChordEventStep[i] = aaEighthSequenceStep[random((previousEventStep / 4) + 1, (previousEventStep / 4) + maxNumBeats)];
      eventStep = aaChordEventStep[i];
      aaNumChords++;
    }

    else {
      aaChordEventStep[i] = aaQuarterSequenceStep[random((previousEventStep / 4) + 1, (previousEventStep / 4) + maxNumBeats)];
      eventStep = aaChordEventStep[i];
      aaNumChords++;
    }
  }

  // generate the chord progression
  for (int i = 0; i < aaNumChords; i++) {
    aaChord[i] = random(0, 7);
  }

  // generate note hold durations
  int noteHoldSteps[3][aaNumChords];
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < aaNumChords; j++) {
      noteHoldSteps[i][j] = random(1, aaChordEventStep[j + 1] - aaChordEventStep[j]);
    }
  }

  // generate bass voice sequence
  int currentChordNumber = 0;
  for (int i = 0; i < 64; i++) {
    if (i == aaChordEventStep[currentChordNumber]) {
      aaVoiceNote[0][i] = aaChordNote[0][aaChord[currentChordNumber]];
      currentChordNumber++;
    } else if (i == aaChordEventStep[currentChordNumber] + noteHoldSteps[0][currentChordNumber]) {
      aaVoiceNote[0][i] = 99;
    } else {
      aaVoiceNote[0][i] = 100;
    }
  }

  // generate next voice sequence
  currentChordNumber = 0;
  for (int i = 0; i < 64; i++) {
    if (i == aaChordEventStep[currentChordNumber]) {
      aaVoiceNote[1][i] = aaChordNote[1][aaChord[currentChordNumber]];
      currentChordNumber++;
    } else if (i == aaChordEventStep[currentChordNumber] + noteHoldSteps[0][currentChordNumber]) {
      aaVoiceNote[1][i] = 99;
    } else {
      aaVoiceNote[1][i] = 100;
    }
  }


  // generate next voice sequence
  currentChordNumber = 0;
  for (int i = 0; i < 64; i++) {
    if (i == aaChordEventStep[currentChordNumber]) {  // note on
      aaVoiceNote[2][i] = aaChordNote[2][aaChord[currentChordNumber]];
      currentChordNumber++;
    } else if (i == aaChordEventStep[currentChordNumber] + noteHoldSteps[0][currentChordNumber]) {  // note off
      aaVoiceNote[2][i] = 99;
    } else {  // no event
      aaVoiceNote[2][i] = 100;
    }
  }


  for (int i = 0; i < 64; i++) {
    int rand = random(0, 4);
    if (i % 2 == 1) {
      aaDrumSequenceStep[2][i] = 0;
    } else if (rand == 0) {
      aaDrumSequenceStep[2][i] = 0;
    } else {
      aaDrumSequenceStep[2][i] = 1;
    }
  }


  aaState = true;
  currentAaSequenceStep = 0;
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
