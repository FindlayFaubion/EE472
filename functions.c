
#include "functions.h"



void TrainCom(void* d) {
  trainComData* tcd = (trainComData*) d;
  bool* north = tcd->north;
  bool* east = tcd->east;
  bool* west = tcd->west;
  unsigned int* trainSize = tcd->trainSize;
  unsigned int* globalCount = tcd->globalCount;
  
  // create a random value between 0 and 2. set a direction TRUE depending on the generated value
  int rand = RandomInt(0, 2);
  if (rand == 0) 
  {
    *west = TRUE;
  }
  else if (rand == 1)
  {
    *north = TRUE;
  }
  else 
  {
    *east = TRUE;
  }
  
  // create a randome value for trainSize between 2 and 9
  unsigned int rand2 = RandomInt(2, 9);
  *trainSize = rand2;
}//Credit for random generator: Alan Ecker
  

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
    if (*ntd.north & *ntd.trainPresent == TRUE) {
        
        //Makes display flash; light is filled with 1 and 0 dictating if the display is on or off.  As global count increments it cycles
        //through the 1's and 0's making the display flash
        if (ntd.light[globalCount % 6]){ //Do something about magic number 6
            string display = "NorthTrain %d", ntd.trainSize; //Not sure if this works
            RIT128x96x4StringDraw(display, 30, 24, 15);
        }else{
            RIT128x96x4Clear();
        }
        
        if (i < 20 & ntd.sound[i]) { //This will throw a null pointer at i = 20 unless i<20 is evaluated first, not sure on syntax
            //Turn beeper on
        }else{
            //turn beeper off
        }
        
        ntd.i++
        
    }else{
        ntd.i = 0; //reseting i to repeat pattern correctly on next train
    }
}
   

void EastTrain(void* d) {
    eastTrainData* etd = (eastTrainData*) d;
    if (*etd.east & *netd.trainPresent == TRUE) {
        
        //Makes display flash; light is filled with 1 and 0 dictating if the display is on or off.  As global count increments it cycles
        //through the 1's and 0's making the display flash
        if (etd.light[globalCount % 8]){ //Do something about magic number 6
            string display = "EastTrain %d", ntd.trainSize; //Not sure if this works
            RIT128x96x4StringDraw(display, 30, 24, 15);
        }else{
            RIT128x96x4Clear();
        }
        
        if (i < 26 & etd.sound[i]) { //This will throw a null pointer at i = 20 unless i<20 is evaluated first, not sure on syntax
            //Turn beeper on
        }else{
            //turn beeper off
        }
        
        etd.i++
        
    }else{
        etd.i = 0; //reseting i to repeat pattern correctly on next train
    }
}


void WestTrain(void* d) {
    westTrainData* wtd = (westTrainData*) d;
    if (*wtd.west & *wtd.trainPresent == TRUE) {
        
        //Makes display flash; light is filled with 1 and 0 dictating if the display is on or off.  As global count increments it cycles
        //through the 1's and 0's making the display flash
        if (wtd.light[globalCount % 4]){ //Do something about magic number 6
            string display = "WestTrain %d", ntd.trainSize; //Not sure if this works
            RIT128x96x4StringDraw(display, 30, 24, 15);
        }else{
            RIT128x96x4Clear();
        }
        
        if (i < 20 & wtd.sound[i]) { //This will throw a null pointer at i = 20 unless i<20 is evaluated first, not sure on syntax
            //Turn beeper on
        }else{
            //turn beeper off
        }
        
        wtd.i++
        
    }else{
        wtd.i = 0; //reseting i to repeat pattern correctly on next train
    }
}
}


void Schedule(void* d) {
    scheduleData* sd = (scheduleData*) d;
    unsigned int* g = sd->globalCount;
    g = g + 1; //globalCount = globalCount + 1;
  //display globalCount: use /,% to get necessary numbers to display
    char a[7] = "";
    unsigned int global = *g;
    for (int i = 0; i <7; i++) {
      a[7-i] = global % 10;
      global = global / 10;
    }
   
    
    for (int i = 0; i < 10000; i++){
      //delay 
    }
     
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
    
    // Fault ISR triggered in following line, currently commented out
    
    
    //randNum = randNum/max;
    
    //retVal =  ((int)((high-low+1)*randNum))+low;
  }
  
  return retVal;
}

        