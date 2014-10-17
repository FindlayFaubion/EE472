
#include "functions.h"

// The error routine that is called if the driver library encounters an error.
#ifdef DEBUG
void __error__(char *pcFilename, unsigned long ulLine){}
#endif


bool north = FALSE;
bool east = FALSE;
bool west = FALSE;
bool gridlock = FALSE;
bool trainPresent = FALSE;
unsigned int trainSize = 0;
unsigned int globalCount = 0;

int main()
{
  
  ////////////////////////////////////////////
  TrainComData TCdata;
  SwitchControlData SCdata;
  NorthTrainData NTdata;
  EastTrainData ETdata;
  WestTrainData WTdata;
  ScheduleData Sdata;
  TrainInit(&TCdata, &SCdata, &NTdata, &ETdata, &WTdata, &Sdata, &north, &east, &west, &gridlock, &trainPresent, &trainSize, &globalCount);
  ////////////////////////////////////////////

  TCB* task[6];
  
  TCB TrainComTCB;
  TrainComTCB.f = &TrainCom;
  TrainComTCB.d = (void*)&TCdata;
  task[0] = &TrainComTCB;
  
  TCB SwitchControlTCB;
  SwitchControlTCB.f = &SwitchControl;
  SwitchControlTCB.d = (void*)&SCdata;
  task[1] = &SwitchControlTCB;
  
  TCB NorthTrainTCB;
  NorthTrainTCB.f = &NorthTrain;
  NorthTrainTCB.d = (void*)&NTdata;
  task[2] = &NorthTrainTCB;
  
  TCB EastTrainTCB;
  EastTrainTCB.f = &EastTrain;
  EastTrainTCB.d = (void*)&ETdata;
  task[3] = &EastTrainTCB;
  
  TCB WestTrainTCB;
  WestTrainTCB.f = &WestTrain;
  WestTrainTCB.d = (void*)&WTdata;
  task[4] = &WestTrainTCB;
  
  TCB ScheduleTCB;
  ScheduleTCB.f = &Schedule;
  ScheduleTCB.d = (void*)&Sdata;
  task[5] = &ScheduleTCB;
  
  ///////////////////////////////////////////////
  int i;
  while(1) {  
    for(i = 0;i < 6;i++) {
      (*task[i] -> f)((void*)task[i]->d);
    }
  }
}
