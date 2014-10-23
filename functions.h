#ifndef HEADER_F 
#define HEADER_F

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"

//Include hardware memory map, GPIO driver, and PWM driver
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"

//Define boolean type 
enum myBool { FALSE = 0, TRUE = 1 };
typedef enum myBool bool;

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

//Null point
#define NULL 0

// offset ascii to display numbers
#define ASCII_OFFSET 48

//NorthTraindata
typedef struct {
    unsigned char* light; //assuming globalcount++ = 0.5 s
    unsigned char* sound;
    int i;//counter variable to count through sound; can I initialize it here?
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

//Function prototypes
void TrainCom (void* data);
void SwitchControl (void* data);
void NorthTrain (void* data);
void WestTrain (void* data);
void EastTrain (void* data);
void Schedule (void* data);

// helper functions
int RandomInt(int low, int high);
void SetNTData(unsigned char*, unsigned char*);
void SetETData(unsigned char*, unsigned char*);
void SetWTData(unsigned char*, unsigned char*);

#endif