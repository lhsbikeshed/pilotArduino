#define JUMPLED 3
#define JUMPBUTTON 4


bool switchStates[5];
bool lastSwitchStates[5];

int avg = 0;
int lastThrottle = 0;

int jumpBlink = 0;
boolean jumpBlinking = false;
byte jumpLEDOn = 0;
byte lastButtonState = LOW;

void setup(){
  for(int i = 0; i < 5; i++){
    switchStates[i] = 0;
    
    pinMode(14 + i, INPUT);
    digitalWrite(14 + i, HIGH);
    if(digitalRead(14+i)){
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
 
}

void loop(){
  
  while(Serial.available()){
    char c = Serial.read();
    if(c == 'b'){
      jumpBlinking = false;
      digitalWrite(JUMPLED,LOW);
      jumpLEDOn = LOW;
    } else if(c == 'B') {
      jumpBlinking = true;
      digitalWrite(JUMPLED,LOW);
      jumpLEDOn = LOW;
    }
  }
  
  
  
  int throttle = map(analogRead(5),14,1024,0,255);
  if(throttle != lastThrottle){
    Serial.print("t");
    Serial.print(255-throttle);
    Serial.print(",");
    
    
    lastThrottle = throttle;
  }
  for(int i = 0; i < 5; i++){
    boolean val = digitalRead(14+i) == HIGH ? true : false;
    if(val != lastSwitchStates[i]){
      Serial.print(i);
      Serial.print(val == true ? 1 : 0);
      Serial.print(",");
      lastSwitchStates[i] = val;
    }
  }
  
  delay(100);
  
  //jump leds
  if(jumpBlinking){
    jumpBlink ++;
    if(jumpBlink > 5){
      jumpBlink = 0;
      jumpLEDOn = 1-jumpLEDOn;
      digitalWrite(JUMPLED, jumpLEDOn);
    }
  }
  
  byte v = digitalRead(JUMPBUTTON);
  if(v != lastButtonState){
    if(v == HIGH && lastButtonState == LOW){
      Serial.print("51,");
    }
    lastButtonState = v;
  }
  
}

