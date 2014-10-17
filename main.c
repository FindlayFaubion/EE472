
#include "functions.h"

// The error routine that is called if the driver library encounters an error.
#ifdef DEBUG
void __error__(char *pcFilename, unsigned long ulLine){}
#endif

// Global state variables
bool north = FALSE;
bool east = FALSE;
bool west = FALSE;
bool gridlock = FALSE;
bool trainPresent = FALSE;
unsigned int trainSize = 0;
unsigned int globalCount = 0;

// Global task data
trainComData tcd;
switchControlData scd;
northTrainData ntd;
eastTrainData etd;
westTrainData wtd;
scheduleData sd;

int main()
{
  
  //Initialize task data to point to global state variables
  TrainInit(&tcd, &scd, &ntd, &etd, &wtd, &sd, &north, &east, &west, &gridlock, &trainPresent, 
            &trainSize, &globalCount);
  

  //Initialize the train control block 
  TCB* task[6];
  
  TCB trainComTCB;
  trainComTCB.f = &TrainCom;
  trainComTCB.d = (void*)&tcd;
  task[0] = &trainComTCB;
  
  TCB switchControlTCB;
  switchControlTCB.f = &SwitchControl;
  switchControlTCB.d = (void*)&scd;
  task[1] = &switchControlTCB;
  
  TCB northTrainTCB;
  northTrainTCB.f = &NorthTrain;
  northTrainTCB.d = (void*)&ntd;
  task[2] = &northTrainTCB;
  
  TCB eastTrainTCB;
  eastTrainTCB.f = &EastTrain;
  eastTrainTCB.d = (void*)&etd;
  task[3] = &eastTrainTCB;
  
  TCB westTrainTCB;
  westTrainTCB.f = &WestTrain;
  westTrainTCB.d = (void*)&wtd;
  task[4] = &westTrainTCB;
  
  TCB scheduleTCB;
  scheduleTCB.f = &Schedule;
  scheduleTCB.d = (void*)&sd;
  task[5] = &scheduleTCB;
  
  // Loop the task control block
  int i;
  while(1) {  
    for(i = 0;i < 6;i++) {
      (*task[i] -> f)((void*)task[i]->d);
    }
  }
}
