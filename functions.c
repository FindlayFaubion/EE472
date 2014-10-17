
#include "functions.h"



void TrainCom(void* d) {
    TrainComData* tcd = (TrainComData*) d;
//  
// rand = RandomInt(0, 2);
// if (rand == 0)
// {
//   west = true;
// }
// else if (rand == 1)
// {
//   north = true;
// }
// else 
// {
//   east = true;
// }
// 
// rand2 = RandomInt(2, 9);
// trainSize = rand2;
//  
}
  

void SwitchControl(void* d) {  
    SwitchControlData* sc = (SwitchControlData*) d;
//  if (trainPresent || !gridlockChecked) 
//  {
//    gridlockedChecked = true;
//    //flash alarm
//  }
//  
//  rand = RandomInt(-2, 2);
//  if (rand < 0)
//  {
//    gridlock = true;
//    trainPresent = false;
//    //alarm
//    
//   
//    //DELAY for -2*n minutes
//    //complete routing
//    gridlock = false;
//  }
//  else 
//  {
//    traversaltime = 0.1 * trainsize; //conversion needed
//    //display direction
//    //after traversaltime 
//      //north = false;
//      //west = false;
//      //east = false;
//      //trainPresent = false;
//      //trainSize = 0;
//  }
}


void NorthTrain(void* d) {
    NorthTrainData* ntd = (NorthTrainData*) d;
    if (d.north) {
        //dothings
    }
//  
//    if (North && )
}
   

void EastTrain(void* d) {
    EastTrainData* etd = (EastTrainData*) d;
//  
}


void WestTrain(void* d) {
    WestTrainData* wtd = (WestTrainData*) d;
//  
}


void Schedule(void* d) {
    ScheduleData* sd = (ScheduleData*) d;
//  globalCount = globalCount + 1;
//  //display globalCount: use /,% to get necessary numbers to display
//  //delay ***ms
//  
}

// Initialize the task data to point to global state variables
void TrainInit(TrainComData* TCdata, SwitchControlData* SCdata, NorthTrainData* NTdata, 
                EastTrainData* ETdata, WestTrainData* WTdata, ScheduleData* Sdata, 
                bool* north, bool* east, bool* west, bool* gridlock, bool* trainPresent, 
                unsigned int* trainSize, unsigned int* globalCount) {
    // north train
    TCdata->north = north;
    TCdata->west = west;
    TCdata->east = east;
    TCdata->gridlock = gridlock;
    TCdata->trainPresent = trainPresent;
    TCdata->trainSize = trainSize;
    TCdata->globalCount = globalCount;
  
    // switch control
    SCdata->north = north;
    SCdata->west = west;
    SCdata->east = east;
    SCdata->gridlock = gridlock;
    SCdata->trainPresent = trainPresent;
    SCdata->trainSize = trainSize;
    SCdata->globalCount = globalCount;
  
    // north data
    NTdata->north = north;
    NTdata->gridlock = gridlock;
    NTdata->trainPresent = trainPresent;
    NTdata->trainSize = trainSize;
    NTdata->globalCount = globalCount;
    
    // east data 
    ETdata->east = east;
    ETdata->gridlock = gridlock;
    ETdata->trainPresent = trainPresent;
    ETdata->trainSize = trainSize;
    ETdata->globalCount = globalCount;
    
    // west data
    WTdata->west = west;
    WTdata->gridlock = gridlock;
    WTdata->trainPresent = trainPresent; 
    WTdata->trainSize = trainSize;
    WTdata->globalCount = globalCount;
    
    // schedule data 
    Sdata->globalCount = globalCount;
}

// Generate a random number between the given bounds
int RandomInt(int low, int high) { //code for RandomInt taken from EE472 website
  double randNum = 0.0;
  int multiplier = 2743;
  int addOn = 5923;
  int seed = 1;
  double max = 0xFFFF-1;
  
  int retVal = 0;
  
  if (low > high)
    retVal = RandomInt(high, low);
  else
  {
    seed = seed*multiplier + addOn;
    randNum = seed;
    
    if (randNum <0)
    {
      randNum = randNum + max;
    }
    
    randNum = randNum/max;
    
    retVal =  ((int)((high-low+1)*randNum))+low;
  }
  
  return retVal;
}

        