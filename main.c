#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "string.h"
#include "stdio.h"

volatile char response[100];
volatile int a;
char cChar;

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(unsigned long ulBase, const unsigned char *pucBuffer, unsigned long ulCount)
{
	//
	// Loop while there are more characters to send.
	//
	while(ulCount--)
	{
		//
		// Write the next character to the UART.
		//
		UARTCharPut(ulBase, *pucBuffer++);
	}
}



//*****************************************************************************
//
// Receive a string from the UART.
//
//*****************************************************************************
void
UARTReceive(unsigned long ulBase, volatile char pucBuffer[])
{
	//
	// Loop while there are more characters to send.
	//
	while(UARTCharsAvail(ulBase))
	{
		// Read the next character from the UART
		cChar = UARTCharGet(ulBase);
		pucBuffer[a++] = cChar;
		UARTCharPut(UART0_BASE, cChar);

	}
}

//*****************************************************************************
//
// The UART0 interrupt handler.
//
//****************************************************************************
//void
//UART0IntHandler(void)
//{
//	unsigned long ulStatus;
//	//tBoolean bRc;
//
//	// Get the interrupt status.
//	ulStatus = UARTIntStatus(UART0_BASE, true);
//
//	// Clear the asserted interrupts.
//	UARTIntClear(UART0_BASE, ulStatus);
//
//	// Mientras haya data en el UART1
//	UARTReceive(UART0_BASE, command);
//
//	//Print to terminal, device response.
//
//}


//////////////////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//
// The UART1 interrupt handler.
//
//****************************************************************************
void
UART1IntHandler(void)
{
	unsigned long ulStatus;
	//tBoolean bRc;

	// Get the interrupt status.
	ulStatus = UARTIntStatus(UART1_BASE, true);

	// Clear the asserted interrupts.
	UARTIntClear(UART1_BASE, ulStatus);

	// Mientras haya data en el UART1
	UARTReceive(UART1_BASE, response);

}


//////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	volatile long int i=1000000;
	volatile long int seg=1000000;//3000000 funciona con 38400 baudrate.
	// Setear clock
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	//Configuracion de los puertos UART que se van a usar en el micro
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// Enable processor interrupts
	IntMasterEnable();

	// Configure the GPIO pin muxing for the UART function.
	GPIOPinConfigure(0x00000001);//GPIO_PA0_U0RX
	GPIOPinConfigure(0x00000401);//GPIO_PA1_U0TX
	GPIOPinConfigure(0x00010001);//GPIO_PB0_U0RX
	GPIOPinConfigure(0x00010401);//GPIO_PB1_U0TX

	// Since GPIO A0 and A1 are used for the UART function, they must be
	// configured for use as a peripheral function (instead of GPIO).
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Configure the UART for 115,200, 8-N-1 operation.
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));


	IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

	// Put a character to show start of example. This will display on the terminal.
	UARTCharPut(UART0_BASE, '!');

	//strlen((const char*)command)
	// Send to UART0 using UARTSend. Testing purposes.
	UARTSend((unsigned long)UART0_BASE, (unsigned char *) "ATD7875042871;", 14);
	UARTCharPut(UART0_BASE, 13);

	i=seg;
	while(i--){}
	UARTSend((unsigned long)UART1_BASE, (unsigned char *) "ATH",3);
	UARTCharPut(UART1_BASE, 13);
	UARTCharPut(UART1_BASE, 26);

	i=seg;
	while(i--){}
	UARTSend((unsigned long)UART1_BASE, (unsigned char *) "AT+CREG?", 8);
	UARTCharPut(UART1_BASE, 13);
	UARTCharPut(UART1_BASE, 26);


	i=seg;
	while(i--){}

	while(1){}

	return 0;
}


