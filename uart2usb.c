//  UART2USB library
//  Rebeca Tourinho
//  February 2016
//  Built with Code Composer Studio v6
//***************************************************************************************

#include <msp430.h>
#include <stdint.h>
#include "uart2usb.h"

extern uint8_t flag;

//char USBrxBuffer[255];
//unsigned char bufferSize = 0;

void UCA1_init(void)
{

    P5SEL = BIT6 + BIT7;						//TX-RX functions selected
    P5DIR = BIT6;								//TX is output and RX input

    //UCA1CTL1 register
	UCA1CTL1 |= UCSWRST;						//Software reset enable bit


    //UCA1CTL0 register
    UCA1CTL0 = UCMODE_0;						//8-N-1, asynchronous UART

    //Internal clock 1.048576 MHz
    //Baud rate = 57600 bps, Table 36-4.
    UCA1CTL1 = UCSSEL__SMCLK;					//Clock source
    UCA1BR0 = 18;								//UCBRx low byte clock prescale
    UCA1BR1 = 0;								//UCBRx high byte clock prescale

    //UCA1MCTL
    UCA1MCTL |= 0x02;							//UCBRSx = 1 and UCBRFx = 0 (Table 36-4)

    UCA1CTL1 &= ~UCSWRST;						//UCA1 enabled

    UCA1IE = 0x01;								//RX interrupt enabled
    __bis_SR_register(GIE);						//Enable Interrupts


}

//Function to send a single char
void USBsendChar(char character)
{
    while (!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = character;
}


void send_string(char *string)
{
   uint8_t tx_complete=0;

   while(*string)
   {
     /*--loads the next 8-bit value to be transmitted into the tx buffer--*/
	 UCA1TXBUF = (uint8_t) *string;
     *string++;

	 /*-- waits for the TX flag bit to be set--*/
     while(tx_complete==0)
     {
       /*--this AND operation results 0 while TX flag bit is not set--*/
    	 tx_complete = UCA1IFG & UCTXIFG;
     }
     tx_complete = 0;
   }

}


#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	//USBrxBuffer[bufferSize++] = UCA1RXBUF;
	//USBsendChar(UCA1RXBUF);
	flag = RX_OK;
	UCA1IFG &= ~UCRXIFG;
//    __bic_SR_register_on_exit(LPM3_bits);
}
