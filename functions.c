
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "stdbool.h"
#include "functions.h"
#include <math.h>


// Sets the train direction, and train size
void TrainCom(void* d) {
  if (!trainPresent && !gridlock) {
      // generate a random direction
      int dir = RandomInt(0, 2);
      if (dir == 0) 
      {
        west = true;
      }
      else if (dir == 1)
      {
        north = true;
      }
      else 
      {
        east = true;
      }
      // create a random value for trainSize between 2 and 9
      trainSize = RandomInt(TRAIN_SIZE_MIN, TRAIN_SIZE_MAX);
      // update train status
      trainPresent = true;
    }
}

// Handles the routing of trains
void SwitchControl(void* d) {  
    switchControlData* scd = (switchControlData*) d;
    if (trainPresent || !(scd->gridlockChecked)) {

        if(!scd->gridlockChecked) {
            // check for a gridlock
            int n = RandomInt(-2, 2);
            if (n < 0) {
                scd->delay = -0.2 * n * GLOBAL_CNT_PER_MIN;
                gridlock = true;
                trainPresent = false;
            } else {
                scd->delay = 0.1 * trainSize * GLOBAL_CNT_PER_MIN;
            }
            // update state 
            scd->gridlockChecked = true; 
        }
    }
    // behavior if gridlocked
    if (gridlock) {
        // issue visual alarm (0.5 sec on, 0.5 off) 
        if (scd->light[globalCount % 2]){ 
            char display[32] = "GRIDLOCK \0";
            RIT128x96x4StringDraw(display, 30, 10, 15);
        }else{
            RIT128x96x4StringDraw("            \0", 30, 24, 15);
        }
        // reset if delay has elapsed
        if (scd->i > scd->delay) {
            gridlock = false;
            scd->gridlockChecked = false;
            scd->i = 0;
        }
    // behavior if not gridlocked
    } else {
        // display train direction (always on)
        RIT128x96x4StringDraw(GetDirection(), 30, 10, 15);
        // reset if delay has elapsed
        if (scd->i > scd->delay) {
            trainPresent = false;
            scd->gridlockChecked = false;
            west = false;
            east = false;   
            north = false;
            trainSize = 0;
            scd->i = 0;
        }
    }
    scd->i++;
}


void NorthTrain(void* d) {
  northTrainData* ntd = (northTrainData*) d;
  if (north && trainPresent) {
    
    //Makes display flash; light is filled with 1 and 0 dictating if the display is on or off.  As global count increments it cycles
    //through the 1's and 0's making the display flash
    if (ntd->light[globalCount % 6]){ //Do something about magic number 6
        char display[32] = "NorthTrain   \0";
        display[11] = (char) trainSize + ASCII_OFFSET; //Not sure if this works
        RIT128x96x4StringDraw(display, 30, 24, 15);
    }else{
        RIT128x96x4StringDraw("            \0", 30, 24, 15);
    }
    
    if (ntd->i < 20 && ntd->sound[ntd->i]) { //This will throw a null pointer at i = 20 unless i<20 is evaluated first, not sure on syntax
      //Turn beeper on
    }else{
      //turn beeper off
    }
    ntd->i++;
    
  }else{
    ntd->i = 0; //reseting i to repeat pattern correctly on next train
  }
}


void EastTrain(void* d) {
  eastTrainData* etd = (eastTrainData*) d;
  if (east && trainPresent) {
    
    //Makes display flash; light is filled with 1 and 0 dictating if the display is on or off.  As global count increments it cycles
    //through the 1's and 0's making the display flash
    if (etd->light[globalCount % 8]){ //Do something about magic number 6
      char display[32] = "EastTrain  \0";
      display[10] = (char) trainSize + ASCII_OFFSET; //Not sure if this works
     RIT128x96x4StringDraw(display, 30, 24, 15);
    }else{
      RIT128x96x4StringDraw("            \0", 30, 24, 15);
    }
    
    if (etd->i < 26 && etd->sound[etd->i]) { //This will throw a null pointer at i = 20 unless i<20 is evaluated first, not sure on syntax
      //Turn beeper on
    }else{
      //turn beeper off
    }
    
    etd->i++;
    
  }else{
    etd->i = 0; //reseting i to repeat pattern correctly on next train
  }
}

// Handle west train behavior
void WestTrain(void* d) {
  westTrainData* wtd = (westTrainData*) d;
  static char display[] = "WestTrain    \0";
  if (west && trainPresent) {
    //Makes display flash
    if (wtd->light[globalCount % 4]){ 
        display[10] = (char) trainSize + ASCII_OFFSET; 
        RIT128x96x4StringDraw(display, 30, 24, 15);
    }else{
        RIT128x96x4StringDraw("            \0", 30, 24, 15);
    }
    
    if (wtd->i < 20 && wtd->sound[wtd->i]) { 
      //Turn beeper on
    }else{
      //turn beeper off
    }
    
    wtd->i++;
    
  }else{
    wtd->i = 0; //reseting i to repeat pattern correctly on next train
  }
}


void Schedule(void* d) {
  // Increment global count
  globalCount++; 
  //RIT128x96x4Clear();
  // Extract global count into characters
  char a[8] = "      \0";
  int global = globalCount;
  int i = 5;
  for (; i >=0 && global != 0; i--) {
    a[i] = global % 10 + ASCII_OFFSET;
    global = global / 10;
  }
  while(i >= 0) {
    a[i] = ' ';
    i--;
  }
  // Display global count
  RIT128x96x4StringDraw(a, 20, 50, 15);
  
  // Delay execution
  for (int i = 0; i < 500000; i++){
    //delay 
  }
  //RIT128x96x4Clear();
}

void SetNTData(unsigned char* NTLight, unsigned char* NTSound) {
  
  NTLight[0] = 1;
  NTLight[1] = 1;
  NTLight[2] = 1;
  NTLight[3] = 0;
  NTLight[4] = 0;
  NTLight[5] = 0;
  
  NTSound[0] = 1;
  NTSound[1] = 1;
  NTSound[2] = 1;
  NTSound[3] = 1;
  NTSound[4] = 0;
  NTSound[5] = 0;
  NTSound[6] = 1;
  NTSound[7] = 1;
  NTSound[8] = 1;
  NTSound[9] = 1;
  NTSound[10] = 0;
  NTSound[11] = 0;
  NTSound[12] = 1;
  NTSound[13] = 1;
  NTSound[14] = 0;
  NTSound[15] = 0;
  NTSound[16] = 1;
  NTSound[17] = 1;
  NTSound[18] = 0;
  NTSound[19] = 0;

  ntd.light = NTLight;
  ntd.sound = NTSound;
}

void SetETData(unsigned char* ETLight, unsigned char* ETSound) {
  ETLight[0] = 1;
  ETLight[1] = 1;
  ETLight[2] = 1;
  ETLight[3] = 1;
  ETLight[4] = 0;
  ETLight[5] = 0;
  ETLight[6] = 0;
  ETLight[7] = 0;
  
  ETSound[0] = 1;
  ETSound[1] = 1;
  ETSound[2] = 1;
  ETSound[3] = 1;
  ETSound[4] = 0;
  ETSound[5] = 0;
  ETSound[6] = 1;
  ETSound[7] = 1;
  ETSound[8] = 1;
  ETSound[9] = 1;
  ETSound[10] = 0;
  ETSound[11] = 0;
  ETSound[12] = 1;
  ETSound[13] = 1;
  ETSound[14] = 1;
  ETSound[15] = 1;
  ETSound[16] = 0;
  ETSound[17] = 0;
  ETSound[18] = 1;
  ETSound[19] = 1;
  ETSound[20] = 0;
  ETSound[21] = 0;
  ETSound[22] = 1;
  ETSound[23] = 1;
  ETSound[24] = 0;
  ETSound[25] = 0;

    etd.light = ETLight;
  etd.sound = ETSound;
}

void SetWTData(unsigned char* WTLight, unsigned char* WTSound) {
  WTLight[0] = 1;
  WTLight[1] = 1;
  WTLight[2] = 0;
  WTLight[3] = 0;
  
  WTSound[0] = 1;
  WTSound[1] = 1;
  WTSound[2] = 1;
  WTSound[3] = 1;
  WTSound[4] = 0;
  WTSound[5] = 0;
  WTSound[6] = 1;
  WTSound[7] = 1;
  WTSound[8] = 0;
  WTSound[9] = 0;
  WTSound[10] = 1;
  WTSound[11] = 1;
  WTSound[12] = 0;
  WTSound[13] = 0;
  wtd.light = WTLight;
  wtd.sound = WTSound;
}

void SetSCData(unsigned char* SCLight) {
    SCLight[0] = 1;
    SCLight[1] = 0;
   
    scd.light = SCLight;
    scd.gridlockChecked = false;
    scd.delay = 0;
    scd.i = 0;
}

// Generate a pseudo random number between the given bounds
// code for RandomInt taken from EE472 website
int RandomInt(int low, int high) { 
    int multiplier = 2743;
    int addOn = 5923;

    int retVal = 0;

    if (low > high) {
        retVal = RandomInt(high, low);
    } else {
        seed = seed*multiplier + addOn;
        if (seed < 0) {
            seed = -1 * seed;
        }
    }
    retVal = seed % (high+1 - low) + low;
    return retVal; 
}

// Returns the direction of the current train as a string
char* GetDirection() {
    if (west) {
        return "West        \0";
    } else if (east) {
        return "East        \0";
    } else {
        return "North       \0";
    }
}
