/***************************************************
 * This is an example program to demonstrate the
 * successful implementation of the USART's
 * receive functions.
 *
 * Connect your developer board to a PC and configure
 * a terminal emulator with the appropriate baud rate
 * in order to see the test message below.
 *
 * Hint: Disconnect your terminal software when loading
 * a new program into the flash!
 *
 * This code is in the public domain.
 *
 ****************************************************
 * Codigo base Laboratorio 5 SEP
 * Codigo modificado por: Javier Silva Orellana
 * Fecha 28-10-2020
 ****************************************************/

/*
 * LIBRERIAS
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <msp430.h>
#include "USART_implement_me.h"


/*
 * VARIABLES
 */
unsigned int pos;
char lista[26] = "QWERTYUIOPASDFGHJKLZXCVBNM";
unsigned int time_limit = 0;
unsigned int game_counter = 0;

/*
 * PROTOTYPES
 */
void Timer_Init(void);

/*
 * MAIN LOOP
 */
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;               //Stop WDT
    struct USART_configuration config_9600_8N1 = {9600, 8, 0, 1};
    USART_Init(config_9600_8N1);           //UART Init
    Timer_Init();                          //Timer Init

    //Welcome message
    USART_Transmit_char('B');
    USART_Transmit_char('i');
    USART_Transmit_char('e');
    USART_Transmit_char('n');
    USART_Transmit_char('v');
    USART_Transmit_char('e');
    USART_Transmit_char('n');
    USART_Transmit_char('i');
    USART_Transmit_char('d');
    USART_Transmit_char('x');
    USART_Transmit_char(' ');
    USART_Transmit_char('a');
    USART_Transmit_char('l');
    USART_Transmit_char(' ');
    USART_Transmit_char('N');
    USART_Transmit_char('e');
    USART_Transmit_char('r');
    USART_Transmit_char('v');
    USART_Transmit_char('i');
    USART_Transmit_char('o');
    USART_Transmit_char('s');
    USART_Transmit_char('o');
    USART_Transmit_char('!');
    USART_Transmit_char('\r');
    USART_Transmit_char('\n');

    //Selección de dificultad
    USART_Transmit_char('E');
    USART_Transmit_char('l');
    USART_Transmit_char('i');
    USART_Transmit_char('g');
    USART_Transmit_char('e');
    USART_Transmit_char(' ');
    USART_Transmit_char('l');
    USART_Transmit_char('a');
    USART_Transmit_char(' ');
    USART_Transmit_char('d');
    USART_Transmit_char('i');
    USART_Transmit_char('f');
    USART_Transmit_char('i');
    USART_Transmit_char('c');
    USART_Transmit_char('u');
    USART_Transmit_char('l');
    USART_Transmit_char('t');
    USART_Transmit_char('a');
    USART_Transmit_char('d');
    USART_Transmit_char(' ');
    USART_Transmit_char('(');
    USART_Transmit_char('E');
    USART_Transmit_char('/');
    USART_Transmit_char('M');
    USART_Transmit_char('/');
    USART_Transmit_char('H');
    USART_Transmit_char(')');
    USART_Transmit_char(':');
    USART_Transmit_char('\r');
    USART_Transmit_char('\n');
    char c = USART_Receive_char();       //El terminal se demora como 1s en printear
    if (c == 'H'){time_limit = 120;}     //HARD:     0.2s
    else if (c== 'M'){time_limit = 150;} //MEDIUM:   0.5s
    else{time_limit=200;}                //EASY:      1s

    //Activamos las interrupciones
    __bis_SR_register( GIE );
    while(1)
    {
        //Envía letra
        srand(time(0));
        pos = rand()%26;
        USART_Transmit_char('P');
        USART_Transmit_char('r');
        USART_Transmit_char('e');
        USART_Transmit_char('s');
        USART_Transmit_char('i');
        USART_Transmit_char('o');
        USART_Transmit_char('n');
        USART_Transmit_char('a');
        USART_Transmit_char(' ');
        USART_Transmit_char(lista[pos]);
        USART_Transmit_char('!');
        USART_Transmit_char('\r');
        USART_Transmit_char('\n');

        //Recepción de respuesta
        TA0CCTL0 = CCIE;                    //Timer ON
        char ans = USART_Receive_char();    //UART Rx
        TA0CCTL0 &= ~CCIE;                  //Timer OFF

        //Filtro 1: Responde a tiempo
        if (game_counter < time_limit)
        {
            //Filtro 2: Verifica respuesta
            if (ans != lista[pos])
            {
                USART_Transmit_char('I');
                USART_Transmit_char('N');
            }
            USART_Transmit_char('C');
            USART_Transmit_char('O');
            USART_Transmit_char('R');
            USART_Transmit_char('R');
            USART_Transmit_char('E');
            USART_Transmit_char('C');
            USART_Transmit_char('T');
            USART_Transmit_char('O');
            USART_Transmit_char('!');
            USART_Transmit_char('\r');
            USART_Transmit_char('\n');
        }
        else
        {
            USART_Transmit_char('M');
            USART_Transmit_char('U');
            USART_Transmit_char('Y');
            USART_Transmit_char(' ');
            USART_Transmit_char('L');
            USART_Transmit_char('E');
            USART_Transmit_char('N');
            USART_Transmit_char('T');
            USART_Transmit_char('O');
            USART_Transmit_char('!');
            USART_Transmit_char('\r');
            USART_Transmit_char('\n');
        }
        game_counter = 0;
    }
}

/*
 * Inicializacion del timer
 */
void Timer_Init(void)
{
    TA0CCTL0 = CCIE;                 //Habilitamos interrupciones
    TA0CTL = TASSEL_2 + ID_3 + MC_1; //SMCLK/8, modo UP
    TA0CCR0 = 1310;                  //Cuenta 10ms
}

/*
 * INTERRUPCION DEL TIMER
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
{
    game_counter++;
}
