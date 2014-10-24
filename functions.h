
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
#define WTLIGHT_LEN 4
#define WTSOUND_LEN 14


//NorthTraindata
typedef struct {
    unsigned char* light; //assuming globalcount++ = 0.5 s
    unsigned char* sound;
    int i;
} northTrainData;

//EastTraindata
typedef struct {
    unsigned char* light; //assuming globalcount++ = 0.5 s
    unsigned char* sound;
    int i;//counter variable to count through sound
} eastTrainData;

//WestTraindata
typedef struct {
    unsigned char* light; //assuming globalcount++ = 0.5 s
    unsigned char* sound;
    int i;//counter variable to count through sound
} westTrainData;

//switchControlData
typedef struct {
    unsigned char* light; //assuming globalcount++ = 0.5 s
    unsigned int i;//counter variable to count through sound
    unsigned int delay;
    bool gridlockChecked;
} switchControlData;

// schedule data
typedef struct {
    int clock_f;
} scheduleData;

//TCB
typedef struct {
  void* y;
  void (*x)(void*);
} std_tcb;

//test struct
typedef struct {
    void (*x)(void*);
    void* y;
} test_struct;

// Declare global task data
extern northTrainData ntd;
extern eastTrainData etd;
extern westTrainData wtd;
extern switchControlData scd;
extern scheduleData sd;

// Train control 
void TrainCom (void* data);
void SwitchControl (void* data);
void NorthTrain (void* data);
void WestTrain (void* data);
void EastTrain (void* data);
void Schedule (void* data);

// Hardware related
void InitBuzzer(int freq);

// Misc helper functions
int RandomInt(int low, int high);
char* GetDirection();
void SetNTData(unsigned char*, unsigned char*);
void SetETData(unsigned char*, unsigned char*);
void SetWTData(unsigned char*, unsigned char*);
void SetSCData(unsigned char*);
void SetSData();