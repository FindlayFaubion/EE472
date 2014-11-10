
#include "functions.h"

// The error routine that is called if the driver library encounters an error.
#ifdef DEBUG
void __error__(char *pcFilename, unsigned long ulLine){}
#endif

// Initialize globals
bool north = FALSE;
bool east = FALSE;
bool west = FALSE;
bool gridlock = FALSE;
bool trainPresent = FALSE;
unsigned int trainSize = 0;
unsigned int globalCount = 0;
int seed = 1;
northTrainData ntd;
eastTrainData etd;
westTrainData wtd;

int main()
{
    // Initialize system clock
    //SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
    //Initialize the train control block 
    
    tcb task_queue [6];
    int test[6];
    test[0] = 2;
    // Train com
    task_queue[0].f = TrainCom;
    task_queue[0].d = (void*) &ntd;
    
    // Switch control
    task_queue[1].f = SwitchControl;
    task_queue[1].d = (void*) &ntd;
   
    //Setting up northTrain light and sound arrays
    int NTLight [] = {1, 1, 1, 0, 0, 0};
    ntd.light = NTLight;
    int NTSound [] = {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
    ntd.sound = NTSound;
    
    // North train
    task_queue[2].f = NorthTrain;
    task_queue[2].d = (void*)&ntd;

    //Setting up eastTrain light and sound arrays
    int ETLight[] = {1, 1, 1, 1, 0, 0, 0, 0,};
    etd.light = ETLight;
    int ETSound[] = {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
    etd.sound = ETSound;
    
    // East train
    task_queue[3].f = EastTrain;
    task_queue[3].d = (void*)&etd;

    //Setting up westTrain light and sound arrays
    int WTLight[] = {1, 1, 0, 0};
    wtd.light = WTLight;
    int WTSound[] = {1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0};
    wtd.sound = WTSound;

    // West train
    task_queue[4].f = WestTrain;
    task_queue[4].d = (void*)&wtd;
    
    // Schedule
    task_queue[5].f = Schedule;
    task_queue[5].d = (void*) &ntd;

    //initialize OLED display
    RIT128x96x4Init(1000000);
     
    RIT128x96x4StringDraw("test", 30, 24, 15);
    // Loop the task control block
char val = north + ASCII_OFFSET;
RIT128x96x4StringDraw(&val, 30, 24, 15);
  int i;
  while(1) {  
    for(i = 0;i < 6;i++) {
      (*task_queue[i].f)(task_queue[i].d);
    }
  }
}
