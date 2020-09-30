#include <msp430F5529.h>

#define PULSADO      1
#define NO_PULSADO   0
#define ACTIVO       1
#define NO_ACTIVO    0

//Funciones
void io_config();
void debouncing();

//Variables globales
char estado_ant = NO_PULSADO;
char estado_act = NO_PULSADO;
char estado_contar = NO_ACTIVO;
char estado_boton;
int  contador = 0;

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	io_config();

	while(1) //void loop
	{
	    /*
	     * Esta funcion de debouncing requiere entrar
	     * siempre en un loop para iterar.
	     */

	    debouncing();


	    if(estado_act == PULSADO){ //Rojo
	        P1OUT |=  BIT0;
	        P4OUT &= ~BIT7;
        }
        else if(estado_act == NO_PULSADO){ //Verde
            P4OUT |=  BIT7;
            P1OUT &= ~BIT0;
        }

	   /* Aqui probablemente tendran su codigo
	    * que hace algo con el estado
	    * real del pulsador
	    */
	}

	return 0;
}


void debouncing(){
    estado_boton = !(P6IN & BIT0);
    if( (estado_ant != estado_boton) || (estado_contar == ACTIVO) ){
        if( (estado_ant == estado_boton) && (estado_contar == ACTIVO) ) //Si se mantiene...
        {
            contador++;
        }
        else if( (estado_ant != estado_boton) && (estado_contar == NO_ACTIVO) ) //Primer cambio
        {
            estado_ant = estado_boton;
            estado_contar = ACTIVO;
            contador++;
        }
        else // (estado_ant != estado_boton) && (estado_contar == ACTIVO)
        {
            estado_ant =  estado_boton;  //Cambia el boton y estamos activos?
            estado_contar = NO_ACTIVO;   //Deja de ser candidato a cambio de estado.
            contador = 0;
        }
        if(contador > 100){ //Esta cantidad es variable.
            estado_act = estado_ant;
            contador = 0;
            estado_contar = NO_ACTIVO;
        }
    }
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

    P4OUT |=  BIT7; //Avisar config lista
    P1OUT |=  BIT0;
    __delay_cycles(1200000);
    // Luces inicialmente apagadas
    // Programa corriendo
    P4OUT &= ~BIT7;
    P1OUT &= ~BIT0;

}
