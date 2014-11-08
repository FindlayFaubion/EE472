
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "stdbool.h"

// hardware memory map, GPIO driver, and PWM driver
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"

// random seed
extern int seed;

// Declare global state variables
extern bool north;
extern bool east;
extern bool west;
extern bool south;
extern bool gridlock;
extern bool trainPresent;
extern unsigned int trainSize;
extern unsigned int globalCount;
extern bool gridlockChecked;

// Define common constants
#define OLED_FREQ 1000000
#define ASCII_OFFSET 48 // offset ascii to display numbers
#define TRAIN_SIZE_MIN 2  // min train size
#define TRAIN_SIZE_MAX 9  // max train size
#define GLOBAL_CNT_PER_MIN 120 // global counts per minute
#define CLEAR_SCREEN "                \0"
#define FREQUENCY 75 //frequency used by train buzzer
#define OLED_LEVEL 15 //brightness level for the OLED 
// global count lengths for the train light and sound
#define NTLIGHT_LEN 6 
#define NTSOUND_LEN 20
#define ETLIGHT_LEN 8
#define ETSOUND_LEN 26
#define STLIGHT_LEN 4
#define STSOUND_LEN 24
#define WTLIGHT_LEN 4
#define WTSOUND_LEN 14


//NorthTraindata
typedef struct {
    unsigned char* light; //assuming globalcount++ = 0.5 s
    unsigned char* sound;
    int i; //count for delays and sound
} northTrainData;

//EastTraindata
typedef struct {
    unsigned char* light; 
    unsigned char* sound;
    int i;
} eastTrainData;

//SouthTraindata
typedef struct {
    unsigned char* light; 
    unsigned char* sound;
    int i;
} southTrainData;

//WestTraindata
typedef struct {
    unsigned char* light; 
    unsigned char* sound;
    int i;
} westTrainData;

//CurrentTrainData
typedef struct {
    unsigned char* light; 
    unsigned char* sound;
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
extern northTrainData ntd;
extern eastTrainData etd;
extern southTrainData std;
extern westTrainData wtd;
extern switchControlData scd;
//extern scheduleData sd;

// train control block 
extern tcb taskArray[6];

// Train control 
void Schedule();
void TrainCom (void* data);
void SwitchControl (void* data);
void NorthTrain (void* data);
void WestTrain (void* data);
void EastTrain (void* data);
void SouthTrain (void* data);
void SerialCom (void* data); 


// System related
void Startup();
void InitBuzzer(int freq);

// Misc helper functions
int RandomInt(int low, int high);
char* GetDirection();
void SetNTData(unsigned char*, unsigned char*);
void SetETData(unsigned char*, unsigned char*);
void SetSTData(unsignehd char*, unsigned char*);
void SetWTData(unsigned char*, unsigned char*);
void SetSCData(unsigned char*);
//void SetSData();