//void setLed(int, int , int , const char);
void setLed(int, const char);
void setLedBrightness(short, short);
void processLeds(char);
//void compute_blink_phase();
void setLedState(short led_idx, char state);

#define LED_OFF 0
#define LED_RED 1
#define LED_GRN 2
#define LED_ORG 3
#define LED_BLU 4
#define LED_PRP 5
#define LED_CYN 6
#define LED_WHT 7

const int LedArray[12]={PIN_LED1B, PIN_LED1G, PIN_LED1R,
                        PIN_LED2B, PIN_LED2G, PIN_LED2R,
                        PIN_LED3B, PIN_LED3G, PIN_LED3R,
                        PIN_LED4B, PIN_LED4G, PIN_LED4R};

const bool ColorArray[8][3]={
                            { LOW,  LOW,  LOW},
                            { LOW,  LOW, HIGH},
                            { LOW, HIGH,  LOW},
                            { LOW, HIGH, HIGH},
                            {HIGH,  LOW,  LOW},
                            {HIGH,  LOW, HIGH},
                            {HIGH, HIGH,  LOW},
                            {HIGH, HIGH, HIGH},
                            };
