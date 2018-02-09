#include <stdio.h>
#include "LPC17xx.h"

/****************************************Pines involucrados****************************************

P2.12 => Interrupción externa (SW2 - para cambio entre seg, min, horas y dd/mm/aa)

Puntos entre dígitos (hora.min.seg)
				P1.0 		=> min.seg							
				P1.1 		=> hora.min	

Seg, min, hora (P1.10, P1.9, P1.8)
				(0,0,1) => Dígito 1
				(0,1,0) => Dígito 2
				(1,0,0) => Dígito 3

	dd/mm/aa (P1.16, P1.15, P1.14)
				(0,0,1) => Dígito 4
				(0,1,0) => Dígito 5
				(1,0,0) => Dígito 6

P1.20	=> A
P1.21	=> B
P1.22	=> C
P1.23	=> D
P1.24	=> E
P1.25	=> F
P1.26	=> G

**************************************************************************************************/

//Funciones de inicialización
static void Init_RTC(void);									//Inicialización del RTC
static void Init_GPIO(void);								//Inicialización I/O
static void	Init_EINT2(void);								//Inicialización Entrada externa 2
static void Init_TIM0 (void);								//Inicialización del TIM0
static void Init_NVIC (void);								//Configuración de interrupciones en el NVIC

int main (){

	Init_RTC();	
	Init_GPIO();
	Init_EINT2();	
	Init_TIM0();
	Init_NVIC ();	
	
	while (1);

}
void Init_RTC(void){
	
	SystemCoreClockUpdate();							//Establecimiento de la frecuencia de reloj principal de acuerdo a la configuración actual del PLL	

	//In CCR all NC bits in this register should be initialized when the RTC is first turned on. Bits 0, 2, 3 y 4
	LPC_RTC->CCR &= ~(1<<0);							//CLKEN debe resetearse 
	LPC_RTC->CCR |= (1<<1);								//CTCRST se resetea con un 1
	LPC_RTC->CCR &= ~(1<<2);							//Must be 0 for normal RTC operation
	LPC_RTC->CCR &= ~(1<<3);							//Must be 0 for normal RTC operation
	LPC_RTC->CCR |= (1<<4);								//CCALEN se resetea o deshabilita con un 1
	
	//Hora inicial
	LPC_RTC->SEC = 0x00;									//0 seg
	LPC_RTC->MIN = 0x2A;									//	min
	LPC_RTC->HOUR = 0x0C;									// 	horas
	LPC_RTC->DOM	= 0x1A;									//Número de día del mes
	LPC_RTC->DOW = 0x5;										//Número de día de la semana
	LPC_RTC->DOY = 0x1A;									//Número de día del año
	LPC_RTC->MONTH = 0x01;
	LPC_RTC->YEAR = 0x7E2;
	
	LPC_RTC->CIIR |= (1<<0);							//Se genera una interrupción cada minuto
	LPC_RTC->CCR &= ~(1<<1);							//CTC Reset: the elements in the internal oscillator divider are reset, and remain reset until CCR[1] is changed to zero
	LPC_RTC->CCR |= (1<<0);								//The time counters are enabled	

	//Alarma
	LPC_RTC->ALSEC = 0x0F;								//0 seg
	LPC_RTC->ALMIN = 0x2B;								//	min
	LPC_RTC->ALHOUR = 0x0C;								// 	horas
	LPC_RTC->ALDOM	= 0x1A;								//Número de día del mes
	LPC_RTC->ALDOW = 0x5;									//Número de día de la semana
	LPC_RTC->ALDOY = 0x1A;								//Número de día del año
	LPC_RTC->ALMON = 0x01;
	LPC_RTC->ALYEAR = 0x7E2;
	LPC_RTC->AMR = 0x00;									//Alarma conectada para cada uno de los registros de tiempo

//	LPC_RTC->CCR &= ~(1<<4);							//The calibration counter is enabled
//	LPC_RTC->CALIBRATION |= (0x00FF);			//CALVAL (16:0) Poner cualquier número mayor o igual a 1
//	LPC_RTC->CALIBRATION |= (1<<17);			//CALDIR (17) igual a 1
	
}
void Init_GPIO(void){
	
	LPC_PINCON->PINSEL4 |= (1<<24); 																		//P2.12 como entrada externa EXT 2	
	//Por otro lado, en el inicio TIM0 se queda alimentado. No hace falta PINCON
	//El PINSEL tiene por defecto el puerto 1 como salida
	LPC_PINCON->PINMODE2 = (0xA<<0) | (0x2A<<16) | (1<<29) | (1<<31);		//Neither pull up nor pull down en P1 en los pines: 0,1,8,9,10,14,15
	LPC_PINCON->PINMODE3 = 0x2 | (0x2AAA<<8);														//Neither pull up nor pull down en P1 en los pines: 16 y del 20 al 26
	LPC_PINCON->PINSEL0 |= (1<<2);																			//Sin dividir la frecuencia de interrupción del TIM0 por nada
	
	LPC_GPIO1->FIODIR = 0x3 | (0x7<<8) | (0x7<<14) | (0x7f<<20);				//Puerto 1 como salida en los pines: 0,1,8,9,10,14,15,16 y del 20 al 26

	//FIOPINLos dp o puntos de los displays se quedan a nivel bajo para estar activados
	
}
void Init_EINT2(void){

	LPC_SC->EXTMODE|=(1<<2);
	LPC_SC->EXTPOLAR=0;										//Por flanco de bajada

}	
void Init_TIM0 (void) {
	
	LPC_TIM0->MR0 |= 0x14585;												//83333 para conseguir muy aproximadamente 600Hz de frecuencia de interrupción para los dígitos
	LPC_TIM0->MCR |= 0x3;														//El TIM0 genera una interrupción cuando TC alcanza MRC. El TC se resetea cuando lo alcanza
	LPC_TIM0->TCR |= 1<<0;													//Cuenta habilitada
	
}
void Init_NVIC (void){									//Inicialización de reloj 1

	NVIC_SetPriorityGrouping (2);
	
	NVIC_SetPriority(RTC_IRQn, 1);				//Establece la prioridad de interrupción para RTC
	NVIC_EnableIRQ(RTC_IRQn);							//Habilita la interrupción en el NVIC
	
	NVIC_SetPriority(EINT2_IRQn, 2);			//Establece la prioridad de interrupción para EINT2
	NVIC_EnableIRQ (EINT2_IRQn);					//Habilita la interrupción en el NVIC
	
	NVIC_SetPriority(TIMER0_IRQn, 3);			//Establece la prioridad de interrupción para EINT0
	NVIC_EnableIRQ (TIMER0_IRQn);					//Habilita la interrupción en el NVIC
	
}




