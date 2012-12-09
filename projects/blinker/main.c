//  *****************************************************************************
//                          main.c
//
//     Demonstration program for Olimex SAM7-H256 Evaluation Board
//
//     blinks LED0 (pin PA8) with an endless loop
//
//  Authors:  James P Lynch  September 23, 2006; Olimex, Mar 2007
//  *****************************************************************************

//  *******************************************************
//                Header Files
//  *******************************************************
#include "AT91SAM7S256.h"
#include "board.h"

//  *******************************************************
//                Function Prototypes
//  *******************************************************
void Timer0IrqHandler(void);
void FiqHandler(void);

//  *******************************************************
//                External References
//  *******************************************************
extern  void LowLevelInit(void);
extern  void TimerSetup(void);
extern  unsigned enableIRQ(void);
extern  unsigned enableFIQ(void);

//  *******************************************************
//               Global Variables
//  *******************************************************
unsigned int    FiqCount = 0;       // global uninitialized variable



// init some defaults
AT91PS_PIO    pPIO   = AT91C_BASE_PIOA;
AT91PS_PMC    pPMC   = AT91C_BASE_PMC;
AT91PS_USART  pUSART = AT91C_BASE_US0;
AT91PS_PDC    pPDC   = AT91C_BASE_PDC_US0;
AT91PS_MC     pMC    = AT91C_BASE_MC;
AT91PS_AIC    pAIC   = AT91C_BASE_AIC;
AT91PS_SYS    pSYS   = AT91C_BASE_SYS;
AT91PS_ADC    pADC   = AT91C_BASE_ADC;
AT91PS_RSTC   pRSTC  = AT91C_BASE_RSTC;

//  *******************************************************
//                     MAIN
//  ******************************************************/
int     main (void) {
    unsigned long   j;                              // loop counter (stack variable)
    unsigned long   IdleCount = 0;                  // idle loop blink counter (2x)

    // Initialize the Atmel AT91SAM7S256 (watchdog, PLL clock, default interrupts, etc.)
    // ---------------------------------------------------------------------------------
    LowLevelInit();


    // Turn on the peripheral clock for Timer0
    // ---------------------------------------

    // enable Timer0 peripheral clock
    pPMC->PMC_PCER = (1<<AT91C_ID_TC0);


    // Set up the PIO ports
    // --------------------

    // PIO Output Enable Register - sets pins P0 - P3 to outputs
    pPIO->PIO_OER = LED_MASK;

    // PIO Set Output Data Register - turns off the four LEDs
    pPIO->PIO_SODR = LED_MASK;


    // Set up the Advanced Interrupt Controller AIC for Timer 0
    // --------------------------------------------------------

    // Disable timer 0 interrupt in AIC Interrupt Disable Command Register
    pAIC->AIC_IDCR = (1<<AT91C_ID_TC0);

    // Set the TC0 IRQ handler address in AIC Source Vector Register[12]
    pAIC->AIC_SVR[AT91C_ID_TC0] = (unsigned int)Timer0IrqHandler;

    // Set the interrupt source type and priority in AIC Source Mode Register[12]
    pAIC->AIC_SMR[AT91C_ID_TC0] = (AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL | 0x4 );

    // Clear the TC0 interrupt in AIC Interrupt Clear Command Register
    pAIC->AIC_ICCR = (1<<AT91C_ID_TC0);

    // Remove disable timer 0 interrupt in AIC Interrupt Disable Command Register
    pAIC->AIC_IDCR = (0<<AT91C_ID_TC0);

    // Enable the TC0 interrupt in AIC Interrupt Enable Command Register
    pAIC->AIC_IECR = (1<<AT91C_ID_TC0);



    // Set up the Advanced Interrupt Controller AIC for FIQ (pushbutton SW1)
    // ---------------------------------------------------------------------

    // Disable FIQ interrupt in AIC Interrupt Disable Command Register
    pAIC->AIC_IDCR = (1<<AT91C_ID_FIQ);

    // Set the interrupt source type in AIC Source Mode Register[0]
    pAIC->AIC_SMR[AT91C_ID_FIQ] = (AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE);

    // Clear the FIQ interrupt in AIC Interrupt Clear Command Register
    pAIC->AIC_ICCR = (1<<AT91C_ID_FIQ);

    // Remove disable FIQ interrupt in AIC Interrupt Disable Command Register
    pAIC->AIC_IDCR = (0<<AT91C_ID_FIQ);

    // Enable the FIQ interrupt in AIC Interrupt Enable Command Register
    pAIC->AIC_IECR = (1<<AT91C_ID_FIQ);


    // Setup timer0 to generate a 50 msec periodic interrupt
    // -----------------------------------------------------

    TimerSetup();


    // enable interrupts
    // -----------------

    enableIRQ();
    enableFIQ();


    // enable reset+}
    pRSTC->RSTC_RCR = 0xA5000008;
    pRSTC->RSTC_RMR = 0xA5000001;


    // endless background blink loop
    // -----------------------------

    while (1) {
        if  ((pPIO->PIO_ODSR & LED1) == LED1)       // read previous state of LED1
            pPIO->PIO_CODR = LED1;                  // turn LED1 (DS1) on
        else
            pPIO->PIO_SODR = LED1;                  // turn LED1 (DS1) off

        for (j = 1000000; j != 0; j-- );            // wait 1 second  2000000

        IdleCount++;                                // count # of times through the idle loop

    }
}

