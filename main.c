
/*
    FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution and has been modified to 
    demonstrate three simple tasks running.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/


/*
 * Creates all the application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the standard demo application tasks.
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "OLED" task - the OLED task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the display directly.  Other tasks wishing to write a
 * message to the OLED send the message on a queue to the OLED task instead of
 * accessing the OLED themselves.  The OLED task just blocks on the queue waiting
 * for messages - waking and displaying the messages as they arrive.
 *
 * "Check" hook -  This only executes every five seconds from the tick hook.
 * Its main function is to check that all the standard demo tasks are still
 * operational.  Should any unexpected behaviour within a demo task be discovered
 * the tick hook will write an error to the OLED (via the OLED task).  If all the
 * demo tasks are executing with their expected behaviour then the check task
 * writes PASS to the OLED (again via the OLED task), as described above.
 *
 * "uIP" task -  This is the task that handles the uIP stack.  All TCP/IP
 * processing is performed in this task.
 */




/*************************************************************************
 * Please ensure to read http://www.freertos.org/portlm3sx965.html
 * which provides information on configuring and running this demo for the
 * various Luminary Micro EKs.
 *************************************************************************/

/* Set the following option to 1 to include the WEB server in the build.  By
default the WEB server is excluded to keep the compiled code size under the 32K
limit imposed by the KickStart version of the IAR compiler.  The graphics
libraries take up a lot of ROM space, hence including the graphics libraries
and the TCP/IP stack together cannot be accommodated with the 32K size limit. */

//  set this value to non 0 to include the web server

#define mainINCLUDE_WEB_SERVER		1

///* Standard includes. */
//#include <stdio.h>
//
///* Scheduler includes. */
//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
//#include "semphr.h"
//
///* Hardware library includes. */
//#include "hw_memmap.h"
//#include "hw_types.h"
//#include "hw_sysctl.h"
//#include "sysctl.h"
//#include "gpio.h"
//#include "grlib.h"
//#include "rit128x96x4.h"
//#include "osram128x64x4.h"
//#include "formike128x128x16.h"
//
///* Demo app includes. */
//
//#include "lcd_message.h"
//#include "bitmap.h"

/* Train task includes */

#include "functions.h"
#define H

/*-----------------------------------------------------------*/

/* 
  The time between cycles of the 'check' functionality (defined within the
  tick hook. 
*/
#define mainCHECK_DELAY	( ( portTickType ) 5000 / portTICK_RATE_MS )

// Size of the stack allocated to the uIP task.
#define mainBASIC_WEB_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 3 )

// The OLED task uses the sprintf function so requires a little more stack too.
#define mainOLED_TASK_STACK_SIZE	    ( configMINIMAL_STACK_SIZE + 50 )

//  Task priorities.
#define mainQUEUE_POLL_PRIORITY		    ( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY		    ( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY		    ( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY		    ( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )
#define mainGEN_QUEUE_TASK_PRIORITY	    ( tskIDLE_PRIORITY )


//  The maximum number of messages that can be waiting for display at any one time.
  #define mainOLED_QUEUE_SIZE					( 20 )

// Dimensions the buffer into which the jitter time is written. 
  #define mainMAX_MSG_LEN						25

/* 
  The period of the system clock in nano seconds.  This is used to calculate
  the jitter time in nano seconds. 
*/
  
#define mainNS_PER_CLOCK ( ( unsigned portLONG ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )


// Constants used when writing strings to the display.

#define mainCHARACTER_HEIGHT		    ( 9 )
#define mainMAX_ROWS_128		    ( mainCHARACTER_HEIGHT * 14 )
#define mainMAX_ROWS_96			    ( mainCHARACTER_HEIGHT * 10 )
#define mainMAX_ROWS_64			    ( mainCHARACTER_HEIGHT * 7 )
#define mainFULL_SCALE			    ( 15 )
#define ulSSI_FREQUENCY			    ( 3500000UL )

/*our globals and stuff*/

int debug = 0;

// train state data
int waitTime = 0;
unsigned char dis_sel = 0;
int dir_to[2];
int dir_from[2];
int pass_count[2];
int pulse_count = 0;
int temp = 0;
bool active = false;
bool trainCreated = false;
bool serial_flag = true;
bool gridlock = false;
bool trainPresent[2] = {false,false};
unsigned int trainSize[2] = {0,0};
unsigned int globalCount = 0;
unsigned int oldGC = 0;
unsigned int oldGC_sel = 0;
currentTrainData ntd;
currentTrainData etd;
currentTrainData std;
currentTrainData wtd;
switchControlData scd;
// task queue
tcb taskArray[4];
currentTrainData trains[4];
// random seed
int seed = 38;

//second Train
//int dir_to[1];
//int dir_from[1];
//int pass_count[1];
//bool trainPresent[1] = false;
//unsigned int trainSize[1] = 0;

/*-----------------------------------------------------------*/

/*
 * The task that handles the uIP stack.  All TCP/IP processing is performed in
 * this task.
 */
extern void vuIP_Task( void *pvParameters );

/*
 * The display is written two by more than one task so is controlled by a
 * 'gatekeeper' task.  This is the only task that is actually permitted to
 * access the display directly.  Other tasks wanting to display a message send
 * the message to the gatekeeper.
 */

static void vOLEDTask( void *pvParameters );

/*
 * Configure the hardware .
 */
static void prvSetupHardware( void );

/*
 * Configures the high frequency timers - those used to measure the timing
 * jitter while the real time kernel is executing.
 */
extern void vSetupHighFrequencyTimer( void );

/*
 * Hook functions that can get called by the kernel.
 */
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );
void vApplicationTickHook( void );

/*
  three dummy tasks of different priorities that simply run, announce 
  themselves, then sleep
*/

void vTask1(void *vParameters);
void vTask2(void *vParameters);
void vTask3(void *vParameters);


/*-----------------------------------------------------------*/

/* 
  The queue used to send messages to the OLED task. 
*/
xQueueHandle xOLEDQueue;

xTaskHandle    TrainComHandle;
xTaskHandle    SwitchControlHandle;
xTaskHandle    SerialComHandle;
xTaskHandle    CurrentTrainHandle;
xTaskHandle    TimerHandle;

/*-----------------------------------------------------------*/


/*************************************************************************
 * Please ensure to read http://www.freertos.org/portlm3sx965.html
 * which provides information on configuring and running this demo for the
 * various Luminary Micro EKs.
 *************************************************************************/

int main( void )
{
    prvSetupHardware();

    /*  
        Create the queue used by the OLED task.  Messages for display on the OLED
        are received via this queue. 
    */
    
    xOLEDQueue = xQueueCreate( mainOLED_QUEUE_SIZE, sizeof( xOLEDMessage ) );
    

    /* 
        Exclude some tasks if using the kickstart version to ensure we stay within
        the 32K code size limit. 
    */
    
    #if mainINCLUDE_WEB_SERVER != 0
    {
      /* 
          Create the uIP task if running on a processor that includes a MAC and PHY. 
      */
      
      if( SysCtlPeripheralPresent( SYSCTL_PERIPH_ETH ) )
      {
          xTaskCreate( vuIP_Task, ( signed portCHAR * ) "uIP", mainBASIC_WEB_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY - 1, NULL );
      }
    }
    #endif
    
    /* Initialize train specific hardware and data */ 
    Startup();
    
    
    /* Start the tasks */
    
    
    xTaskCreate( vOLEDTask, ( signed portCHAR * ) "OLED", mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate(TrainCom, "Train", 200,NULL, 1,TrainComHandle);
    xTaskCreate(SwitchControl, "Switch", 200,(void*) &scd, 1,SwitchControlHandle);
    xTaskCreate(CurrentTrain, "Current", 200, NULL, 1,CurrentTrainHandle);
    xTaskCreate(SerialCom, "Serial", 200,(void*) &scd, 1,SerialComHandle);
    xTaskCreate(Timer, "Timer", 200,(void*) NULL, 1, TimerHandle);
    /*
      Configure the high frequency interrupt used to measure the interrupt
      jitter time.  
    */
    
    vSetupHighFrequencyTimer();

    /* 
      Start the scheduler. 
    */
    
    vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle task. */
    
    return 0;
}

/*
  three dummy tasks
*/

void vTask1(void *vParameters)
{
  xOLEDMessage xMessage;
   
  volatile unsigned long ul;  
  const char *T1Text = "Task 1 is running\n\r";

  xMessage.pcMessage = "Bon Jour, Task 1";
  
  while(1)
  {
      //Send the message to the OLED gatekeeper for display.
      xQueueSend( xOLEDQueue, &xMessage, 0 );
    
      vTaskDelay(1000);
  }
}

void vTask2(void *vParameters)
{
  xOLEDMessage xMessage;
   
  volatile unsigned long ul;  
  const char *T1Text = "Task 2 is running\n\r";
  
  xMessage.pcMessage = "Bon Jour, Task 2";
  
  while(1)
  {
     // Send the message to the OLED gatekeeper for display. 
     xQueueSend( xOLEDQueue, &xMessage, 0 );
    
     vTaskDelay(3000);
  }
}

void vTask3(void *vParameters)
{
  xOLEDMessage xMessage;
   
  volatile unsigned long ul;  
  const char *T1Text = "Task 3 is running\n\r";
  
  xMessage.pcMessage = "Bon Jour, Task 3";
  
  while(1)
  {
      // Send the message to the OLED gatekeeper for display. 
      xQueueSend( xOLEDQueue, &xMessage, 0 );
    
      vTaskDelay(2000);
  }
}

/*
  the OLED Task
*/

void vOLEDTask( void *pvParameters )
{
    xOLEDMessage xMessage;
    unsigned portLONG ulY, ulMaxY;
    static portCHAR cMessage[ mainMAX_MSG_LEN ];
    extern volatile unsigned portLONG ulMaxJitter;
    unsigned portBASE_TYPE uxUnusedStackOnEntry;
    const unsigned portCHAR *pucImage;

// Functions to access the OLED. 

    void ( *vOLEDInit )( unsigned portLONG ) = NULL;
    void ( *vOLEDStringDraw )( const portCHAR *, unsigned portLONG, unsigned portLONG, unsigned portCHAR ) = NULL;
    void ( *vOLEDImageDraw )( const unsigned portCHAR *, unsigned portLONG, unsigned portLONG, unsigned portLONG, unsigned portLONG ) = NULL;
    void ( *vOLEDClear )( void ) = NULL;
  
  
    vOLEDInit = RIT128x96x4Init;
    vOLEDStringDraw = RIT128x96x4StringDraw;
    vOLEDImageDraw = RIT128x96x4ImageDraw;
    vOLEDClear = RIT128x96x4Clear;
    ulMaxY = mainMAX_ROWS_96;
   // pucImage = pucBasicBitmap;
              
// Just for demo purposes.
    uxUnusedStackOnEntry = uxTaskGetStackHighWaterMark( NULL );
  
    ulY = ulMaxY;
    
    /* Initialise the OLED  */
    vOLEDInit( ulSSI_FREQUENCY );	
    
    while( 1 )
    {
      // Wait for a message to arrive that requires displaying.
      
      xQueueReceive( xOLEDQueue, &xMessage, portMAX_DELAY );
  
      // Write the message on the next available row. 
      
//      ulY += mainCHARACTER_HEIGHT;
//      if( ulY >= ulMaxY )
//      {
//          ulY = mainCHARACTER_HEIGHT;
//          vOLEDClear();
//      }
  
      // Display the message  
                      
      sprintf( cMessage, "%s", xMessage.pcMessage);
      
      vOLEDStringDraw( cMessage, xMessage.XLOC, xMessage.YLOC, mainFULL_SCALE );
      
  }
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
    ( void ) pxTask;
    ( void ) pcTaskName;
  
    while( 1 );
}

/*-----------------------------------------------------------*/

void prvSetupHardware( void )
{
    /* 
      If running on Rev A2 silicon, turn the LDO voltage up to 2.75V.  This is
      a workaround to allow the PLL to operate reliably. 
    */
  
    if( DEVICE_IS_REVA2 )
    {
        SysCtlLDOSet( SYSCTL_LDO_2_75V );
    }
	
    // Set the clocking to run from the PLL at 50 MHz
    
    SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ );
    
    /* 	
      Enable Port F for Ethernet LEDs
            LED0        Bit 3   Output
            LED1        Bit 2   Output 
    */
    
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    GPIODirModeSet( GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_3), GPIO_DIR_MODE_HW );
    GPIOPadConfigSet( GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_3 ), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD );	
	
}


/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    static xOLEDMessage xMessage = { "PASS" };
    static unsigned portLONG ulTicksSinceLastDisplay = 0;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    /* 
      Called from every tick interrupt.  Have enough ticks passed to make it
      time to perform our health status check again? 
    */
    
    ulTicksSinceLastDisplay++;
    if( ulTicksSinceLastDisplay >= mainCHECK_DELAY )
    {
       ulTicksSinceLastDisplay = 0;
            
    }
}