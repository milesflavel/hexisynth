//#include <SoftwareSerial.h>
//const int rx = 3;
//const int tx = 4;
//SoftwareSerial ss = SoftwareSerial(rx, tx);

int latchPin = 2;
int clockPin = 1;
int dataPin = 0;

void setup()
{
  //pinMode(rx, INPUT);
  //pinMode(tx, OUTPUT);
  //ss.begin(9600);
  Serial.begin(31250);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
}

long previousMillis = 0;
unsigned long tempo = 50;
int diff = 1;
int mode = 0;
int state = 0;

int notes[3] = {-1, -1, -1};

boolean tones[16][16] = {{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
                         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};

uint8_t reg1 = 0;
uint8_t reg2 = 0;

void setRegisters(int count) {
  if(count == -1) {
    reg1 = 0;
    reg2 = 0;
  }
  else {
    for(int i=16; i>0; i--) {
      if(i>8) bitWrite(reg2, 16-i, tones[count][i-1]);
      else bitWrite(reg1, 8-i, tones[count][i-1]);
    }
  }
}

//MIDI bytes
byte commandByte;
byte noteByte;
byte velocityByte;

boolean checkMIDI()
{
  boolean changed = false;
  do
  {
    if (Serial.available())
    {
      commandByte = Serial.read();//read first byte
      noteByte = Serial.read();//read next byte
      velocityByte = Serial.read();//read final byte
      doNoteLogic();
    }
  }
  while (Serial.available() > 2);//when at least three bytes available
}

byte noteOn = 0x90;
byte noteOff = 0x80;

void doNoteLogic()
{
  if (commandByte == noteOn && velocityByte != 0x00)
  {
    boolean alreadyActive = false;
    for (int n=0; n<3; n++)
    {
      if (notes[n] == (int)noteByte) alreadyActive == true;
    }
    if (!alreadyActive)
    {
      notes[2] = notes[1];
      notes[1] = notes[0];
      notes[0] = (int)noteByte;
    }
  }
  if (commandByte == noteOff || (commandByte == noteOn && velocityByte == 0x00))
  {
    for (int n=0; n<3; n++)
    {
      if (notes[n] == (int)noteByte) notes[n] = -1;
    }
  }
}

void loop()
{
  /*
  if(ss.available()>0) {
    if(ss.read() == '#') {
      notes[0] = ss.read();
      notes[1] = ss.read();
      notes[2] = ss.read();
    }
    ss.print("#");
    ss.print(notes[0]);
    ss.print(notes[1]);
    ss.println(notes[2]);
  }
  */
  
  //Check for incoming MIDI signals
  checkMIDI();
  
  digitalWrite(latchPin, LOW);
  if(notes[0] == -1) setRegisters(-1);
  else setRegisters((int)notes[0]);
  shiftOut(dataPin, clockPin, MSBFIRST, reg1);
  shiftOut(dataPin, clockPin, MSBFIRST, reg2);
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin, LOW);
  if(notes[1] == -1) setRegisters(-1);
  else setRegisters((int)notes[1]);
  shiftOut(dataPin, clockPin, MSBFIRST, reg1);
  shiftOut(dataPin, clockPin, MSBFIRST, reg2);
  digitalWrite(latchPin, HIGH);
  delay(10);
  digitalWrite(latchPin, LOW);
  if(notes[2] == -1) setRegisters(-1);
  else setRegisters((int)notes[2]);
  shiftOut(dataPin, clockPin, MSBFIRST, reg1);
  shiftOut(dataPin, clockPin, MSBFIRST, reg2);
  digitalWrite(latchPin, HIGH);
  delay(10);
  
  /*
  if(mode==0) diff = 1;
  if(mode==1) diff = -1;
  if(mode==2)
  {
    if(state==0) diff = 1;
    if(state==15) diff = -1;
  }

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > tempo) {
    previousMillis = currentMillis;
    state += diff;
  }
  
  digitalWrite(latchPin, LOW);
  
  switch(state)
  {
    case -1:
    state = 15;
    break;
    case 0:
    setRegisters(state);
    break;
    case 1:
    setRegisters(state);
    break;
    case 2:
    setRegisters(state);
    break;
    case 3:
    setRegisters(state);
    break;
    case 4:
    setRegisters(state);
    break;
    case 5:
    setRegisters(state);
    break;
    case 6:
    setRegisters(state);
    break;
    case 7:
    setRegisters(state);
    break;
    case 8:
    setRegisters(state);
    break;
    case 9:
    setRegisters(state);
    break;
    case 10:
    setRegisters(state);
    break;
    case 11:
    setRegisters(state);
    break;
    case 12: 
    setRegisters(state);
    break;
    case 13:
    setRegisters(state);
    break;
    case 14:
    setRegisters(state);
    break;
    case 15:
    setRegisters(state);
    break;
    case 16:
    state = 0;
    break;
  }
  
  shiftOut(dataPin, clockPin, MSBFIRST, reg1);
  shiftOut(dataPin, clockPin, MSBFIRST, reg2);
  digitalWrite(latchPin, HIGH);
  */
}
