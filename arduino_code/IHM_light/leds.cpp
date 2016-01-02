
#include "Arduino.h"
#include "gendefs.h"
#include "leds.h"
#include "led_scroll.h"

static short led_bright[4]={15, 15, 15, 15};
static short bright_cpt=0;

static long blink_length = 250;
static long lastBlinkToggle = 0;
static boolean blink_phase=false;

static char led_state[4]={'0','0','0','0'};

static long LastScrollPhaseTime = 0;
static long scroll_phase_delay = 200;
static int scroll_phase = 0;

extern int pmi_menu;

// private functions prototypes
void compute_blink_phase();

void setLedState(short led_idx, char state)
{
  led_state[led_idx] = state;
}

void setLedBrightness(short led_idx, short brightness)
{
  led_bright[led_idx] = brightness;
}

void outrgb(int ledidx, const char color)
{
  for(int i=0;i<3;i++){
    digitalWrite(LedArray[ledidx*3+i], ColorArray[color][i]);
  }
}

void blinkLed(int led, const char color)
{
  if(blink_phase==true) setLed(led, color);
  else setLed(led, '0');
}

void setLed(int led, const char color) {
  if(led_bright[0]<bright_cpt){
    outrgb(led, LED_OFF);
  } else {
    switch(color){
      case '0': outrgb(led,LED_OFF); break;
      case 'R': outrgb(led,LED_RED); break;
      case 'G': outrgb(led,LED_GRN); break;
      case 'O': outrgb(led,LED_ORG); break;
      case 'B': outrgb(led,LED_BLU); break;
      case 'P': outrgb(led,LED_PRP); break;
      case 'C': outrgb(led,LED_CYN); break;
      case 'W': outrgb(led,LED_WHT); break;
      case 'r':
      case 'g':
      case 'o':
      case 'b':
      case 'p':
      case 'c':
      case 'w':
        blinkLed(led, color-('a'-'A')); break;
    }
  }
}

void processLeds(char state) {
  switch(state){
  case 0:
    // Everybody is OFF
    led_state[0]='0';led_state[1]='0';led_state[2]='0';led_state[3]='0';
    for(int i=0;i<4;i++){
        setLed(i,led_state[i]);
    }
    break;
    
  case 1:
    // Boot scrolling
    for(int i=0;i<4;i++){
      setLed(i,scroll[scroll_phase][i]);
    }
    if(millis()-LastScrollPhaseTime>scroll_phase_delay){
      scroll_phase++;
      scroll_phase=scroll_phase%SCROLL_STATE_MAX;
      LastScrollPhaseTime=millis();
    }
    break;
    
  case 2:
    // Normal operation
    for(int i=0;i<4;i++){
      setLed(i,led_state[i]);
    }
    break;
    
  case 3:
    // PMI Menu
    for(int i=0;i<4;i++){
      setLed(i,i==pmi_menu?'R':'G');
    }
    break;
  }
  compute_blink_phase();
  bright_cpt+=1;
  bright_cpt&=0xf;
}

void compute_blink_phase()
{
  if(millis()-lastBlinkToggle > blink_length ||
     millis() < lastBlinkToggle) {
    blink_phase = !blink_phase;
    lastBlinkToggle = millis();
  }
}


