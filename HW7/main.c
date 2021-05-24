//ME433 - HW7
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "spi_i2c.h"
#include <math.h>

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

void initSPI();
unsigned char spi_io(unsigned char o);
static volatile double sinewave[500];
static volatile double trianglewave[500];
#define CS LATAbits.LATA0 //chip select pin 

int main() {

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
    
    initSPI();
    __builtin_enable_interrupts();

    
    
   
    while (1) {
        //LATAbits.LATA0 = 0; 
        //LATAbits.LATA0 = 1;
        
        //Sine Wave 
        int i; 
        for(i=0; i< 500; i++){
            sinewave[i] = sin((i*4*M_PI)/10)/2 + 0.5 ; 
        }
        unsigned char c = 0; 
        unsigned short p;
        
        int ii;
        for(ii = 0; ii<500; ii++){
            unsigned short v = sinewave[ii]*1024;
            p = (c<<15); 
            p = p|(0b111<<12); 
            p = p|(v<<2);

            CS = 0;  //chip select: low
            spi_io(p>>8); 
            spi_io(p);
            CS = 1; //chip select: high
        
        
            _CP0_SET_COUNT(0); 
            while (_CP0_GET_COUNT() < 48000000/1000) {;} //delay
            
        
        }
        
        
        //Triangle Wave 
        int j; 
        int m = 100; 
        for(j=0; j< 500; j++){
            // sinewave[i] = sin((i*4*M_PI)/10)/2 + 0.5 ; 
            trianglewave[j] = m - abs(j % (2*m) - m);
        }
        unsigned char c2 = 1; 
        unsigned short p2;
        int jj;
        
        for(jj = 0; jj<500; jj++){
            unsigned short v2 = trianglewave[jj]*1024;
            p2 = (c2<<15); 
            p2 = p2|(0b111<<12); 
            p2 = p2|(v2<<2);

            CS = 0;  //chip select: low
            spi_io(p2>>8); 
            spi_io(p2);
            CS = 1; //chip select: high
        
        
            _CP0_SET_COUNT(0); 
            while (_CP0_GET_COUNT() < 48000000/1000) {;} //delay
            
        
        }
        
    }
   
}

         
// initialize SPI1
void initSPI() {
    // Pin B14 has to be SCK1
    // Turn of analog pins
    ANSELA = 0; 
    // Make an output pin for CS
    TRISAbits.TRISA0 = 0 ;
    LATAbits.LATA0 = 1;
    //.
    // Set SDO1
    RPA1Rbits.RPA1R = 0b0011; 
    // Set SDI1
    SDI1Rbits.SDI1R = 0b0001; 
   

    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1000; // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi 
}


// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}