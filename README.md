<h1 align="center">🎵 共融藝術 ✕ 科技 學生工作坊 🎵</h1>
<p align="center"> 2025/26: Student Workshop </p>
<p align="center">👩‍🏫 Yan &emsp;&emsp;👨‍🏫 Sky&emsp;&emsp;👩‍🏫 Nix</p>


## 🎨 工作坊內容簡介：
賽馬會科藝共融計劃（Jockey Club Project IDEA）由香港賽馬會慈善信託基金捐助、香港城市大學主辦，旨在提升本地中學教師及學生對藝術科技的認識和能力，透過工作坊讓他們掌握如何應用數碼科技進行藝術創作，為傳統創作方式注入新元素，創造更多別樹一格的表現方式。同時，本計劃將透過分享和體驗活動提升學生對共融藝術的意識，啟發他們反思如何讓殘疾人士欣賞藝術及參與藝術活動，透過藝術科技建立共融的社會。 

## 🎵 Movemin ｜結合感測與聲音合成，自製演奏音樂的控制器
【工作坊介紹】
Movemin 靈感來自經典電子樂器 特雷門琴（Theremin），是一個憑演奏者身體與天線之間距離的變化，來控制聲音頻率的聲音裝置。同學將製作屬於自己的電子樂器 Movemin，探索「Musical expression for all」的可能性：重新思考演奏樂器的操作方式，嘗試透過不同的肢體動作或工具與裝置互動，並搭配電腦預設的音樂程式，創造出多元且獨特的聲音表現，讓更多人能參與音樂與互動藝術的體驗。

【學習重點】
- ⁠通達設計：運用五金材料，動手製作動態雕塑或觸控鍵盤，打造能讓不同能力人士共同參與的樂器介面。
- ⁠從電路到聲音：學習基礎電子電路與機械原理，並透過程式編碼（如 Arduino），掌握將物理互動轉化為聲音訊號的技術。
- 聲音設計入門：使用虛擬模組合成器 VCV Rack，透過串連(Patching) 各種訊號模組，創作屬於自己的獨特音色與效果。

【成果展示】
學員於工作坊中完成的優秀作品，將有機會獲選於年度展覽中公開展出。

## 📖 學習資料下載 :
- 第一堂 [Lesson 1: Introduction](https://docs.google.com/presentation/d/1WSr9kcV1-ZjsRH_j-11dmlGryh9tGZXPMCw8sXCov08/edit?usp=sharing)
- 第二堂 [Lesson 2: Assembly & Arduino](https://docs.google.com/presentation/d/19V55PmKnhZj6ZIIb1CYu6AyAa-iCVDYn30vg3Tqv0Sw/edit?usp=sharing)
- 第三堂 [Lesson 3: Working With VCV Rack 2](https://docs.google.com/presentation/d/10nIk9Jp0Kmlm_R8HbipU5P-_flwunWAbKxvU4t2fisc/edit?usp=sharing)
- 組裝指南 [Movemin Assembly Guide](https://github.com/JC-Project-IDEA/2025-26-Student-Workshop/blob/28620ab0dedbc34ca16eff0b042d239f05b2dca1/Slides/Assembly%20Guide%20for%20Workshops.pdf)  

## 🙋🏻🙋🏻‍♂️🙋🏻‍♀️ 報名參展：
2025/26 年度展覽將於 2026 年 1 月 30 日至 2 月 9 日假香港文化中心舉行，學生可點擊以下連結報名參展：
- https://tinyurl.com/IDEA-exhi202526

## Useful Links:
1. VCV Rack 2: https://vcvrack.com/
2. VCV free module manuals: https://vcvrack.com/Free#VCO2
3. Arduino IDE: https://www.arduino.cc/en/software/
4. Midi Monitor:  https://www.snoize.com/MIDIMonitor/
5. Midi OX:  www.midiox.com

## Theremin IR Sculpture (Updated: 2025/09/23)
```sh
// External lbiraries required, make sure the library is downloaded before compiling the code
#include <SharpIR.h>
#include <MIDIUSB.h>

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
```
## Theremin IR Touch (Updated: 2025/09/23)
```sh
// External lbiraries required, make sure the library is downloaded before compiling the code
#include <SharpIR.h>
#include <MIDIUSB.h>
#include <CapacitiveSensor.h>

long TimeSerial = 0;  //Serial update time

//Pulse capture variables
volatile uint16_t lastCapture = 0;
//volatile means the variable will be updated at anytime, no interups
//uint16_t means 16bit binary for counter usage`
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

//Touch Sensor variables
CapacitiveSensor* touchplates[4];
long touch[4] = { 0, 0, 0, 0 };
bool NoteOn[4] = { 0, 0, 0, 0 };
long velocity[4] = { 0, 0, 0, 0 };
long smoothedVelocity[4] = { 0, 0, 0, 0 };
int cap_threshold[4] = { 1000, 1000, 1000, 1000 };  //<<<<<<<<<<<<< Touch sensitivity configuration

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

//MIDI message - Touch Sensor
int TO_midiChannel = 2;
int TO_midiCC[4] = { 3, 4, 5, 6 };
int TO_midiNotes[4] = { 55, 59, 62, 64 };

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

  //touchplates capacitive setup
  touchplates[0] = new CapacitiveSensor(2, 3);
  touchplates[1] = new CapacitiveSensor(5, 6);
  touchplates[2] = new CapacitiveSensor(7, 8);
  touchplates[3] = new CapacitiveSensor(10, 16);
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


  //---------------------------------------------- Touch reading
  for (int i = 0; i < 4; i++) {
    touch[i] = touchplates[i]->capacitiveSensor(30);
  }

  //---------------------------------------------- Touch MIDI output
  for (int i = 0; i < 4; i++) {
    if (touch[i] > cap_threshold[i]) {
      velocity[i] = (touch[i] - cap_threshold[i]) / 30;
      smoothedVelocity[i] = (velocity[i] * smoothingFactor) + (smoothedVelocity[i] * (1 - smoothingFactor));
      velocity[i] = smoothedVelocity[i];
      controlChange(TO_midiChannel, TO_midiCC[i], velocity[i]);
      MidiUSB.flush();
      if (NoteOn[i] == 0) {
        noteOn(TO_midiChannel, TO_midiNotes[i], 127);
        MidiUSB.flush();
        NoteOn[i] = 1;
      }
    }
    if (touch[i] < cap_threshold[i] && NoteOn[i] > 0) {
      NoteOn[i] = 0;
      noteOff(TO_midiChannel, TO_midiNotes[i], 127);
      MidiUSB.flush();
    }
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
    Serial.print("\t");
    Serial.print("Touch 0: ");
    Serial.print(touch[0]);  // print touch0 sensor output
    Serial.print("\t");
    Serial.print("Touch 1: ");
    Serial.print(touch[1]);  // print touch1 sensor output
    Serial.print("\t");
    Serial.print("Touch 2: ");
    Serial.print(touch[2]);  // print touch2 sensor output
    Serial.print("\t");
    Serial.print("Touch 3: ");
    Serial.println(touch[3]);  // print touch3 sensor output
    TimeSerial = TimeNow;
  }
}
```
