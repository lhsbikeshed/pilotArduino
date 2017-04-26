/*
centre console

 top row 7 9
 bot row 8 6

 floppy box
 10
 13

 above eng:
 5  2

 above door

 12
 11

 tect

 3
 4
 */

#define JUMPLED 33
#define JUMPBUTTON 35

bool switchStates[5];
bool lastSwitchStates[5];

int avg = 0;
int lastThrottle = 0;

int jumpBlink = 0;
boolean jumpBlinking = false;
byte jumpLEDOn = 0;
byte lastButtonState = LOW;

int switchPinMap[] = {
  39,37,41,43,45 };

//------------- plug stuff
struct PlugState {
  int plug;
  int socket;
  boolean con;
}
plugState[6];
int sockets[] = { 2, 3, 6, 11, 14, 13 };
int plugs[] = { 4, 5, 7, 9, 10, 12 };
int currentSocket = 0;

void setup() {
  for (int i = 0; i < 5; i++) {
    switchStates[i] = 0;

    pinMode(switchPinMap[i], INPUT);
    digitalWrite(switchPinMap[i], HIGH);
    if (digitalRead(switchPinMap[i])) {
      lastSwitchStates[i] = true;
    } else {
      lastSwitchStates[i] = false;
    }
  }
  Serial.begin(115200);

  //jump stuff
  pinMode(JUMPLED, OUTPUT);
  digitalWrite(JUMPLED, LOW);
  jumpBlink = 0;
  pinMode(JUMPBUTTON, INPUT);
  digitalWrite(JUMPBUTTON, HIGH);

  //----- plug stuff
  for (int s = 0; s < 6; s++) {
    pinMode(sockets[s], OUTPUT);
    digitalWrite(sockets[s], LOW);
  }
  for (int p = 0; p < 6; p++) {
    pinMode(plugs[p], INPUT);
    digitalWrite(plugs[p], HIGH);
    plugState[p].plug = plugs[p];
    plugState[p].con = false;
    plugState[p].socket = 100 + p;
  }
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'b') {
      jumpBlinking = false;
      digitalWrite(JUMPLED, LOW);
      jumpLEDOn = LOW;
    } else if (c == 'B') {
      jumpBlinking = true;
      digitalWrite(JUMPLED, LOW);
      jumpLEDOn = LOW;
    }
  }

  int throttle = map(analogRead(5), 14, 1024, 0, 255);
  if (throttle != lastThrottle) {
    Serial.print("t");
    Serial.print(255 - throttle);
    Serial.print(",");

    lastThrottle = throttle;
  }
  for (int i = 0; i < 5; i++) {
    boolean val = digitalRead(switchPinMap[i]) == HIGH ? true : false;
    if (val != lastSwitchStates[i]) {
      Serial.print(i);
      Serial.print(val == true ? 1 : 0);
      Serial.print(",");
      lastSwitchStates[i] = val;
    }
  }

  delay(100);

  //jump leds
  if (jumpBlinking) {
    jumpBlink++;
    if (jumpBlink > 5) {
      jumpBlink = 0;
      jumpLEDOn = 1 - jumpLEDOn;
      digitalWrite(JUMPLED, jumpLEDOn);
    }
  }

  byte v = digitalRead(JUMPBUTTON);
  if (v != lastButtonState) {
    if (v == HIGH && lastButtonState == LOW) {
      Serial.print("51,");
    }
    lastButtonState = v;
  }

  //------ read the cable puzzle
  readCables();
}

void readCables() {
  for (int plugId = 0; plugId < 6; plugId++) {
    //read the plug states
    byte b = digitalRead(plugs[plugId]);
    if (!b) {
      if (plugState[plugId].con == false) {
        Serial.print("C");
        Serial.print(plugs[plugId]);
        Serial.print(":");
        Serial.print(sockets[currentSocket]);
        Serial.print(",");
        plugState[plugId].con = true;
        plugState[plugId].socket = currentSocket;
      }
    } else {
      if (plugState[plugId].con == true && plugState[plugId].socket == currentSocket) {
        Serial.print("c");
        Serial.print(plugs[plugId]);
        Serial.print(":");
        Serial.print(sockets[currentSocket]);
        Serial.print(",");
        plugState[plugId].con = false;
      }
    }
  }

  //turn sockets low except current one
  currentSocket++;
  currentSocket %= 6;
  for (int sl = 0; sl < 6; sl++) {
    if (sl != currentSocket) {
      digitalWrite(sockets[sl], HIGH);
    } else {
      digitalWrite(sockets[sl], LOW);
    }
  }
}
