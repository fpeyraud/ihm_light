#include "gendefs.h"

#include "led_scroll.h"



// State machine
// 0 just booted
// 1 initialized, waiting while host boots
// 2 synched
// 3 FrontPanel interface
char state = 0;
char stbkp = 0;

String inputString = "";
boolean fStringComplete = false;
int LbuttonState, RbuttonState;
int LlastButtonState = HIGH;
int RlastButtonState = HIGH;

long LlastDebounceTime = 0;
long RlastDebounceTime = 0;
long debounceDelay = 50;


int pmi_menu = 0;

#include "leds.h"

void setup()
{
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  
  inputString.reserve(6);

  pinMode(PIN_LEFT_BTN,  INPUT_PULLUP);
  pinMode(PIN_RIGHT_BTN, INPUT_PULLUP);

  pinMode(PIN_PWON, INPUT);
  digitalWrite(PIN_PWON, LOW);
  pinMode(PIN_RST,  INPUT);
  digitalWrite(PIN_RST,  LOW);
  pinMode(PIN_PLED, INPUT);
  digitalWrite(PIN_PLED, LOW);
  
  for(int i=0;i<12;i++) pinMode(LedArray[i], OUTPUT);
  for(int i=0;i<4;i++) setLed(i,'W');
  delay(500);
  for(int i=0;i<4;i++) setLed(i,'0');
}

void loop()
{
  process_serial();
  process_commands();
  poll_buttons();
  processLeds(state);
  compute_fsm_transition();
}

void compute_fsm_transition()
{
  static boolean FtempRun=false;
  static long DtempRun=0;

  // Manage interne  
  if(LbuttonState==LOW && RbuttonState==LOW){
    if(!FtempRun){
      // Start temporization
      FtempRun=true;
      DtempRun=millis();
    } else {
      if(millis()-DtempRun > 3000){
        //Temporization expired
        stbkp=state;
        state=3;
        FtempRun=false;
      }
    }
  } else if(FtempRun){
    // reset Temporization flag
    FtempRun=false;
  }

  switch(state){
  case 0:
    //if (get_Power_Led()) state=1;
    state=1;
    delay(500);
    break;
  case 1:
    if (Serial) state=2;
    break;
  case 2:
    break;
  case 3:
    FtempRun=true;
    if(LbuttonState==LOW||RbuttonState==LOW) DtempRun=millis();
    if(millis()-DtempRun > 5000){
      //Temporization expired
      state=stbkp;
      FtempRun=false;
    }
    break;
  }
  // in ANY stat, these conditions apply
  //if (!get_power_led()) state=0; 
}

void poll_buttons()
{
  int reading = digitalRead(PIN_LEFT_BTN);
  if (reading != LlastButtonState) LlastDebounceTime = millis();
  if ((millis() - LlastDebounceTime) > debounceDelay) {
    if (reading != LbuttonState) {
      LbuttonState = reading;
      switch(state){
      case 2:
        if (LbuttonState == LOW) Serial.print("L\n"); else Serial.print("l\n");
        break;
      case 3:
        if (LbuttonState == LOW) pmi_menu=((++pmi_menu)%4);
        break;
      }
    }
  }
  LlastButtonState = reading;
  
  reading = digitalRead(PIN_RIGHT_BTN);
  if (reading != RlastButtonState) RlastDebounceTime = millis();
  if ((millis() - RlastDebounceTime) > debounceDelay) {
    if (reading != RbuttonState) {
      RbuttonState = reading;
      switch(state){
      case 2:
        if (RbuttonState == LOW) Serial.print("R\n"); else Serial.print("r\n");
        break;
      case 3:
        if (RbuttonState == LOW) process_pmi_menu();
        break;
      }
    }
  }
  RlastButtonState = reading;
}

void process_pmi_menu()
{
  switch(pmi_menu){
  case 0:
    break;
  case 1:
    trigger_nmi();
    break;
  case 2:
    trigger_reset();
    break;
  case 3:
    trigger_poweroff();
    break;
  }
  state=2;
  pmi_menu=0;
}

void process_commands()
{
  if(fStringComplete) {
    if(inputString=="PWOFF\n"){
      trigger_poweroff();
    }
    else if(inputString=="RESET\n"){
      trigger_reset();
    }
    else if(inputString=="NMI\n"){
      trigger_nmi();
    }
    else if(inputString.startsWith("SL")){
      int ledidx=inputString.charAt(2)-'0';
      ledidx--;
      //led_state[ledidx] = inputString.charAt(3);
      setLedState(ledidx, inputString.charAt(3));
    }
    else if(inputString.startsWith("BR")){
      setLedBrightness(0,(short)(inputString.substring(2).toInt()));
    }
    else if(inputString.startsWith("L")){
      for(int i=0;i<4;i++){
        //led_state[i]=inputString.charAt(i+1);
        setLedState(i, inputString.charAt(i+1));
      }
      //processLeds();
    } else {
      Serial.print("ERROR\n");
    }
    inputString="";
    fStringComplete=false;
  }
}

void pullPinDown(int pin, const int t) {
  digitalWrite(pin, LOW);
  pinMode(pin, OUTPUT);
  delay(t);
  pinMode(pin, INPUT);
}

void trigger_reset() {
  pullPinDown(PIN_RST, 300);
  state = 0;
}

void trigger_poweroff() {
  pullPinDown(PIN_PWON, 5000);
  state = 0;
}

void trigger_nmi() {
  pullPinDown(PIN_PWON, 300);
}

int process_serial(){
  while(Serial.available()){
    char c = Serial.read();
    inputString += c;
    if (c == '\n') {
      fStringComplete = true;
    }
  }
}


