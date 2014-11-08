
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
int seed = 41;
northTrainData ntd;
eastTrainData etd;
southTrainData std;
westTrainData wtd;
switchControlData scd;
//scheduleData sd;
tcb taskArray[4];
tcb 

int main() {
    Startup();
    
    //GPIO writing
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    // Loop the task control block
    while(1) {  
          Schedule();
          //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00); //write GPIO LED0 off
    }
}
