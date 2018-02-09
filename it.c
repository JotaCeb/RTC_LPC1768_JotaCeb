#include "LPC17xx.h"
#include <stdio.h>

static int32_t dec_7seg[]={0x40,0x79,0x24,0x30,0x19,0x12,0x03,0x78,0x00,0x18};			//Array de números
int32_t conmutacion;

static int aux_int, ano;	
static char segundos, minutos, horas, dia, mes;																			//Uds de tiempo del RTC

static char flag_cambio;

void RTC_IRQHandler (void){
	LPC_RTC->ILR = (1<<0);									//clears the counter increment interrupt seg, min o la que esté configurada
	LPC_RTC->ILR = (1<<1);									//clears the alarm interrupt
	
	aux_int = LPC_RTC->CTIME0;							//Registro de tiempos de segundos, minutos, horas y día de la semana
	segundos = aux_int & 0x0000003F;				//Lectura de segundos	
	
	aux_int = LPC_RTC->CTIME0;							//Registro de tiempos de segundos, minutos, horas y día de la semana
	aux_int = aux_int & 0x00003F00;
	minutos = aux_int >> 8;									//Lectura de minutos
	
	aux_int = LPC_RTC->CTIME0;							//Registro de tiempos de segundos, minutos, horas y día de la semana
	aux_int = aux_int & 0x001F0000;
	horas = aux_int >> 16;									//Lectura de horas
	
	aux_int = LPC_RTC->CTIME1;							//Registro de tiempos de día de mes, mes y año
	dia = aux_int & 0x0000001F;							//Lectura de dia									
	
	aux_int = LPC_RTC->CTIME1;							//Registro de tiempos de día de mes, mes y año
	aux_int = aux_int & 0x00000F00;
	mes = aux_int >> 8;											//Lectura de mes
	
	aux_int = LPC_RTC->CTIME1;							//Registro de tiempos de día de mes, mes y año
	aux_int = aux_int & 0x0FFF0000;
	ano = aux_int >> 16;										//Lectura de año
	
}

void TIMER0_IRQHandler(void){
	char aux_segundos_ano, aux_minutos_mes, aux_horas_dia;																//Uds de tiempo del RTC
	
	LPC_TIM0->IR |= 1<<0;																																	//Limpiar flag de interrupción
	
	switch (conmutacion)
	{
		case 0:								//segundos-años, dígito 1
		if (flag_cambio == 0)	{
			aux_segundos_ano = (segundos%10); 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_segundos_ano] << 20)|(1<<8)|(1<<1)|(1<<0);			//También se apagan los puntos de los displays (dp)
		}
		else{
			aux_segundos_ano = ((ano%1000)%100)%10; 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_segundos_ano] << 20)|(1<<8)|(1<<1)|(1<<0);			//También se apagan los puntos de los displays (dp)
		}
		break;	
		case 1:								//segundos-años, dígito 2
		if (flag_cambio == 0)	{
			aux_segundos_ano = (segundos/10); 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_segundos_ano] << 20)|(1<<9)|(1<<1)|(1<<0);			//También se apagan los puntos de los displays (dp)
		}
		else{
			aux_segundos_ano = ((ano%1000)%100)/10; 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_segundos_ano] << 20)|(1<<9)|(1<<1)|(1<<0);			//También se apagan los puntos de los displays (dp)
		}
			break;	
		case 2:								//minutos-mes, dígito 3
		if (flag_cambio == 0)	{
			aux_minutos_mes = (minutos%10); 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_minutos_mes] << 20)|(1<<10);			//También se encienden los puntos de los displays (dp)
		}
		else{
			aux_minutos_mes = (mes%10); 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_minutos_mes] << 20)|(1<<10);			//También se encienden los puntos de los displays (dp)
		}
		break;	
		case 3:								//minutos-mes, dígito 4
		if (flag_cambio == 0)	{
			aux_minutos_mes = (minutos/10); 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_minutos_mes] << 20)|(1<<14)|(1<<1)|(1<<0);			//También se apagan los puntos de los displays (dp)
		}
		else{
			aux_minutos_mes = (mes/10); 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_minutos_mes] << 20)|(1<<14)|(1<<1)|(1<<0);			//También se apagan los puntos de los displays (dp)
		}
			break;	
				case 4:								//horas-días, dígito 5
		if (flag_cambio == 0)	{
			aux_horas_dia = (horas%10); 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_horas_dia] << 20)|(1<<15);				//También se encienden los puntos de los displays (dp)
		}
		else{
			aux_horas_dia = (dia%10); 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_horas_dia] << 20)|(1<<15);				//También se encienden los puntos de los displays (dp)
		}
		break;	
		case 5:										//horas-días, dígito 6
		if (flag_cambio == 0)	{
			aux_horas_dia = (horas/10); 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_horas_dia] << 20)|(1<<16)|(1<<1)|(1<<0);				//También se apagan los puntos de los displays (dp)
		}
		else{
			aux_horas_dia = (dia/10); 
			LPC_GPIO1->FIOPIN &= ~(0x1C703);																									//Todos los ánodos comunes a 0
			LPC_GPIO1->FIOPIN = (dec_7seg[aux_horas_dia] << 20)|(1<<16)|(1<<1)|(1<<0);				//También se apagan los puntos de los displays (dp)
		}
			break;	
}
conmutacion++;
if (conmutacion>=6)
	conmutacion=0;
}

void EINT2_IRQHandler (void){
	
	LPC_SC->EXTINT=(1<<2);										//Borra flag externo
	
	if (flag_cambio == 0) flag_cambio = 1;
	else flag_cambio = 0;

}


