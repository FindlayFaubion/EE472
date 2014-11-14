
#include "stdbool.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "inc/hw_nvic.h"
#include "drivers/rit128x96x4.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"

// random seed
extern int seed;

// Declare global state variables
extern bool serial_flag;
extern bool gridlock;
extern bool trainPresent;
extern unsigned int trainSize;
extern unsigned int globalCount;
extern bool gridlockChecked;
extern int dir_to;
extern int dir_from;
extern int pass_count;
extern int pulse_count;

// Define common constants
#define OLED_FREQ 1000000
#define ASCII_OFFSET 48 // offset ascii to display numbers
#define TRAIN_SIZE_MIN 2  // min train size
#define TRAIN_SIZE_MAX 9  // max train size
#define GLOBAL_CNT_PER_MIN 120 // global counts per minute
#define CLEAR_SCREEN "                        \0"
#define NA "N/A    \0"
#define FREQUENCY 75 //frequency used by train buzzer
#define OLED_LEVEL 15 //brightness level for the OLED 

// Hardware constants
#define BUTTON_PINS 0x000000FF
#define PULSE_PIN 0x00000004

// global count lengths for the train light and sound
#define NTLIGHT_LEN 6 
#define NTSOUND_LEN 20
#define ETLIGHT_LEN 8
#define ETSOUND_LEN 26
#define STLIGHT_LEN 4
#define STSOUND_LEN 24
#define WTLIGHT_LEN 4
#define WTSOUND_LEN 14

// Display coordinates 
#define BASE_X 0 
#define BASE_Y 0
#define SHIFT_Y 10
#define SHIFT_X 65
#define PRES 0*SHIFT_Y 
#define SZ 1*SHIFT_Y
#define FROM 2*SHIFT_Y 
#define TO 3*SHIFT_Y
#define PASS 4*SHIFT_Y
#define GLOB 5*SHIFT_Y
#define GRIDL 6*SHIFT_Y
#define UART_SHIFT 15
#define UART_STR_LEN 27

////NorthTraindata
//typedef struct {
//    unsigned char* light; //assuming globalcount++ = 0.5 s
//    unsigned char* sound;
//    int lightlen;
//    int soundlen;
//    int i; //count for delays and sound
//} northTrainData;
//
////EastTraindata
//typedef struct {
//    unsigned char* light; 
//    unsigned char* sound;
//    int i;
//} eastTrainData;
//
////SouthTraindata
//typedef struct {
//    unsigned char* light; 
//    unsigned char* sound;
//    int i;
//} southTrainData;
//
////WestTraindata
//typedef struct {
//    unsigned char* light; 
//    unsigned char* sound;
//    int i;
//} westTrainData;

//CurrentTrainData
typedef struct {
    unsigned char* light; 
    unsigned char* sound;
    int lightlen;
    int soundlen;
    int i;
} currentTrainData;

//switchControlData
typedef struct {
    unsigned char* light; 
    unsigned int i;
    unsigned int delay;
    bool gridlockChecked;
} switchControlData;

// schedule data
//typedef struct {
//    int clock_f;
//} scheduleData;

//TCB
typedef struct {
  void* y;
  void (*x)(void*);
} tcb;

// Declare global task data
extern currentTrainData ntd;
extern currentTrainData etd;
extern currentTrainData std;
extern currentTrainData wtd;
extern switchControlData scd;
//extern scheduleData sd;

// global train data
extern tcb taskArray[4];
extern currentTrainData trains[4];

// Train control 
void Schedule();
void TrainCom (void* data);
void SwitchControl (void* data);
void CurrentTrain (void* data);
void SerialCom (void* data); 

// System related
void Startup();
void InitBuzzer(int freq);

// Interrupt handlers
void ButtonHandler();
void IntTimer1();
void PulseCount();

// Misc helper functions
int RandomInt(int low, int high);
void IntToString(int num, char* string, int len);
void GetDirection(int dir, char word[]);
//void GetDirectionArray(int dir, char word[]);
int GetPassengers(int freq);
void SetNTData(unsigned char*, unsigned char*);
void SetETData(unsigned char*, unsigned char*);
void SetSTData(unsigned char*, unsigned char*);
void SetWTData(unsigned char*, unsigned char*);
void SetSCData(unsigned char*);
void UARTSend(const unsigned char*, unsigned long);