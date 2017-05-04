#define F_CPU 32000000UL // 32 MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/sleep.h>
#include <stdio.h>

#include "aes.h"

//Activar reloj interno de 32 MHz
void initClock(){
	CCP = CCP_IOREG_gc;              //Disable register security for oscillator update
    OSC.CTRL = OSC_RC32MEN_bm;       //Enable 32MHz oscillator
    while(!(OSC.STATUS & OSC_RC32MRDY_bm)); //Wait for oscillator to be ready
    CCP = CCP_IOREG_gc;              //Disable register security for clock update
    CLK.CTRL = CLK_SCLKSEL_RC32M_gc; //Switch to 32MHz clock
}

//Activar cristal externo
void initExtClock(){
	OSC_XOSCCTRL = OSC_FRQRANGE_2TO9_gc | OSC_XOSCSEL_XTAL_16KCLK_gc; 	//External crystal, 2 to 9 Mhz
	OSC_CTRL |= OSC_XOSCEN_bm; 	//Enable external oscillator
	while(!(OSC_STATUS & OSC_XOSCRDY_bm)); 	//Wait for clock stabilization
	CCP = CCP_IOREG_gc; //Disable register security for clock update
	CLK_CTRL = CLK_SCLKSEL_XOSC_gc;  //Switch to external clock
}

//Inicializar la UART, en función de la velocidad de clock
void initUart(uint8_t slow){
	//PIN3 (TXD0) as output. 
	PORTD.DIRSET = PIN7_bm;
	//PIN2 (RXD0) as input. 
	PORTD.DIRCLR = PIN6_bm;
	
	USARTD1.CTRLA = 0;
	USARTD1.CTRLC = (uint8_t) USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc;
	
	//Set Baudrate to 115200 bps
	if(slow){
		USARTD1.BAUDCTRLA = (428 & 0xff) << USART_BSEL_gp;
		USARTD1.BAUDCTRLB = ((-7) << USART_BSCALE_gp) | ((428 >> 8) << USART_BSEL_gp);
	}else{
		USARTD1.BAUDCTRLA = 131;
		USARTD1.BAUDCTRLB = 0b1101 << USART_BSCALE0_bp;
	}

	//Enable RX and TX
	USARTD1.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}

//Imprimir caracter a UART
int uart_putchar(char data){
	while((USARTD1.STATUS & USART_DREIF_bm) == 0);
	USARTD1.DATA = data;
	return 0;
}
int uart_putchar_printf(char data, FILE *stream){
	if(data == '\n') uart_putchar('\r');
	return uart_putchar(data);
}
//Leer caracter de UART
char uart_getchar(){
	while((USARTD1.STATUS & USART_RXCIF_bm) == 0);
	return USARTD1.DATA;
}

//Necesario para usar printf
FILE uart_output = FDEV_SETUP_STREAM(uart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

//Clave de cifrado AES128
const uint8_t clave[16] = "1234estaesunapru";
#define BIT_TRIGGER (0)

int main(){
	initClock();
	initUart(0);
	
	stdout = &uart_output; //Permite el uso de printf via UART
	
	_delay_ms(100);
	
	printf("Reloj Interno\n");
	
	initExtClock();
	initUart(1);
	
	printf("Reloj Externo\n");
	
	uint8_t mensaje[16], cifrado[16];
	uint8_t i;
	
	//Señal de disparo como salida
	PORTE.DIRSET = 0xFF;
	PORTE.OUT = (1<<BIT_TRIGGER);
		
	while(1){	
		while(uart_getchar() != 0); //Busco byte 0 para sincronizar
		
		//Leo 16 bytes a cifrar
		for(i=0;i<16;i++)
			mensaje[i] = uart_getchar();
		
		//Disparo el trigger y cifro
		PORTE.OUT = 0x00;
		AES128_ECB_encrypt(mensaje, clave, cifrado);
		PORTE.OUT = (1<<BIT_TRIGGER);
		
		uart_putchar(0x00); //Envio byte 0 para sincronizar
		
		//Envio 16 bytes cifrados
		for(i=0;i<16;i++)
			uart_putchar(mensaje[i]);
		
		_delay_ms(10);
	}
}
