#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "stdbool.h"
#include "functions.h"
#include <math.h>

// Handle timing 
void Schedule(void* d) {
	int i;
        
        int glb_cnt_prev = globalCount;
        // Display global count
        char a[7];
        IntToString(globalCount, a, 7);
        RIT128x96x4StringDraw(a, BASE_X+SHIFT_X, BASE_Y+GLOB, OLED_LEVEL);
        
        // execute queue
	for(i = 0; taskArray[i].y; i++) { 
		(*taskArray[i].x)(taskArray[i].y);
	}

	// update queue
	int p = 0;
	// traincom
	if (!trainPresent && !gridlock) {
		taskArray[p].x = TrainCom;
		taskArray[p].y = (void*) &ntd; // data unused
		p++;
	}
	// switch control
	if(trainPresent || gridlock) {
		taskArray[p].x = SwitchControl;
		taskArray[p].y = (void*) &scd;
		p++;
	}
	// train present (if gridlock, trainpresent == false)
	if(trainPresent) {
		taskArray[p].x = CurrentTrain;
		taskArray[p].y = (void*) &trains[dir_to];
		p++;
	}
	// serial com
	if(serial_flag) {
                serial_flag = false;
		taskArray[p].x = SerialCom;
		taskArray[p].y = (void*) &ntd; // data unused
		p++;
	}
	// terminate task queue
	taskArray[p].y = 0;
        
        // get the frequency and update the number of passengers  
        int pulse_freq = pulse_count * 2;
        pass_count = GetPassengers(pulse_freq);
        if (pass_count < 0) {
          pass_count = 0;
        }
        pulse_count = 0;
        
        // Delay until the next global count
        while(glb_cnt_prev == globalCount) {};
     
}


// Startup function
void Startup() {
    ///////////////////////////////////////////////////////////////////////////
    // Set the clocking to run directly from the crystal
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

    // Initialize the OLED
    RIT128x96x4Init(OLED_FREQ);

    // Initialize PWM buzzer
    InitBuzzer(FREQUENCY);
    
    ///////////////////////////////////////////////////////////////////////////
    // Initialize ISR for pushbuttons
    
    //Clear the default ISR handler and install IntGPIOe as the handler:
    GPIOPortIntUnregister(GPIO_PORTE_BASE);
    GPIOPortIntRegister(GPIO_PORTE_BASE, ButtonHandler);

    //Enable GPIO port E, set pin 0 as an input
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, BUTTON_PINS);
    
    //GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);

    //Activate the pull-up on GPIO port E
    GPIOPadConfigSet(GPIO_PORTE_BASE, BUTTON_PINS, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);
    
    //Configure GPIO port E as triggering on falling edges
    GPIOIntTypeSet(GPIO_PORTE_BASE, BUTTON_PINS, GPIO_FALLING_EDGE);
    
    //Enable interrupts for GPIO port E
    GPIOPinIntEnable(GPIO_PORTE_BASE, BUTTON_PINS);
    IntEnable(INT_GPIOE);
    IntMasterEnable();
    
    ///////////////////////////////////////////////////////////////////////////
        // Initialize ISR for function generator 
    
    //Clear the default ISR handler and install IntGPIOe as the handler:
    GPIOPortIntUnregister(GPIO_PORTF_BASE);
    GPIOPortIntRegister(GPIO_PORTF_BASE, PulseCount);

    //Enable GPIO port E, set pin 0 as an input
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, PULSE_PIN);
    
    //GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);

    //Activate the pull-up on GPIO port F
    GPIOPadConfigSet(GPIO_PORTF_BASE, PULSE_PIN, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);
    
    //Configure GPIO port E as triggering on falling edges
    GPIOIntTypeSet(GPIO_PORTF_BASE, PULSE_PIN, GPIO_FALLING_EDGE);
    
    //Enable interrupts for GPIO port F
    GPIOPinIntEnable(GPIO_PORTF_BASE, PULSE_PIN);
    IntEnable(INT_GPIOF);
//    IntMasterEnable();
    
     ///////////////////////////////////////////////////////////////////////////
    // Initialize ISR for timer/global count
    
    TimerIntUnregister(TIMER1_BASE, TIMER_A);
    TimerIntRegister(TIMER1_BASE, TIMER_A, IntTimer1);

    //Enable Timer 1    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    //Configure Timer 1 and set the timebase to 0.5 second    
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 2);    
    
    //Enable interrupts for Timer0 and activate it
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER1_BASE, TIMER_A);
    
    ///////////////////////////////////////////////////////////////////////////
    // Initialize serial comm
    
    //Include peripherals GPIOA and UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    
     // Set GPIO A0 and A1 as UART pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
   
    ///////////////////////////////////////////////////////////////////////////
    // Initialize task queue
    
            // Train com
    taskArray[0].x = TrainCom;
    taskArray[0].y = (void*) &ntd; //data not actually used

            // Switch control
    static unsigned char SCLight [2];
    SetSCData(SCLight);
    //taskArray[1].x = SwitchControl;
    //taskArray[1].y = (void*) &scd;

            // Serial comm
    //taskArray[2].x = SerialCom;
    //taskArray[2].y = (void*) &ntd; // data unused

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

    trains[0] = ntd; 
    trains[1] = etd; 
    trains[2] = std; 
    trains[3] = wtd;

    // Set display labels 
    RIT128x96x4StringDraw("Trn Pres: \0",BASE_X, BASE_Y+PRES, OLED_LEVEL);
    RIT128x96x4StringDraw("Trn Size: \0",BASE_X, BASE_Y+SZ, OLED_LEVEL);
    RIT128x96x4StringDraw("From Dir: \0",BASE_X, BASE_Y+FROM, OLED_LEVEL);
    RIT128x96x4StringDraw("To Dir: \0",  BASE_X, BASE_Y+TO, OLED_LEVEL);
    RIT128x96x4StringDraw("Ppl. Ct: \0", BASE_X, BASE_Y+PASS, OLED_LEVEL);
    RIT128x96x4StringDraw("Glb Ct: \0",  BASE_X, BASE_Y+GLOB, OLED_LEVEL);
}

// Sets the train direction, and train size
void TrainCom(void* d) {
	// Clear to-direction, from-direction, gridlock, and passenger count
        RIT128x96x4StringDraw("N \0", BASE_X+SHIFT_X, BASE_Y+PRES, OLED_LEVEL);  
        RIT128x96x4StringDraw(NA, BASE_X+SHIFT_X, BASE_Y+TO, OLED_LEVEL);
        RIT128x96x4StringDraw(NA, BASE_X+SHIFT_X, BASE_Y+SZ, OLED_LEVEL);
        RIT128x96x4StringDraw(NA, BASE_X+SHIFT_X, BASE_Y+FROM, OLED_LEVEL);
        RIT128x96x4StringDraw(NA, BASE_X+SHIFT_X, BASE_Y+PASS, OLED_LEVEL);
        RIT128x96x4StringDraw(CLEAR_SCREEN, BASE_X+SHIFT_X, BASE_Y+GRIDL, OLED_LEVEL);

        // Clear the buzzer
        PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, false);

	// generate a random to-direction
	dir_to = RandomInt(0, 3);

	// create a random value for trainSize between 2 and 9
	//trainSize = RandomInt(TRAIN_SIZE_MIN, TRAIN_SIZE_MAX);
        trainSize = 2;
        
        // reset the delay counter for light and sound
	trains[dir_to].i = 0;
}

// Handles the routing of trains
void SwitchControl(void* d){ 
    switchControlData* scd = (switchControlData*) d;
    // case 1 (trainpresent, gridlock not checked)
    if (!(scd->gridlockChecked)) {
        // check for a gridlock
        //int n = RandomInt(-2, 2);
        int n = 1;
        if (n < 0) {
            scd->delay = -0.2 * n * GLOBAL_CNT_PER_MIN;
            gridlock = true;
            trainPresent = false;
            serial_flag = true;
        } else {
            char train_sz[2];
            char passengers[4];
            IntToString(trainSize, train_sz, 2);
            IntToString(pass_count, passengers, 4);
            RIT128x96x4StringDraw(CLEAR_SCREEN, BASE_X+SHIFT_X, BASE_Y+SZ, OLED_LEVEL);
            RIT128x96x4StringDraw("Y \0", BASE_X+SHIFT_X, BASE_Y+PRES, OLED_LEVEL);
            RIT128x96x4StringDraw(passengers, BASE_X+SHIFT_X, BASE_Y+PASS, OLED_LEVEL);
            RIT128x96x4StringDraw(train_sz, BASE_X+SHIFT_X, BASE_Y+SZ, OLED_LEVEL);
            RIT128x96x4StringDraw(GetDirection(dir_from), BASE_X+SHIFT_X, BASE_Y+FROM, OLED_LEVEL);

            scd->delay = 0.1 * trainSize * GLOBAL_CNT_PER_MIN;
        }
        // update state 
        scd->gridlockChecked = true; 
    }
    // case2 (gridlocked)
    if (gridlock) {
        // issue visual alarm (0.5 sec on, 0.5 off) 
        if (scd->light[globalCount % 2]){ 
            char display[32] = "GRIDLOCK \0";
            RIT128x96x4StringDraw(display, BASE_X+SHIFT_X, BASE_Y+GRIDL, OLED_LEVEL);
        }else{
                RIT128x96x4StringDraw(CLEAR_SCREEN, BASE_X+SHIFT_X, BASE_Y+GRIDL, OLED_LEVEL);
        }
        // reset if delay has elapsed
        if (scd->i > scd->delay) {
                serial_flag = true;
                gridlock = false;
                scd->gridlockChecked = false;
                scd->i = 0;
        }
     // case3 train is present and not gridlocked
    } else {
        // reset if delay has elapsed
        if (scd->i > scd->delay) {
                serial_flag = true;
                trainPresent = false;
                scd->gridlockChecked = false;
                trainSize = 0;
                scd->i = 0;
        }
    }
    scd->i++;
}

// Handle current train behavior
void CurrentTrain(void* d) {
  if(!gridlock){
    currentTrainData* ctd = (currentTrainData*) d;
    if (ctd->light[globalCount % ctd->lightlen]){ 
		char* display = GetDirection(dir_to);
		RIT128x96x4StringDraw(display, BASE_X+SHIFT_X, BASE_Y+TO, OLED_LEVEL);
    } else {
		RIT128x96x4StringDraw(CLEAR_SCREEN, BASE_X+SHIFT_X, BASE_Y+TO, OLED_LEVEL);
    }
  
  if (ctd->i < ctd->soundlen && ctd->sound[ctd->i]) { 
		PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
  } else {
		PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, false);
  }
  ctd->i++;
  }
}

// Serial communications
void SerialCom(void* d) {
      unsigned char* str0 = "Train Present:           \r\n";
      unsigned char* str1 = "Train Size:              \r\n";
      unsigned char* str2 = "From Dir:                \r\n";
      unsigned char* str3 = "To Dir:                  \r\n";
      unsigned char* str4 = "Ppl Cnt:                 \r\n";
      unsigned char* str5 = "Glb Cnt:               \r\n\n";
      // 
      //033[2J clear
      //033[0;0H cursor
      if(!gridlock) {
        if(trainPresent){
          str0[13] = (unsigned char) 3;
          str1[UART_SHIFT] = (unsigned char) trainSize + ASCII_OFFSET;
          unsigned char* dir_f = (unsigned char*) GetDirection(dir_from);
          unsigned char* dir_t = (unsigned char*) GetDirection(dir_to);
          for(int i = 0; i < 5; i++) str2[UART_SHIFT + i] = dir_f[i];
          for(int i = 0; i < 5; i++) str3[UART_SHIFT + i] = dir_t[i];
          unsigned char ppl_cnt[4];
          IntToString(pass_count, ppl_cnt, 4);
          for(int i = 0; i < 3; i++) str4[UART_SHIFT + i] = ppl_cnt[i];
          unsigned char glb_cnt[8];
          IntToString(globalCount, glb_cnt, 9);
          for(int i = 0; i < 6; i++) str5[UART_SHIFT + i] = glb_cnt[i];
        } else {
          
        }
        //print stuff
      } else {
        //print gridlock
      }
      UARTCharPut(UART0_BASE, '\f');
      UARTSend((const unsigned char*) str0, UART_STR_LEN);
      UARTSend((const unsigned char*) str1, UART_STR_LEN);
      UARTSend((const unsigned char*) str2, UART_STR_LEN);
      UARTSend((const unsigned char*) str3, UART_STR_LEN);
      UARTSend((const unsigned char*) str4, UART_STR_LEN);
      UARTSend((const unsigned char*) str5, UART_STR_LEN);
}

//UART SEND  from stellarisware
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount){
    // Loop while there are more characters to send.
    while(ulCount--) {
        // Write the next character to the UART.
        UARTCharPut(UART0_BASE, *pucBuffer++);
    }
}

// set data for North Train
void SetNTData(unsigned char* NTLight, unsigned char* NTSound){
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
  ntd.lightlen = 6;
  ntd.soundlen = 20;
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
		etd.lightlen = 8;
		etd.soundlen = 26;
}

// Set data South train
void SetSTData(unsigned char* STLight, unsigned char* STSound) {
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
		std.lightlen = 4;
		std.soundlen = 24;
  
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
		wtd.lightlen = 4;
		wtd.soundlen = 14;
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
char* GetDirection(int dir) {
		if (dir == 0) {
			return "North \0";
		} else if (dir == 1) {
			return "East  \0";
		} else if (dir == 2) {
			return "South \0";
		} else {
			return "West  \0"; 
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

// Interrupt handler for button press
void ButtonHandler() {
                unsigned char buttons = GPIOPinRead(GPIO_PORTE_BASE, BUTTON_PINS);
		//Clear the interrupt 
		GPIOPinIntClear(GPIO_PORTE_BASE, BUTTON_PINS);
                //set the direction
		if (!trainPresent && !gridlock) {	
				//Read from all four buttons
				//unsigned char buttons = GPIOPinRead(GPIO_PORTE_BASE, BUTTON_PINS);
				//Invert since buttons are active low
				buttons = ~buttons;
				if(buttons % 2) {  	
					dir_from = 0;
				} else if ((buttons / 2) % 2){
					dir_from = 2;
				} else if ((buttons / 4) % 2){
					dir_from = 3;
				}else if ((buttons / 8) % 2){
					dir_from = 1;
				}
				trainPresent = true;
                                serial_flag = true;
		}

}

// Returns the number of passengers, given a pulse frequency
int GetPassengers(int pulse_freq) {
    return (int) 3.0/10.0 * pulse_freq - 300;
}

//ISR for Timer 1, increment global count
void IntTimer1(void)
{
  //Clear the interrupt 
  TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  globalCount++;
}

//Interrupt for counting the number of FG pulses
void PulseCount() {
  pulse_count++;
  GPIOPinIntClear(GPIO_PORTF_BASE, PULSE_PIN);
}

// converts an int to a string
// allocates a string of fixed length
void IntToString(int num, char* string, int len) {
    int i;
    for (i = len-2; i >=0; i--) {
            string[i] = num % 10 + ASCII_OFFSET;
            num = num / 10;
    }
    string[len-1] = '\0';
}