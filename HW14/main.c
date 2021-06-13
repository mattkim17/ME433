#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

void angle_to_duty(int angle){
    OC5RS = (angle*6300/17) ;
}

int main(void) {
  // call initializations
  __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; //A4 as output
    TRISBbits.TRISB4 = 1; //B4 as input 
    
    LATAbits.LATA4 = 0; 
    LATBbits.LATB4 = 0; //both A4 and B4 turned off 
  // set the pin you want to use to OCx   48000000 = 1 sec
   
  //  OC5CON = 0b111; 
    TRISBbits.TRISB6 = 0; //set B6 as output 
    LATBbits.LATB6 = 1; // set B6 as high
    
    RPB6Rbits.RPB6R = 0b0110; // set up OC5 using RPA2R pin
    
    T2CONbits.TCKPS = 5;     //N =(1:32) set the timer prescaler so that you can use the largest PR2 value as possible without going over 65535 and the frequency is 50Hz
  // possible values for TCKPS are 0 for 1:1, 1 for 1:2, 2 for 1:4, 3 for 1:8, 4 for 1:16, 5 for 1:32, ...
    PR2 = 65535;              // max value for PR2 is 65535 -> ((PR2+1) * N * 12.5 ns)
    TMR2 = 0;                // initial TMR2 count is 0
    
    OC5CONbits.OCTSEL = 0;  
    OC5CONbits.OCM = 0b110;  // PWM mode without fault pin; other OCxCON bits are defaults
    OC5RS = 6500;             // duty cycle = OCxRS/(PR2+1) --> 5000/50000 = 0.1  [0.5~2.5ms / 20ms]
    OC5R = 6500;              // initialize before turning OCx on; afterward it is read-only
    T2CONbits.ON = 1;        // turn on Timer2
    OC5CONbits.ON = 1;       // turn on OCx
  
  // the rest of your code
  
  // set OCxRS to get between a 0.5ms and 2.5ms pulse out of the possible 20ms (50Hz)
   
    
    int i ;
    while(1){
        angle_to_duty(170);
    }
    
               
            
        
    
        
   }
   
