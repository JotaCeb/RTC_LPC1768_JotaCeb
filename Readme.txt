Este proyecto permite realizar un reloj con un hardware muy simple. 
Sólo es necesaria alimentación, elementos pasivos para los diodos 
de los displays, cableado, un LPC1768 (Blueboard en este caso) y 
dos D5631AB (display de 3 dígitos).

Las conexiones necesarias son las siguientes (también aparecen en
el main.c)

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

El sistema de CALIBRACIÓN permite sumar +1 segundo o +2 segundos en 
caso de que la carga del procesador (interrupciones u otros), vaya
retrasando en el tiempo el contador de tiempo del RTC. 

/*************************************************************************************************/

En caso de querer configurar la ALARMA, cada uno de los registros
de la misma (ALSEC, ALMIN,...) deben coincidir con los registros 
homólogos de cuenta del RTC (CTIMEx), que son SEC, MIN,... 