
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "stdbool.h"
#include "functions.h"
#include <math.h>


// Handle timing 
void Schedule(void* d) {
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0xFF); //write GPIO LED0 on for testing

      // GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_59, 0xF8);
      //scheduleData* sd = (scheduleData*) d;
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
      RIT128x96x4StringDraw(a, 20, 50, OLED_LEVEL);
      
      // Delay execution
      SysCtlDelay(SysCtlClockGet() / 6); // 6 produces 500 ms delay
      
      // update queue
      for (int i = 0; i < 5; i++) {
        (*taskArray[i].x)(taskArray[i].y);
      }
}

// Startup function
void Startup() {
    // Set the clocking to run directly from the crystal
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

    // Initialize the OLED
    RIT128x96x4Init(OLED_FREQ);

    // Initialize PWM buzzer
    InitBuzzer(FREQUENCY);
    
    // Initialize task queue
        // Train com
    taskArray[0].x = TrainCom;
    taskArray[0].y = (void*) &ntd; //data not actually used

        // Switch control
    static unsigned char SCLight [2];
    SetSCData(SCLight);
    taskArray[1].x = SwitchControl;
    taskArray[1].y = (void*) &scd;
    
        // Serial comm
    taskArray[2].x = SerialCom;
    taskArray[2].y = (void*) &ntd; // data unused
    
   // Initialize train data for all directions
        // North train
    static unsigned char NTLight [NTLIGHT_LEN]; 
    static unsigned char NTSound [NTSOUND_LEN]; 
    SetNTData(NTLight, NTSound); 
    
        // South train
    static unsigned char STLight [STLIGHT_LEN]; 
    static unsigned char STSound [STSOUND_LEN]; 
    SetSTData(STLight, STSound); 

        // East train 
    static unsigned char ETLight[ETLIGHT_LEN];  
    static unsigned char ETSound[ETSOUND_LEN]; 
    SetETData(ETLight, ETSound);

        // West train
    static unsigned char WTLight[WTLIGHT_LEN];  
    static unsigned char WTSound[WTSOUND_LEN]; 
    SetWTData(WTLight, WTSound);
  
    static tcb* trains[4];
    trains[0] = &ntd;
    trains[1] = &etd;
    trains[2] = &std; 
    

//    Schedule
//    SetSData();
//    taskArray[5].x = Schedule;
//    taskArray[5].y = (void*) &sd;
}

// Sets the train direction, and train size
void TrainCom(void* d) {
  if (!trainPresent && !gridlock) {
      // generate a random direction
      int dir = RandomInt(0, 3);
      // north 
      if (dir == 0) {
       
      // east 
      } else if (dir == 1) {
      
      // south 
      } else if(dir == 2) {
       
      // west 
      } else {
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
        RIT128x96x4StringDraw(CLEAR_SCREEN, 30, 24, OLED_LEVEL);
        // issue visual alarm (0.5 sec on, 0.5 off) 
        if (scd->light[globalCount % 2]){ 
            char display[32] = "GRIDLOCK \0";
            RIT128x96x4StringDraw(display, 30, 10, OLED_LEVEL);
        }else{
            RIT128x96x4StringDraw(CLEAR_SCREEN, 30, 10, OLED_LEVEL);
        }
        // reset if delay has elapsed
        if (scd->i > scd->delay) {
            west = false;
            east = false;   
            north = false;
            gridlock = false;
            scd->gridlockChecked = false;
            scd->i = 0;
        }
    // train is present and not gridlocked
    } else {
        // display train direction (always on)
        RIT128x96x4StringDraw(GetDirection(), 30, 10, OLED_LEVEL);
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

// Handle current train behavior
void CurrentTrain(void* d) {
    currentTrainData* ctd = (currentTrainData*) d;
    if (ntd->light[globalCount % ctd.lightlen]){ 
        char display[32];
        display[0] = GetDirection();
        //display[11] = (char) trainSize + ASCII_OFFSET;
        display[8] = "\0";
        RIT128x96x4StringDraw(display, 30, 24, OLED_LEVEL);
    }else{
        RIT128x96x4StringDraw(CLEAR_SCREEN, 30, 24, OLED_LEVEL);
    }
    
    if (ctd->i < ctd.soundlen && ctd->sound[ctd->i]) { 
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
    }else{
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, false);
    }
    ctd->i++;
}
// Handle north train behavior
void NorthTrain(void* d) {
  northTrainData* ntd = (northTrainData*) d;
  if (north && trainPresent) {
    if (ntd->light[globalCount % NTLIGHT_LEN]){ 
        char display[32] = "NorthTrain   \0";
        display[11] = (char) trainSize + ASCII_OFFSET;
        RIT128x96x4StringDraw(display, 30, 24, OLED_LEVEL);
    }else{
        RIT128x96x4StringDraw(CLEAR_SCREEN, 30, 24, OLED_LEVEL);
    }
    
    if (ntd->i < NTSOUND_LEN && ntd->sound[ntd->i]) { 
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
    }else{
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, false);
    }
    ntd->i++;
    
  }else{
    ntd->i = 0; 
  }
}

// Handle east train behavior
void EastTrain(void* d) {
  eastTrainData* etd = (eastTrainData*) d;
  if (east && trainPresent) {
    if (etd->light[globalCount % ETLIGHT_LEN]){ 
      char display[32] = "EastTrain   \0";
      display[10] = (char) trainSize + ASCII_OFFSET; 
     RIT128x96x4StringDraw(display, 30, 24, OLED_LEVEL);
    }else{
      RIT128x96x4StringDraw(CLEAR_SCREEN, 30, 24, OLED_LEVEL);
    }
    
    if (etd->i < ETSOUND_LEN && etd->sound[etd->i]) { 
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
    }else{
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, false);
    }
    
    etd->i++;
    
  }else{
    etd->i = 0; 
  }
}

// Handle south train behavior
void SouthTrain(void* d) {
  southTrainData* std = (southTrainData*) d;
  if (south && trainPresent) {
    if (std->light[globalCount % STLIGHT_LEN]){ 
      char display[32] = "SouthTrain   \0";
      display[11] = (char) trainSize + ASCII_OFFSET; 
     RIT128x96x4StringDraw(display, 30, 24, OLED_LEVEL);
    }else{
      RIT128x96x4StringDraw(CLEAR_SCREEN, 30, 24, OLED_LEVEL);
    }
    
    if (std->i < STSOUND_LEN && std->sound[std->i]) { 
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
    }else{
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, false);
    }
    
    std->i++;
    
  }else{
    std->i = 0; 
  }
}

// Handle west train behavior
void WestTrain(void* d) {
  westTrainData* wtd = (westTrainData*) d;
  if (west && trainPresent) {
    //Makes display flash
    if (wtd->light[globalCount % WTLIGHT_LEN]){ 
        char display[32] = "WestTrain    \0";
        display[10] = (char) trainSize + ASCII_OFFSET; 
        RIT128x96x4StringDraw(display, 30, 24, OLED_LEVEL);
    }else{
        RIT128x96x4StringDraw(CLEAR_SCREEN, 30, 24, OLED_LEVEL);
    }
    
    if (wtd->i < WTSOUND_LEN && wtd->sound[wtd->i]) { 
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
    }else{
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, false);
    }
    
    wtd->i++;
    
  }else{
    wtd->i = 0; //reseting i to repeat pattern correctly on next train
  }
}

// Serial communications
void SerialCom(void* d) {
    
}

// set data for North Train
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

// set data for East Train
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

// Set data South train
void SetSTData(void SetSTData(unsigned char* STLight, unsigned char* STSound) {
  STLight[0] = 1;
  STLight[1] = 1;
  STLight[2] = 0;
  STLight[3] = 0;
  
  STSound[0] = 1;
  STSound[1] = 1;
  STSound[2] = 1;
  STSound[3] = 1;
  STSound[4] = 0;
  STSound[5] = 0;
  STSound[6] = 1;
  STSound[7] = 1;
  STSound[8] = 1;
  STSound[9] = 1;
  STSound[10] = 0;
  STSound[11] = 0;
  STSound[12] = 1;
  STSound[13] = 1;
  STSound[14] = 0;
  STSound[15] = 0;
  STSound[16] = 1;
  STSound[17] = 1;
  STSound[18] = 0;
  STSound[19] = 0;
  STSound[20] = 1;
  STSound[21] = 1;
  STSound[22] = 0;
  STSound[23] = 0;

  std.light = STLight;
  std.sound = STSound;
}


// set data for West Train
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

// set data for Switch Control
void SetSCData(unsigned char* SCLight) {
    SCLight[0] = 1;
    SCLight[1] = 0;
   
    scd.light = SCLight;
    scd.gridlockChecked = false;
    scd.delay = 0;
    scd.i = 0;
}

// set data for Schedule
//void SetSData() {
 //   sd.clock_f = SysCtlClockGet();
//}

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

// Initialize the PWM buzzer on GPIOG
void InitBuzzer(int freq) {
    int ulPeriod = SysCtlClockGet() / freq; //Set the pulse period
    //Set PWM Divide Ratio to 1
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    //Set Device: PWM0 Enabled
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    //Set GPIO Port: G Enabled
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    //Tell Port G, Pin 1, to take input from PWM 0
    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);
    //Configure PWM0 in up-down count mode, no sync to clock
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
        PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    //Set PWM period
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod);
    //Set PWM0, output 1 to a duty cycle of 1/8
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ulPeriod / 16);
    //Activate PWM0
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}



