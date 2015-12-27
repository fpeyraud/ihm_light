
#define LED_OFF 0
#define LED_RED 1
#define LED_GRN 2
#define LED_ORG 3
#define LED_BLU 4
#define LED_PRP 5
#define LED_CYN 6
#define LED_WHT 7

#define PIN_PWON 1
#define PIN_RST  0
#define PIN_PLED 2

#define PIN_LEFT_BTN  4
#define PIN_RIGHT_BTN 5

#define PIN_LED1R 8
#define PIN_LED1B 9
#define PIN_LED1G 7
#define PIN_LED2R 10
#define PIN_LED2B 16
#define PIN_LED2G 6
#define PIN_LED3R 15
#define PIN_LED3B 18
#define PIN_LED3G 14
#define PIN_LED4R 20
#define PIN_LED4B 21
#define PIN_LED4G 19

// 7 6 16 19

const int LedArray[12]={PIN_LED1B, PIN_LED1G, PIN_LED1R,
                        PIN_LED2B, PIN_LED2G, PIN_LED2R,
                        PIN_LED3B, PIN_LED3G, PIN_LED3R,
                        PIN_LED4B, PIN_LED4G, PIN_LED4R};
char led_state[4]={'0','0','0','0'};

const char scroll[7][4] = {{'0','0','0','0'},
                           {'R','0','0','0'},
                           {'O','R','0','0'},
                           {'G','O','R','0'},
                           {'0','G','O','R'},
                           {'0','0','G','O'},
                           {'0','0','0','G'},
                          };
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

long blink_length = 250;
long lastBlinkToggle = 0;
boolean blink_phase=false;

long LastScrollPhaseTime = 0;
long scroll_phase_delay = 200;
int scroll_phase = 0;

int pmi_menu = 0;


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
  
  pinMode(PIN_LED1R, OUTPUT);
  pinMode(PIN_LED1G, OUTPUT);
  pinMode(PIN_LED1B, OUTPUT);
  pinMode(PIN_LED2R, OUTPUT);
  pinMode(PIN_LED2G, OUTPUT);
  pinMode(PIN_LED2B, OUTPUT);
  pinMode(PIN_LED3R, OUTPUT);
  pinMode(PIN_LED3G, OUTPUT);
  pinMode(PIN_LED3B, OUTPUT);
  pinMode(PIN_LED4R, OUTPUT);
  pinMode(PIN_LED4G, OUTPUT);
  pinMode(PIN_LED4B, OUTPUT);
  digitalWrite(PIN_LED1R, HIGH);
  digitalWrite(PIN_LED1G, LOW);
  digitalWrite(PIN_LED1B, LOW);
  digitalWrite(PIN_LED2R, HIGH);
  digitalWrite(PIN_LED2G, LOW);
  digitalWrite(PIN_LED2B, LOW);
  digitalWrite(PIN_LED3R, HIGH);
  digitalWrite(PIN_LED3G, LOW);
  digitalWrite(PIN_LED3B, LOW);
  digitalWrite(PIN_LED4R, HIGH);
  digitalWrite(PIN_LED4G, LOW);
  digitalWrite(PIN_LED4B, LOW);
  delay(500);
  digitalWrite(PIN_LED1R, LOW);
  digitalWrite(PIN_LED1G, LOW);
  digitalWrite(PIN_LED1B, LOW);
  digitalWrite(PIN_LED2R, LOW);
  digitalWrite(PIN_LED2G, LOW);
  digitalWrite(PIN_LED2B, LOW);
  digitalWrite(PIN_LED3R, LOW);
  digitalWrite(PIN_LED3G, LOW);
  digitalWrite(PIN_LED3B, LOW);
  digitalWrite(PIN_LED4R, LOW);
  digitalWrite(PIN_LED4G, LOW);
  digitalWrite(PIN_LED4B, LOW);
}

void loop()
{
  process_serial();
  process_commands();
  poll_buttons();
  processLeds();
  compute_blink_phase();
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

void compute_blink_phase()
{
  if(millis()-lastBlinkToggle > blink_length ||
     millis() < lastBlinkToggle) {
    blink_phase = !blink_phase;
    lastBlinkToggle = millis();
  }
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
      led_state[ledidx] = inputString.charAt(3);
    }
    else if(inputString.startsWith("L")){
      for(int i=0;i<4;i++){
        led_state[i]=inputString.charAt(i+1);
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

void processLeds() {
//  setLed(PIN_LED1G, PIN_LED1R, inputString.charAt(1));
//  setLed(PIN_LED2G, PIN_LED2R, inputString.charAt(2));
//  setLed(PIN_LED3G, PIN_LED3R, inputString.charAt(3));
//  setLed(PIN_LED4G, PIN_LED4R, inputString.charAt(4));
  switch(state){
  case 0:
    led_state[0]='0';led_state[1]='0';led_state[2]='0';led_state[3]='0';
    for(int i=0;i<4;i++){
        setLed(LedArray[i*3],LedArray[i*3+1],LedArray[i*3+2],led_state[i]);
    }
  case 1:
    for(int i=0;i<4;i++){
      setLed(LedArray[i*3],LedArray[i*3+1],LedArray[i*3+2],scroll[scroll_phase][i]);
    }
    if(millis()-LastScrollPhaseTime>scroll_phase_delay){
      scroll_phase++;
      scroll_phase=scroll_phase%7;
      LastScrollPhaseTime=millis();
    }
    break;
    
  case 2:
    for(int i=0;i<4;i++){
      setLed(LedArray[i*3],LedArray[i*3+1],LedArray[i*3+2],led_state[i]);
    }
    break;
  case 3:
    for(int i=0;i<4;i++){
      setLed(LedArray[i*3],LedArray[i*3+1],LedArray[i*3+2],i==pmi_menu?'R':'G');
    }
    break;
  }
}

void setLed(int pin_blue, int pin_green, int pin_red, const char color) {
  switch(color){
    case '0':
      digitalWrite(pin_blue, LOW);
      digitalWrite(pin_green, LOW);
      digitalWrite(pin_red, LOW);
      break;
    case 'R':
      digitalWrite(pin_blue, LOW);
      digitalWrite(pin_green, LOW);
      digitalWrite(pin_red, HIGH);
      break;
    case 'G':
      digitalWrite(pin_blue, LOW);
      digitalWrite(pin_green, HIGH);
      digitalWrite(pin_red, LOW);
      break;
    case 'O':
      digitalWrite(pin_blue, LOW);
      digitalWrite(pin_green, HIGH);
      digitalWrite(pin_red, HIGH);
      break;
    case 'B':
      digitalWrite(pin_blue, HIGH);
      digitalWrite(pin_green, LOW);
      digitalWrite(pin_red, LOW);
      break;
    case 'P':
      digitalWrite(pin_blue, HIGH);
      digitalWrite(pin_green, LOW);
      digitalWrite(pin_red, HIGH);
      break;
    case 'C':
      digitalWrite(pin_blue, HIGH);
      digitalWrite(pin_green, HIGH);
      digitalWrite(pin_red, LOW);
      break;
    case 'W':
      digitalWrite(pin_blue, HIGH);
      digitalWrite(pin_green, HIGH);
      digitalWrite(pin_red, HIGH);
      break;
    case 'r':
      if(blink_phase==true) setLed(pin_blue, pin_green, pin_red, 'R');
      else setLed(pin_blue, pin_green, pin_red, '0');
      break;
    case 'g':
      if(blink_phase==true) setLed(pin_blue, pin_green, pin_red, 'G');
      else setLed(pin_blue, pin_green, pin_red, '0');
      break;
    case 'o':
      if(blink_phase==true) setLed(pin_blue, pin_green, pin_red, 'O');
      else setLed(pin_blue, pin_green, pin_red, '0');
      break;
    case 'b':
      if(blink_phase==true) setLed(pin_blue, pin_green, pin_red, 'B');
      else setLed(pin_blue, pin_green, pin_red, '0');
      break;
    case 'p':
      if(blink_phase==true) setLed(pin_blue, pin_green, pin_red, 'P');
      else setLed(pin_blue, pin_green, pin_red, '0');
      break;
    case 'c':
      if(blink_phase==true) setLed(pin_blue, pin_green, pin_red, 'C');
      else setLed(pin_blue, pin_green, pin_red, '0');
      break;
  }
}
