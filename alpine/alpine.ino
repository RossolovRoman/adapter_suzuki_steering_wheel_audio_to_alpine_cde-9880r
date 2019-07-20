// Suzuki SX4 2006-10 steering wheel remote control interface for Apline
//
// Arduino:     (+5V)           (anaPin)      (GND)      (alpPin)
//                ^                 ^           ^           ^  
//                |   +---------+   |           |           |
//                +---|  1 kOm  |---+           |           |
//                    +---------+   |           |           |
//                                  |           |           |
// Remote key:                      +-- (KEY) --+----+      |
//                                                   |      |
// Jack:                                             |      |
//                                      +--------+   |      |
//                                  OOOO|        | ==== == --
//                                      +--------+
//
//
// Remote key (SUZUKI SX4 2006-10): 
//
//   +-3,6kOm--+--820--+--330--+--180--+--110--+--82--+--30--o   <-- (anaPin)
//   |         |       |       |       |       |      |    
//   |       SEEK-   SEEK+   MODE    VOL-    VOL+   MUTE
//   |         |       |       |       |       |      |  
//   |        \       \       \       \       \      \
//   |         |       |       |       |       |      |  
//   +---------+-------+-------+-------+-------+------+------o   <-- (GND)




#define CHECK_MS 5
#define DOUBLE_MS 200
#define HOLD_MS 1200

#define NO_B 859           //nothing pressed
#define NEXT_DOWN_B 627    // SEEK DOWN
#define NEXT_UP_B 441      // SEEK UP
#define MODE_B 305         // MODE
#define VOLUME_DOWN_B 200  // VOLUME DOWN
#define VOLUME_UP_B 120    // VOLUME UP
#define CHANGE_B 55        // CHANGE

#define alpPin 3           //ALPINE JACK
#define anaPin 1           //REMOTE KEY
#define led 13             //LED

boolean volUp[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 1,1,0,1,1,0,1,1, 1,1,0,1,0,1,1,0, 1,1,0,1,0,1,0,1};    //VOLUME UP
boolean volDn[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,1,0,1,1,0,1, 1,1,1,1,0,1,1,0, 1,1,0,1,0,1,0,1};    //VOLUME DOWN
boolean mute[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 1,0,1,0,1,1,0,1, 1,1,1,0,1,1,1,0, 1,1,0,1,0,1,0,1};     //MUTE
boolean pstUp[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 1,0,1,0,1,0,1,1, 1,1,1,0,1,1,1,1, 0,1,0,1,0,1,0,1};    //FOLDER/MEMORY UP
boolean pstDn[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,0,1,0,1,0,1, 1,1,1,1,1,1,1,1, 0,1,0,1,0,1,0,1};    //FOLDER/MEMORY DOWN
boolean source[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 1,0,1,1,0,1,1,1, 1,1,0,1,1,0,1,1, 0,1,0,1,0,1,0,1};   //SOURCE (TUNER/CD/USB/AUX)
boolean trkUp[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 1,0,1,1,1,0,1,1, 1,1,0,1,1,0,1,0, 1,1,0,1,0,1,0,1};    //TRACK UP
boolean trkDn[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,0,1,1,1,0,1, 1,1,1,1,1,0,1,0, 1,1,0,1,0,1,0,1};    //TRACK DOWN
boolean power[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,1,1,0,1,1,1, 1,1,1,0,1,0,1,1, 0,1,0,1,0,1,0,1};    //POWER (ON/OFF)
boolean entPlay[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,0,1,0,1,1,1, 1,1,1,1,1,1,0,1, 0,1,0,1,0,1,0,1};  //MODE (PLAY/PAUSE)
boolean bandProg[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,1,0,1,0,1,1, 1,1,1,1,0,1,1,1, 0,1,0,1,0,1,0,1}; //BANG PROG (FM1/FM2/FM3/AM)
boolean defeat[48] = {1,1,0,1,0,1,1,1, 1,1,0,1,1,0,1,1, 1,0,1,0,1,0,1,1, 0,1,1,0,1,1,1,1, 1,1,1,0,1,1,0,1, 0,1,0,1,0,1,0,1};   //DEFEAT

boolean spFlag = 0;
boolean dpFlag = 0;
boolean hpFlag = 0;

int button = 0; 
int spbutton = 0;

void setup() {
Serial.begin(9600);
pinMode(alpPin, OUTPUT);
pinMode(led, OUTPUT);
}

void loop() {
static unsigned long oldTime = 0;
int switchStatus = 0;
if (millis() >= oldTime + CHECK_MS) {
oldTime = millis();
switchStatus = CheckSwitch();

if (switchStatus == 0) {
//no button press
} else if (switchStatus == 1) {
// single press
switch (spbutton) {
case 0: // no button, why are wh here
break;
case 1: // track up
commandSend(trkUp);
break;
case 2: // track down
commandSend(trkDn);
break;
case 3: // vol up
commandSend(volUp);
break;
case 4: // vol down
commandSend(volDn);
break;
case 5: // change
commandSend(source);
break;
case 6: // mode
commandSend(entPlay);
break;
} 
} else if (switchStatus == 2) {
//double press
switch (spbutton) {
case 0: // no button, why are wh here
break;
case 1: // folder up
commandSend(pstUp);
break;
case 2: // folder down
commandSend(pstDn);
break;
case 3: // vol up
commandSend(volUp);
commandSend(volUp);
break;
case 4: // vol down
commandSend(volDn);
commandSend(volDn);
break;
case 5: // change
commandSend(source);
commandSend(source);
break;
case 6: // mode
commandSend(entPlay);
commandSend(entPlay);
break;
} 
} else if (switchStatus == 3) {
//hold
switch (spbutton) {
case 0: // no button, why are wh here
break;
case 1: // track up
commandSend(trkUp);
break;
case 2: // track down
commandSend(trkDn);
break;
case 3: // vol up
commandSend(volUp);
break;
case 4: // vol down
commandSend(volDn);
break;
case 5: // change
commandSend(power);
break;
case 6: // mode
commandSend(mute);
break;
}}}
}

// Service routine called by a timer interrupt
int CheckSwitch() {
static uint16_t State = 0; // Current debounce status
static int curTime = 0;

State=(State<<1) | !RawKeyPressed() | 0xe000;
if (!spFlag) {
if (hpFlag) {
if (State == 0xe000) {
return 3;
} else {
hpFlag = 0;
}
}
if (State==0xf000) {
spFlag = 1;
hpFlag = 1;
dpFlag = 1;
curTime = 0;
spbutton = button;
return 0;
}
} 
if (hpFlag && State != 0xe000) {
hpFlag = 0;
}
if (spFlag && hpFlag && curTime > HOLD_MS/CHECK_MS) {
spFlag = 0;
dpFlag = 0;
return 3;
} else if (spFlag && dpFlag && State == 0xf000 && curTime < DOUBLE_MS) {
spFlag = 0;
hpFlag = 0;
dpFlag = 0;
return 2;
} else if (spFlag && !hpFlag && curTime > DOUBLE_MS) {
spFlag = 0;
hpFlag = 0;
dpFlag = 0;
return 1;
} else {
curTime = curTime + CHECK_MS;
return 0;
}
}

boolean RawKeyPressed() {
int analogPin = analogRead(anaPin); //REMOTE KEY
if (analogPin > NO_B - 10) {
button = 0;
return 0;
} else if (analogPin > NEXT_DOWN_B - 10 & analogPin < NEXT_DOWN_B + 10) {
button = 2;
return 1;
} else if (analogPin > NEXT_UP_B - 10 & analogPin < NEXT_UP_B + 10) {
button = 1;
return 1;
} else if (analogPin > MODE_B - 10 & analogPin < MODE_B + 10) {
button = 5;
return 1;
} else if (analogPin > VOLUME_UP_B - 10 & analogPin < VOLUME_UP_B + 10) {
button = 3;
return 1;
} else if (analogPin > VOLUME_DOWN_B - 10 & analogPin < VOLUME_DOWN_B + 10 ) {
button = 4;
return 1;
} else if (analogPin > CHANGE_B - 10 & analogPin < CHANGE_B + 10 ) {
button = 6;
return 1;
}else {
button = 0;
return 0;
}
}

//---------SEND COMMAND-----------------------------------------------
void commandSend(boolean mas[]) {
// led blinker (command start)
digitalWrite(led, HIGH);

//first send 8ms high
digitalWrite(alpPin, HIGH);
delay(8);
// send 4.5ms low
digitalWrite(alpPin, LOW);
delayMicroseconds(4500);

for (int i = 0; i <= 47; i++) {
//send bit for 0.5ms
if (mas[i] == 1 ) {
digitalWrite(alpPin, HIGH);
} else {
digitalWrite(alpPin, LOW);
} 
delayMicroseconds(500);
// wait 0.5ms
digitalWrite(alpPin, LOW);
delayMicroseconds(500);
}
// send 41ms low
digitalWrite(alpPin, LOW);
delay(41); 

// led blinker (command finish)
digitalWrite(led, LOW);
}
