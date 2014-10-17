//#ifndef HEADER_F 
//#define HEADER_F

// test commit 
//another test

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"

//Include hardware memory map, GPIO driver, and PWM driver
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"


enum myBool { FALSE = 0, TRUE = 1 };
typedef enum myBool bool;

//TrainComdata
typedef struct{
  bool* north;
  bool* east;
  bool* west;
  bool* gridlock;
  bool* trainPresent;
  unsigned int* trainSize;
  unsigned int* globalCount;
} TrainComData;

//SwitchControldata
typedef struct{
  bool* north;
  bool* east;
  bool* west;
  bool* gridlock;
  bool* trainPresent;
  unsigned int* trainSize;
  unsigned int* globalCount;
} SwitchControlData;

//NorthTraindata
typedef struct{
  bool* north;
  bool* gridlock;
  bool* trainPresent;
  unsigned int* trainSize;
  unsigned int* globalCount;
} NorthTrainData;

//EastTraindata
typedef struct{
  bool* east;
  bool* gridlock;
  bool* trainPresent;
  unsigned int* trainSize;
  unsigned int* globalCount;
} EastTrainData;

//WestTraindata
typedef struct{
  bool* west;
  bool* gridlock;
  bool* trainPresent;
  unsigned int* trainSize;
  unsigned int* globalCount;
} WestTrainData;

//Scheduledata
typedef struct{
  unsigned int* globalCount;
} ScheduleData;

//TCB
typedef struct{
  void* d;
  void (*f)(void*);
} TCB;

void TrainCom(void* data);
void SwitchControl(void* data);
void NorthTrain(void* data);
void WestTrain(void* data);
void EastTrain(void* data);
void Schedule(void* data);
void TrainInit(TrainComData* TCdata, SwitchControlData* SCdata, NorthTrainData* NTdata, 
                EastTrainData* ETdata, WestTrainData* WTdata, ScheduleData* Sdata, 
                bool* north, bool* east, bool* west, bool* gridlock, bool* trainPresent, 
                unsigned int* trainSize, unsigned int* globalCount);

//#endif