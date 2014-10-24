
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
int seed = 42;
northTrainData ntd;
eastTrainData etd;
westTrainData wtd;
switchControlData scd;
scheduleData sd;

int main()
{
    // Set the clocking to run directly from the crystal
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

    // Initialize the OLED
    RIT128x96x4Init(OLED_FREQ);

    // Initialize PWM buzzer
    InitBuzzer(FREQUENCY);

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
    unsigned char NTLight [NTLIGHT_LEN];  
    unsigned char NTSound [NTSOUND_LEN]; 
    SetNTData(NTLight, NTSound); 
    taskArray[2].x = NorthTrain;
    taskArray[2].y = (void*)&ntd;

    // East train 
    unsigned char ETLight[ETLIGHT_LEN];  
    unsigned char ETSound[ETSOUND_LEN]; 
    SetETData(ETLight, ETSound);
    taskArray[3].x = EastTrain;  
    taskArray[3].y = (void*)&etd;

    // West train
    unsigned char WTLight[WTLIGHT_LEN];  
    unsigned char WTSound[WTSOUND_LEN]; 
    SetWTData(WTLight, WTSound);
    taskArray[4].x = WestTrain;
    taskArray[4].y = (void*)&wtd;

    // Schedule
    SetSData();
    taskArray[5].x = Schedule;
    taskArray[5].y = (void*) &sd;

    //GPIO writing
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    // Loop the task control block
    int i;
    while(1) {  
        for(i = 0;i < 6;i++) {
          (*taskArray[i].x)(taskArray[i].y);
          GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00); //write GPIO LED0 off
        }
        i = 0;
    }
}
