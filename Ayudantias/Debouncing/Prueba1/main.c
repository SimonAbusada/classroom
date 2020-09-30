//Librerias
#include <msp430F5529.h>

#define estado_boton !(P6IN & BIT0)

//Funciones
void io_config();

/**
 * main.c
 */

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	io_config(); //Configuracion de puertos

	char estado = 1;
    char contador = 0;
    long int i;

	while(1){  //void loop

	    if(estado_boton){ //Trigger pulsador

	        /* Solucion?
	         *
	         */
	        //

	        __delay_cycles(50000);

	        //Codigo para contar veces que se repite
	        for(i = 5000;i > 0;i--){
	            if(estado != estado_boton){
	                contador++;
	                estado = estado_boton;
	            }
	        }

            //Mostrando contador
            while(contador > 0){
                P1OUT |=  BIT0;
                __delay_cycles(100000);
                P1OUT &= ~BIT0;
                __delay_cycles(100000);
                contador--;
            }

            //Verde para indicar contacto total
            P4OUT |=  BIT7;
            __delay_cycles(200000);
            P4OUT &= ~BIT7;
            __delay_cycles(100000);

            /* Delay estrategico para no tomar
             * switch bouncing de subida.
             */
	        __delay_cycles(800000);
            estado = 1;
            contador = 0;
	    }

	}
	return 0;
}


void io_config(){
    //Leds rojo 1.0   verde 4.7
    //Pulsador aluminio 6.0
    P1DIR |=  BIT0;
    P4DIR |=  BIT7;

    //puerto de entrada
    P6DIR &= ~BIT0;
    P6REN |=  BIT0;
    P6OUT |=  BIT0;

    //Luces inicialmente apagadas
    P4OUT &= ~BIT7;
    P1OUT &= ~BIT0;

}
