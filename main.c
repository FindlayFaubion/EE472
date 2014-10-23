
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
  test_struct testarray[6];
  std_tcb taskArray[6];
  
  testarray[0].x = &TrainCom;
  testarray[0].y = (void*) &seed;
  
  testarray[1].x = &NorthTrain;
  // Train com
  taskArray[0].x = TrainCom;
  taskArray[0].y = (void*) &ntd;
  
  // Switch control
  taskArray[1].x = SwitchControl;
  taskArray[1].y = (void*) &ntd;
  
  //Setting up northTrain light and sound arrays
  unsigned char NTLight [6];
  unsigned char NTSound [20];
  SetNTData(NTLight, NTSound); 
  ntd.light = NTLight;
  ntd.sound = NTSound;
  
  // North train
  taskArray[2].x = NorthTrain;
  taskArray[2].y = (void*)&ntd;
  
  //Setting up eastTrain light and sound arrays
  unsigned char ETLight[8];
  unsigned char ETSound[26];
  SetETData(ETLight, ETSound);
  etd.light = ETLight;
  etd.sound = ETSound;
  
  // East train
  taskArray[3].x = EastTrain;
  taskArray[3].y = (void*)&etd;
  
  //Setting up westTrain light and sound arrays
  unsigned char WTLight[4];
  unsigned char WTSound[14];
  SetWTData(WTLight, WTSound);
  wtd.light = WTLight;
  wtd.sound = WTSound;
  
  // West train
  taskArray[4].x = WestTrain;
  taskArray[4].y = (void*)&wtd;
  
  // Schedule
  taskArray[5].x = Schedule;
  taskArray[5].y = (void*) &ntd;
  
  
  
  //initialize OLED display
  RIT128x96x4Init(1000000);
  
  RIT128x96x4StringDraw("test", 30, 24, 15);






  // Loop the task control block
  int i;
  while(1) {  
    for(i = 0;i < 6;i++) {
      (*taskArray[i].x)(taskArray[i].y);
    }
    i = 0;
  }
}
