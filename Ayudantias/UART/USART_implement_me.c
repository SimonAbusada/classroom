/***************************************************
 *This is a USART library for the MSP430.
 *
 * It can provide basic USART (serial) communication for any application.
 *
 * This code is in the public domain. Feel free to do with it whatever you want.
 *
 *
 *
 * FOR STUDENTS:
 *
 * This file will be given to you in an 'empty' state. The function bodies are
 * provided, but not their content. You are supposed to add the proper code
 * and complete these functions.
 *
 * Hint 1: RTFM! The TI device datasheets contain all information necessary.
 *
 * Hint 2: We do not expect you to write the most performant or exceptionally
 * well-engineered code, but it should work. If in doubt, sacrifice speed for
 * reliability. If you are bored, rewrite the entire library in assembler and
 * squeeze the last microsecond out of it.
 *
  ****************************************************
 * Codigo base Laboratorio 5 SEP
 * Codigo modificado por: Jose Gutierrez Molina
 * Fecha 28-05-2020
 ****************************************************/


// MCU Clock Speed - needed for baud rate value computation
#include <msp430.h>
#include <string.h>

#include "USART_implement_me.h"


// The initialisation function.
//La funcion de inicializacion recibe como argumento una estructura.
uint8_t USART_Init(struct USART_configuration config)
{
    //Configuracion de ejemplo UART - 9600baudrate-8bits de datos-parity bit deshabilitado-1 stop bit
    P4SEL |= BIT4+BIT5;                     //Habilitamos la funcion de los puertos P4.4 como Tx y P4.5 como Rx (UART)
    UCA1CTL1 |= UCSWRST;                    //Resetea el modulo (IMPORTANTE)

    switch (config.baudrate) {              //Config Baudrate
    case 9600:
      UCA1CTL1 |= UCSSEL_1;                 //Escoge el clock ACLK

      UCA1BR0 = 3;                          //Constantes obtenidas de tabla 36-4 User's Guide
      UCA1BR1 = 0;
      UCA1MCTL = UCBRS_3 + UCBRF_0;
    break;
    //default:                              //formalidad: poner un caso por default
    }

    switch (config.databits) {              //Config Data bit
    case 7:
      UCA1CTL0 |= (1 << UC7BIT);            //data frame de 7bits
    break;
    case 8:
      UCA1CTL0 &= ~UC7BIT;                  //data frame de 8bits
    break;
    //default:
    }

    switch (config.parity) {                //Config Parity bit
    case 0:
        UCA1CTL0 &= ~UCPEN;                 //bit de paridad desabilitado
    break;
    //default:
    }

    switch (config.stopbits) {              //Config Stop bits
    case 1:
        UCA1CTL0 &= ~UCSPB;                 //1bit de parada
    break;
    //default:
    }

    UCA1CTL1 &= ~UCSWRST;                   //**Initialize USCI state machine**
}


// Transmits a single character
void USART_Transmit_char(uint8_t data)      //Entregamos a la funcion un dato
{
    while (!(UCA1IFG & UCTXIFG));             //Verifica que el buffer de transmision este desocupado.
                                            //Si el buffer esta ocupado, el flag UCTXIFG=0, la operacion UCA1IFG & UCTXIFG=0,
                                            //luego es negada !(UCA1IFG&UCTXIFG)=1 y por lo tanto, el argumento del while es
                                            //un 1, ie, while(TRUE) y el programa se queda en esta linea de codigo dado
                                            //que el buffer esta ocupado.
                                            //En caso que el buffer este desocupado, el flag UCTXIFG=1, la operacion
                                            //UCA1IFG & UCTXIFG=1, luego es negada !(UCA1IFG&UCTXIFG)=0 y por lo tanto el
                                            //argumento de la funcion while es un 0, ie, while(FALSE) y el programa sale
                                            //de esta linea y sigue ejecutando el codigo que sigue.
    UCA1TXBUF = data;                       //Cargamos el dato al buffer de transmision
}


// Receives a single character
char USART_Receive_char(void)               //funcion no recibe nada como argumento y retorna un dato tipo char(dato recibido)
{
    while(!(UCA1IFG & UCRXIFG));            //Misma logica que antes. Cuando se recibe un caracter, UCRXIFG=1,
                                            //la operacion UCA1IFG & UCRXIFG = 1, luego es negada !(UCA1IFG & UCRXIFG)=0 y
                                            //finalmente el argumento del while es un 0, ie, while(FALSE) y el programa
                                            //sale de esta linea y ejecuta el resto de codigo.
    char data = UCA1RXBUF;                  //asigna a nuestra variable data (del tipo char) el dato recibido
    return data;                            //la funcion retorna la variable data que contiene el caracter recibido
}



// Receives a '\n' terminated string and writes it into a supplied buffer.
// The buffer must be guaranteed to handle at least bufflen bytes.
// Returns the number of bytes written into the buffer. (If you want)
uint8_t USART_Receive_String(/* Fill here*/)
{
    //TAREA DE USTEDES
    //UTILIZAR BUFFER SEGUN LOS REQUERIMIENTOS DE LA APLICACION/TAREA
}


// Transmits a given string
void USART_Transmit_String(char* string)
{
    //TAREA DE USTEDES

}
