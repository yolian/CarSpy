#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "utils/uartstdio.h"
#include "string.h"
#include "stdio.h"

//int PinData=2;

int main(void)
{

	//unsigned char cThisChar;
	//char cThis2Char;
	//char command[] = "AT";
	volatile char command[15];
	volatile char response[100];
	//const char *ATcom = "AT";
	volatile unsigned long a = 0;
	int flag = 0;
	int delay = 200000;
	int l;

	//char charArray[];
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	//Maybe LED usage.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the GPIO pin muxing for the UART function.
    // This is only necessary if your part supports GPIO pin function muxing.
    // Study the data sheet to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using
    //
    GPIOPinConfigure(0x00000001);//GPIO_PA0_U0RX
    GPIOPinConfigure(0x00000401);//GPIO_PA1_U0TX
    GPIOPinConfigure(0x00010001);//GPIO_PB0_U0RX
    GPIOPinConfigure(0x00010401);//GPIO_PB1_U0TX


    //
    // Since GPIO A0 and A1 are used for the UART function, they must be
    // configured for use as a peripheral function (instead of GPIO).
    // TODO: change this to match the port/pin you are using
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_3 | GPIO_PIN_4);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    // This function uses SysCtlClockGet() to get the system clock
    // frequency.  This could be also be a variable or hard coded value
    // instead of a function call.
    // RCC2/RCC
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
    UARTFIFOEnable(UART1_BASE);
    UARTFIFOLevelSet(UART1_BASE,UART_FIFO_TX1_8,UART_FIFO_RX1_8);

    UARTFIFOEnable(UART0_BASE);
    UARTFIFOLevelSet(UART0_BASE,UART_FIFO_TX1_8,UART_FIFO_RX1_8);
    //
    // Put a character to show start of example.  This will display on the
    // terminal.
    //

    UARTCharPut(UART0_BASE, '!');


    //
    // Enter a loop to read characters from the UART, and write them back
    // (echo).  When a line end is received, the loop terminates.
    //
//    do
    while(1){
        //
        // Read a character using the blocking read function.  This function
        // will not return until a character is available.
        //

    	if(flag)
    	{
    		UARTEnable(UART1_BASE);
    		UARTEnable(UART0_BASE);
    		flag = 0;
    	}

    	while(!UARTCharsAvail(UART0_BASE))
    	{}
    	// Hubo que usar esto a diferencia de UARTCharGetNonBlocking, porque seguia llenando el
    	//array de command con basura.
    	while(UARTCharsAvail(UART0_BASE))
    	{
    		command[a++] = UARTCharGetNonBlocking(UART0_BASE);
    		SysCtlDelay(delay);
    		//UARTCharPut(UART0_BASE, command[a]); TESTING
    	}

    	command[a] = '*';
    	a=0;

    	//Testing to see what's saved in the command array
//    	while(command[a] != '*')
//    	{
//    		UARTCharPut(UART0_BASE, command[a]);
//    		a++;
//    	}
//    	a=0;
    	//sprintf(command, "%s ", &ATcom);
//    	l = strlen(command);
    	//Write the command into the transmit buffer of UART 1. Send to attached device.
    	while(command[a] != '*')//a < l
    	{
    		UARTCharPut(UART1_BASE, command[a]);
    		a++;
    		SysCtlDelay(delay);
    	}
    	a=0;

//    	while(a < 15)
//    	{
//    		command[a] = ' ';
//    		a++;
//    	}
//    	a=0;
//    	while(1)
//    	{
//    		UARTCharPut(UART1_BASE, 0x55);
//    	}
    	//After command is sent, await a response from the device.

    	while(!UARTCharsAvail(UART1_BASE))
    	{}
    	// Hubo que usar esto a diferencia de UARTCharGetNonBlocking, porque seguia llenando el
    	//array de command con basura.
    	while(UARTCharsAvail(UART1_BASE))
    	{
    		response[a] = UARTCharGet(UART1_BASE);
    		a++;
    		//SysCtlDelay(delay);
    	}
    	response[a] = '*';
    	a=0;

//    	UARTDisable(UART0_BASE);
//    	UARTEnable(UART0_BASE);
    	while(response[a] != '*')
    	{

    		UARTCharPut(UART0_BASE, response[a]);
    		a++;
      		//SysCtlDelay(delay);

    	}
    	a=0;
    	l = sizeof(response);
    	while(a < l)
        {
        	response[a] = ' ';
        	a++;
        }
        a=0;
        flag = 1;
        UARTDisable(UART1_BASE);
        UARTDisable(UART0_BASE);
        //
        // Write the same character using the blocking write function.  This
        // function will not return until there was space in the FIFO and
        // the character is written.
        //

//        	UARTCharPut(UART0_BASE, cThisChar);
        	//UARTCharPut(UART0_BASE, cThis2Char);

    //
    // Stay in the loop until either a CR or LF is received.
    //
    }// DA WHILE LOOP FINISHES HERE



//    } while((cThisChar != '\n') && (cThisChar != '\r'));

    //
    // Put a character to show the end of the example.  This will display on
    // the terminal.
    //
    UARTCharPut(UART0_BASE, '@');


    //
    // Return no errors
    //
    return(0);
//	while(1)
//	{
////		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, PinData);
////		SysCtlDelay(2000000);
////		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
////		SysCtlDelay(2000000);
////		if(PinData==8) {PinData=2;} else {PinData=PinData*2;}
//	}
}
