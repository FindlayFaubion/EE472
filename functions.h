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

//TrainCom data
typedef struct {
  bool* north;
  bool* east;
  bool* west;
  bool* gridlock;
  bool* trainPresent;
  unsigned int* trainSize;
  unsigned int* globalCount;
} trainComData;

//SwitchControldata
typedef struct {
  bool* north;
  bool* east;
  bool* west;
  bool* gridlock;
  bool* trainPresent;
  unsigned int* trainSize;
  unsigned int* globalCount;
} switchControlData;

//NorthTraindata
typedef struct {
  bool* north;
  bool* gridlock;
  bool* trainPresent;
  unsigned int* trainSize;
  unsigned int* globalCount;
} northTrainData;

//EastTraindata
typedef struct {
  bool* east;
  bool* gridlock;
  bool* trainPresent;
  unsigned int* trainSize;
  unsigned int* globalCount;
} eastTrainData;

//WestTraindata
typedef struct {
  bool* west;
  bool* gridlock;
  bool* trainPresent;
  unsigned int* trainSize;
  unsigned int* globalCount;
} westTrainData;

//Scheduledata
typedef struct {
  unsigned int* globalCount;
} scheduleData;

//TCB
typedef struct {
  void* d;
  void (*f)(void*);
} TCB;

//Function prototypes
void TrainCom (void* data);
void SwitchControl (void* data);
void NorthTrain (void* data);
void WestTrain (void* data);
void EastTrain (void* data);
void Schedule (void* data);
void TrainInit (trainComData* tcd,   switchControlData* scd, northTrainData* ntd, 
                eastTrainData* etd,  westTrainData* wtd,     scheduleData* sd, 
                bool* north,         bool* east,             bool* west, 
                bool* gridlock,      bool* trainPresent,     unsigned int* trainSize, 
                unsigned int* globalCount);

#endif