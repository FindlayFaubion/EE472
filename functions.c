
#include "functions.h"

void TrainCom(void* d) {
    trainComData* tcd = (trainComData*) d;
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
    switchControlData* sc = (switchControlData*) d;
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
    northTrainData* ntd = (northTrainData*) d;
    if () {
        //dothings
    }
//  
//    if (North && )
}
   

void EastTrain(void* d) {
    eastTrainData* etd = (eastTrainData*) d;
//  
}


void WestTrain(void* d) {
    westTrainData* wtd = (westTrainData*) d;
//  
}


void Schedule(void* d) {
    scheduleData* sd = (scheduleData*) d;
//  globalCount = globalCount + 1;
//  //display globalCount: use /,% to get necessary numbers to display
//  //delay ***ms
//  
}

// Initialize the task data to point to global state variables
void TrainInit(trainComData* tcd, switchControlData* scd, northTrainData* ntd, 
                eastTrainData* etd, westTrainData* wtd, scheduleData* sd, 
                bool* north, bool* east, bool* west, bool* gridlock, bool* trainPresent, 
                unsigned int* trainSize, unsigned int* globalCount) {
    // train communication
    tcd->north = north;
    tcd->west = west;
    tcd->east = east;
    tcd->gridlock = gridlock;
    tcd->trainPresent = trainPresent;
    tcd->trainSize = trainSize;
    tcd->globalCount = globalCount;
  
    // switch control
    scd->north = north;
    scd->west = west;
    scd->east = east;
    scd->gridlock = gridlock;
    scd->trainPresent = trainPresent;
    scd->trainSize = trainSize;
    scd->globalCount = globalCount;
  
    // north data
    ntd->north = north;
    ntd->gridlock = gridlock;
    ntd->trainPresent = trainPresent;
    ntd->trainSize = trainSize;
    ntd->globalCount = globalCount;
    
    // east data 
    etd->east = east;
    etd->gridlock = gridlock;
    etd->trainPresent = trainPresent;
    etd->trainSize = trainSize;
    etd->globalCount = globalCount;
    
    // west data
    wtd->west = west;
    wtd->gridlock = gridlock;
    wtd->trainPresent = trainPresent; 
    wtd->trainSize = trainSize;
    wtd->globalCount = globalCount;
    
    // schedule data 
    sd->globalCount = globalCount;
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

        