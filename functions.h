/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Hardware library includes. */
#include "hw_memmap.h"
#include "hw_types.h"
#include "hw_sysctl.h"
#include "sysctl.h"
#include "gpio.h"
#include "grlib.h"
#include "rit128x96x4.h"
#include "osram128x64x4.h"
#include "formike128x128x16.h"

/* Demo app includes. */

#include "lcd_message.h"
//#include "bitmap.h"

//OUR stuff
#include "driverlib/adc.h"
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
#include <math.h>




//OLED queue
extern xQueueHandle xOLEDQueue;
// random seed
extern int seed;

extern int debug;

// Declare global state variable
extern unsigned char dis_sel; //d stands for display_selection
extern bool serial_flag;
extern bool gridlock;
extern bool trainPresent[2];
extern unsigned int trainSize[2];
extern unsigned int globalCount;
extern unsigned int oldGC;
extern bool gridlockChecked;
extern int dir_to[2];
extern int dir_from[2];
extern int pass_count[2];
extern int pulse_count;
extern int temp;

//second Train Globals
extern int dir_to2;
extern int dir_from2;
extern int pass_count2;
extern bool trainPresent2;
extern unsigned int trainSize2;

// Define common constants
#define OLED_FREQ 1000000
#define ASCII_OFFSET 48 // offset ascii to display numbers
#define TRAIN_SIZE_MIN 2  // min train size
#define TRAIN_SIZE_MAX 9  // max train size
#define GLOBAL_CNT_PER_MIN 120 // global counts per minute
#define CLEAR_SCREEN "                        \0"
#define NA "N/A    "
#define FREQUENCY 40 //frequency used by train buzzer
#define OLED_LEVEL 15 //brightness level for the OLED 

// Hardware constants
#define BUTTON_PINS 0x000000FF
#define PULSE_PIN 0x00000004
#define SELECT_PIN 0x00000002
#define DELAY 500
    
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
#define TEMP 6*SHIFT_Y
#define GRIDL 7*SHIFT_Y

#define UART_SHIFT 15
#define UART_STR_LEN 17


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
extern bool active;
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
void Timer();

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
void Print(signed char*, int, int);
int GetTemp(int adc_val);