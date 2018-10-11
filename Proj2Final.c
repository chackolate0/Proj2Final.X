/* ************************************************************************** */
/** Descriptive File Name
  @Company
    CPEG 222 Team 1
    Alex Chacko & Matias Saavedra
  @File Name
    Proj2Final.c
  @Summary
    Brief description of the file.
  @Description
    Describe the purpose of this file.
 */

#pragma config FSRSSEL = PRIORITY_7     // Shadow Register Set Priority Select (SRS Priority 7)
#pragma config PMDL1WAY = ON            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow only one reconfiguration)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2        // PLL Input Divider (12x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (24x Multiplier)
#pragma config FPLLODIV = DIV_1       // System PLL Output Clock Divider (PLL Divide by 256)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Fast RC Osc w/Div-by-N (FRCDIV))
#pragma config FSOSCEN = ON             // Secondary Oscillator Enable (Enabled)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = XT            // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_2           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = OFF              // Watchdog Timer Enable (WDT Enabled)
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is Disabled)
#pragma config JTAGEN = OFF              // JTAG Enable (JTAG Port Enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

#ifndef _SUPPRESS_PLIB_WARNING
#define _SUPPRESS_PLIB_WARNING
#endif

#include <xc.h>
#include <plib.h>
#include <stdio.h>
#include "utils.h"
#include "swt.h"
#include "ssd.h"
#include <math.h>
#include "btn.h"

#define FOSC (80000000L)
#define CORE_TICK_PERIOD (FOSC/20)

#define DELAY_BTN 50              //50 ms 1953
enum states {LEFT, RIGHT, COUNT_UP, COUNT_DOWN, STOP};
int btnState = STOP;
int hex1, hex2;
int ssdVal = 0;

int main(void){

    char hexString[80];
    LED_Init();
    LCD_Init();
    SSD_Init();
    SWT_Init();

    BTN_Init();

    OpenCoreTimer(CORE_TICK_PERIOD);
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    mConfigIntCoreTimer(CT_INT_ON | CT_INT_PRIOR_5 | CT_INT_SUB_PRIOR_0);
    INTEnableSystemMultiVectoredInt();

    LCD_WriteStringAtPos("Team 1",0,0);
    while(1){

        if(btnState == STOP){
            //
        }
        if(btnState == LEFT){
            LED_SetGroupValue(SWT_GetGroupValue());
            hex1 = SWT_GetGroupValue();
            sprintf(hexString, "Hex: %#04X + %#04X", hex1, hex2);
            ssdVal = hex1 + hex2;
        }
        if(btnState == RIGHT){
            LED_SetGroupValue(SWT_GetGroupValue());
            hex2 = SWT_GetGroupValue();
            sprintf(hexString, "Hex: %#04X + %#04X", hex1, hex2);
            ssdVal = hex1 + hex2;
        }
        if(btnState == COUNT_UP){
            int i;
            for(i=0; i < hex1 + hex2 + 1; i++){
                ssdVal = i;
                update_SSD(ssdVal);
                countTimer(1000);
            }
            btnState = STOP;
        }
        delay_ms(DELAY_BTN);
        LCD_WriteStringAtPos(hexString, 1, 0);
        LED_SetGroupValue(SWT_GetGroupValue());
        update_SSD(SWT_GetGroupValue());
        //SSD_WriteDigitsGroupedDecimal(SWT_GetGroupValue(),0);
        update_SSD(ssdVal);
    }
}

int getHexFromSWT(int n){
        int digit1, digit2, digit3, digit4;
        if (n == 0){
            digit1 = SWT_GetValue(0);
            digit2 = SWT_GetValue(1) * 0x2;
            digit3 = SWT_GetValue(2) * 0x4;
            digit4 = SWT_GetValue(3) * 0x8;
        }
        if (n == 1){
            digit1 = SWT_GetValue(4);
            digit2 = SWT_GetValue(5) * 0x2;
            digit3 = SWT_GetValue(6) * 0x4;
            digit4 = SWT_GetValue(7) * 0x8;
        }
        return digit1 | digit2 | digit3 | digit4;
        //return SWT_GetValue(0) & SWT_GetValue(1);
}

void update_SSD(int value) {
    int hunds, tens, ones, tenths;
    char SSD1 = 0b0000000; //SSD setting for 1st SSD (LSD)
    char SSD2 = 0b0000000; //SSD setting for 2nd SSD
    char SSD3 = 0b0000000; //SSD setting for 3rd SSD
    char SSD4 = 0b0000000; //SSD setting for 4th SSD (MSD)

    hunds = floor(value / 100);
    if (hunds > 0)
        SSD4 = hunds; //SSD4 = display_char[thous];
    else
        SSD4 = 17; //blank display
    tens = floor((value % 100) / 10);
    if (hunds == 0 && tens == 0){
        SSD3 = 17; //blank display
    }
    else
        SSD3 = tens;
    ones = floor(value % 10);
    if (hunds == 0 && tens == 0 && ones == 0)
        SSD2 = 0;
    else
        SSD1 = ones;

    SSD_WriteDigits(SSD1, SSD2, SSD3, SSD4, 0, 0, 0, 0);
        SSD2 = ones;
    tenths = floor(value % 1);
    SSD1 = tenths;

    SSD_WriteDigits(SSD1, SSD2, SSD3, SSD4, 0, 1, 0, 0);
}

void delay_ms(int ms) {
    int i, counter;
    for (counter = 0; counter < ms; counter++) {
        for (i = 0; i < 1300; i++) {
        } //software delay ~1 millisec
    }
}

void countTimer(int ms) {
    unsigned int ui;
    ui = ReadCoreTimer();
    // convert in seconds (1 core tick = 2 SYS cycles)
   return ( ui * 2.0 /80000000);
}

void __ISR(_CORE_TIMER_VECTOR, ipl5) _CoreTimerHandler(void){
    mCTClearIntFlag(); //clear interrupt
    if(BTN_GetValue(2)){//PORTFbits.RF0
            delay_ms(DELAY_BTN);
            btnState = STOP;
        }
    else if(BTN_GetValue(3)){//PORTBbits.RB8
            delay_ms(DELAY_BTN);
            btnState = RIGHT;
        }
        else if(BTN_GetValue(1)){
            delay_ms(DELAY_BTN);
            btnState = LEFT;
        }
        else if(BTN_GetValue(0)){
            delay_ms(DELAY_BTN);
            btnState = COUNT_UP;
            }
        else if(BTN_GetValue(4)){
            delay_ms(DELAY_BTN);
            btnState = COUNT_DOWN;
        }

    UpdateCoreTimer(CORE_TICK_PERIOD);  //update period
}
