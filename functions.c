
#include "functions.h"

void TrainCom(void* d) {
  
  // create a random value between 0 and 2. set a direction TRUE depending on the generated value
  int rand = RandomInt(0, 2);
  if (rand == 0) 
  {
    west = TRUE;
  }
  else if (rand == 1)
  {
    north = TRUE;
  }
  else 
  {
    east = TRUE;
  }
  
  // create a randome value for trainSize between 2 and 9
  trainSize = RandomInt(2, 9);
}
  

void SwitchControl(void* d) {  
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
    if (north && trainPresent) {
       
        //Makes display flash; light is filled with 1 and 0 dictating if the display is on or off.  As global count increments it cycles
        //through the 1's and 0's making the display flash
        if (ntd->light[globalCount % 6]){ //Do something about magic number 6
            char display[32] = "NorthTrain ";
            display[11] = (char) trainSize + ASCII_OFFSET; //Not sure if this works
            RIT128x96x4StringDraw(display, 30, 24, 15);
        }else{
            RIT128x96x4Clear();
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
            char display[32] = "EastTrain ";
            display[10] = (char) trainSize + ASCII_OFFSET; //Not sure if this works
            RIT128x96x4StringDraw(display, 30, 24, 15);
        }else{
            RIT128x96x4Clear();
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


void WestTrain(void* d) {
    westTrainData* wtd = (westTrainData*) d;
    if (west && trainPresent) {
        
        //Makes display flash; light is filled with 1 and 0 dictating if the display is on or off.  As global count increments it cycles
        //through the 1's and 0's making the display flash
        if (wtd->light[globalCount % 4]){ //Do something about magic number 6
            char display[32] = "WestTrain ";
            display[10] = (char) trainSize + ASCII_OFFSET; //Not sure if this works
            RIT128x96x4StringDraw(display, 30, 24, 15);
        }else{
            RIT128x96x4Clear();
        }
        
        if (wtd->i < 20 && wtd->sound[wtd->i]) { //This will throw a null pointer at i = 20 unless i<20 is evaluated first, not sure on syntax
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
    
    // Extract global count into characters
    char a[6] = "";
    int global = globalCount;
    for (int i = 0; i <6; i++) {
      a[5-i] = global % 10 + ASCII_OFFSET;
      global = global / 10;
    }
    
    // Display global count
    RIT128x96x4StringDraw(a, 20, 24, 15);

    // Delay execution
    for (int i = 0; i < 10000; i++){
      //delay 
    }
}

// Generate a random number between the given bounds
// code for RandomInt taken from EE472 website
int RandomInt(int low, int high) { 
  double randNum = 0.0;
  int multiplier = 2743;
  int addOn = 5923;
  double max = 0xFFF-1;
  
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