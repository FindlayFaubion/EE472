
#include "stdbool.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "functions.h"

// The error routine that is called if the driver library encounters an error.
#ifdef DEBUG
void __error__(char *pcFilename, unsigned long ulLine){}
#endif

//Initialize globals
bool north = false;
bool east = false;
bool west = false;
bool gridlock = false;
bool trainPresent = false;
unsigned int trainSize = 0;
unsigned int globalCount = 0;
int seed = 1;
northTrainData ntd;
eastTrainData etd;
westTrainData wtd;
switchControlData scd;
scheduleData sd;

int main()
{

      unsigned long ulPeriod;

    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

    // Activate the display and display "PWM DEMO"
    RIT128x96x4Init(1000000);
   /*
    //Set PWM Divide Ratio to 1
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

    //Set Device: PWM0 Enabled
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
   
    //Set GPIO Port: G Enabled
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
   
    //Tell Port G, Pin 1, to take input from PWM 0
    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);
   
    //Set a 440 Hz frequency as u1Period
    ulPeriod = SysCtlClockGet() / 100;
   
    //Configure PWM0 in up-down count mode, no sync to clock
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    //Set u1Period (440 Hz) as the period of PWM0
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod);

    //Set PWM0, output 1 to a duty cycle of 1/8
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ulPeriod / 16);

    //Activate PWM0
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);   
    int d;
    while(1)
    {
      PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod);
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
      for(d=100000; d>0; d--);
      PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod*2);
     for(d=100000; d>0; d--);
      PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, false);
      for(d=100000; d>0; d--);
     for(d=100000; d>0; d--);
    }
 */
  //Initialize the train control block 
  std_tcb taskArray[6];

  // Train com
  taskArray[0].x = TrainCom;
  taskArray[0].y = (void*) &ntd;
  
  // Switch control
  unsigned char SCLight [2];
  SetSCData(SCLight);
  taskArray[1].x = SwitchControl;
  taskArray[1].y = (void*) &scd;
    
    // North train
  unsigned char NTLight [6];
  unsigned char NTSound [20];
  SetNTData(NTLight, NTSound); 
  taskArray[2].x = NorthTrain;
  taskArray[2].y = (void*)&ntd;
  
    // East train
  unsigned char ETLight[8];
  unsigned char ETSound[26];
  SetETData(ETLight, ETSound);
  taskArray[3].x = EastTrain;  
  taskArray[3].y = (void*)&etd;
  
  // West train
  unsigned char WTLight[4];
  unsigned char WTSound[14];
  SetWTData(WTLight, WTSound);
  taskArray[4].x = WestTrain;
  taskArray[4].y = (void*)&wtd;
  
  // Schedule
  SetSData();
  taskArray[5].x = Schedule;
  taskArray[5].y = (void*) &sd;
  
  // Loop the task control block
  int i;
  while(1) {  
    for(i = 0;i < 6;i++) {
      (*taskArray[i].x)(taskArray[i].y);
    }
    i = 0;
  }
}
