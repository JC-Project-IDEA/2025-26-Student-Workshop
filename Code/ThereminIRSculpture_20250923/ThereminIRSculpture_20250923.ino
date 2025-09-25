// External lbiraries required, make sure the library is downloaded before compiling the code
#include <SharpIR.h>
#include <MIDIUSB.h>
#include <CapacitiveSensor.h>

long TimeSerial = 0;  //Serial update time

//Pulse capture variables
volatile uint16_t lastCapture = 0;
//volatile means the variable will be updated at anytime, no interups
//uint16_t means 16bit binary for counter usage
volatile uint16_t pulseWidth = 0;
volatile bool newPulse = false;
long pulseWidth_cal = 0;

//Hardware input capture function for detecting a signal edge from TLC555 > 7493
ISR(TIMER1_CAPT_vect) {
  uint16_t capture = ICR1;             // Read the captured timer value
  pulseWidth = capture - lastCapture;  // Calculate pulse width
  lastCapture = capture;               // Update last capture time
  newPulse = true;                     // Set flag to process pulse width in loop
}

//Variables for Theremin sense auto-calibration
long TimeNow = 0;
long Timetrigger = 0;
long Offset = 0;
int OffsetFlag = 0;
// Auto-calibration function
void setOffset() {
  Offset = pulseWidth + 10;  //<<<<<<<<<<<<< Auto
  //Offset = 3663;  //<<<<<<<<<<<<< Manual
  OffsetFlag = 1;
}

//Theremin sense variables
float CapSenseValue = 0;
float smoothedCapSense = 0;
float pulseScale = 2;

//IR sensor variables
#define IRPin A0
SharpIR IRSensor(SharpIR::GP2Y0A21YK0F, IRPin);
float Distance_cm;
float readDistance_cm;
float smoothedDistance = 0;
float maxDistance = 40;
float minDistance = 10;

//For all sensors
float smoothingFactor = 0.1;  //<<<<<<<<<<<<< Sensor value smoothness

//MIDI message - Theremin
int TH_midiChannel = 0;  //midi channel

int TH_midiCC = 1;  //midi cc
int TH_midiCCValue = 0;
int prevTH_midiCCValue = 0;

int TH_midiNOTE = 36;  //midi note
int TH_midiNOTEOn = 0;
long TH_midiNOTE_buff = 0;
long TH_midiNOTE_buffTime = 200;

//MIDI message - IR Sensor
int IR_midiChannel = 1;  //midi channel

int IR_midiCC = 2;  //midi cc
int IR_midiCCValue = 0;
int prevIR_midiCCValue = 0;

int IR_midiNOTE = 36;  //midi note
int IR_midiNOTEOn = 0;
long IR_midiNOTE_buff = 0;
long IR_midiNOTE_buffTime = 200;

//MIDI messages function setup
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = { 0x08, 0x80 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
}

void setup() {
  //Setup for Serial port
  Serial.begin(115200);

  //----------------------------------------------- Technical setup, for copy and paste
  //Setup for MCU clocking and capture function
  TCCR1A = 0;              // Normal mode (no PWM)
  TCCR1B = (1 << ICES1) |  // Capture on rising edge
           (1 << CS10);    // Prescaler = 1 (16 MHz timer clock with 16 MHz CPU)
  TIMSK1 = (1 << ICIE1);   // Enable input capture interrupt
  TIFR1 = (1 << ICF1);     // Clear input capture flag
  sei();                   // Enable global interrupts

  //Setup for Theremin sensor inputs
  pinMode(4, INPUT);  // Theremin input (Arduino Pro Micro =  D4)
}

void loop() {
  //Autocalibration for Theremin
  TimeNow = millis();
  if (TimeNow > 5000 && OffsetFlag == 0) {
    setOffset();
  }

  //---------------------------------------------- Theremin reading
  if (newPulse) {
    newPulse = false;  //turn off flag
    pulseWidth_cal = (pulseWidth - Offset);
    smoothedCapSense = (pulseWidth_cal * smoothingFactor) + (smoothedCapSense * (1 - smoothingFactor));
    CapSenseValue = smoothedCapSense;

    //---------------------------------------------- Theremin MIDI output
    TH_midiCCValue = CapSenseValue * pulseScale;
    if (TH_midiCCValue < 0) {
      TH_midiCCValue = 0;
    }
    if (TH_midiCCValue > 127) {
      TH_midiCCValue = 127;
    }
  }
  if (prevTH_midiCCValue != TH_midiCCValue && pulseWidth_cal <= Offset) {
    controlChange(TH_midiChannel, TH_midiCC, TH_midiCCValue);
    MidiUSB.flush();

    if (TH_midiNOTEOn == 0) {
      TH_midiNOTEOn = 1;
      TH_midiNOTE_buff = TimeNow;
    }

    if (TH_midiNOTEOn == 1 && TimeNow - TH_midiNOTE_buff > TH_midiNOTE_buffTime) {
      noteOn(TH_midiChannel, TH_midiNOTE, 127);
      MidiUSB.flush();
      TH_midiNOTEOn = 2;
    }

    prevTH_midiCCValue = TH_midiCCValue;
  }

  if (TH_midiNOTEOn == 2 && TH_midiCCValue <= 0) {
    noteOff(TH_midiChannel, TH_midiNOTE, 0);
    MidiUSB.flush();
    TH_midiNOTEOn = 0;
  }

  //---------------------------------------------- IRSensor reading›
  readDistance_cm = IRSensor.getDistance();                              //Read IR Sensor raw data
  if (readDistance_cm < maxDistance || readDistance_cm > minDistance) {  //Update IR Sensor raw data only if the reading is in the range of maximum and minimum distance
    Distance_cm = readDistance_cm;
  }
  smoothedDistance = (Distance_cm * smoothingFactor) + (smoothedDistance * (1 - smoothingFactor));  //Smooth out raw data according to smoothingFactor
  Distance_cm = smoothedDistance;
  //Set detective range
  if (Distance_cm > maxDistance) {
    Distance_cm = maxDistance;
  }
  if (Distance_cm < minDistance) {
    Distance_cm = minDistance;
  }

  //---------------------------------------------- IRSensor MIDI output
  IR_midiCCValue = (127 - (Distance_cm - minDistance) * (127 / (maxDistance - minDistance)));
  if (prevIR_midiCCValue != IR_midiCCValue && readDistance_cm <= maxDistance) {
    controlChange(IR_midiChannel, IR_midiCC, IR_midiCCValue);
    MidiUSB.flush();

    if (IR_midiNOTEOn == 0) {
      IR_midiNOTEOn = 1;
      IR_midiNOTE_buff = TimeNow;
    }

    if (IR_midiNOTEOn == 1 && TimeNow - IR_midiNOTE_buff > IR_midiNOTE_buffTime) {
      noteOn(IR_midiChannel, IR_midiNOTE, 127);
      MidiUSB.flush();
      IR_midiNOTEOn = 2;
    }

    prevIR_midiCCValue = IR_midiCCValue;
  }

  if (IR_midiNOTEOn == 2 && IR_midiCCValue <= 0) {
    noteOff(IR_midiChannel, IR_midiNOTE, 0);
    MidiUSB.flush();
    IR_midiNOTEOn = 0;
  }

  //---------------------------------------------- Serial Print for debug
  if (TimeNow - TimeSerial >= 100) {
    Serial.print(millis());  // check on performance in milliseconds
    Serial.print("\t");      // tab character for debug windown spacing
    Serial.print("Theremin Pulse: ");
    Serial.print(pulseWidth);  // print theremin sensor raw output
    Serial.print("\t");
    Serial.print("Offset: ");
    Serial.print(Offset);  // print theremin sensor offset
    Serial.print("\t");
    Serial.print("Calibrated: ");
    Serial.print(pulseWidth_cal);  // print theremin sensor output after offset
    Serial.print("\t");
    Serial.print("TH_MIDI: ");
    Serial.print(TH_midiCCValue);  // print theremin sensor MIDICC output
    Serial.print("\t");
    Serial.print("IR: ");
    Serial.print(Distance_cm);  // print IR sensor output
    Serial.print("\t");
    Serial.print("IR_MIDI: ");
    Serial.print(IR_midiCCValue);  // print IR sensor MIDICC output
    Serial.println("\t");
    TimeSerial = TimeNow;
  }
}