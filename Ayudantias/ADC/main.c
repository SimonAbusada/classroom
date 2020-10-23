//REF ADC y PWM(Timer):
/*
 * name: Ayudantia #1 ADC y PWM
 * Author:
 *     Camila Turrieta
 *     Felipe S�nchez
 *
 * Microcontroller:
 *      MSP430F5529
 * Copyright (c) 2012, Texas Instruments Incorporated
 *
 */

#include <msp430.h>

void config_adc(void);                          //configuracion ADC
void start_conversion(void);                     //para obtener la conversion del ADC
void timer_A(void);                             //configuracion TIMER para PWM

int ADC_x;                                      //guarda valor lectura ADC eje X
int ADC_y;                                      //guarda valor lectura ADC eje Y

int main(void)
{
    WDTCTL = WDTPW+WDTHOLD;                     //Detiene el watchdog

    //Inicializa el ADC y el TIMER-PWM
    config_adc();
    timer_A();

    //ADC
    P6DIR &= ~BIT5;                             //Entrada ADC12 P6.5-eje x
    P6DIR &= ~BIT3;                             //Entrada ADC12 P6.3-eje y
    P6SEL |= BIT3 + BIT5;                       //Habilita el canal ADC (P6.5-P6.3) (puerto con proposito especifico)

    // PWM-TIMER
    P1DIR |= BIT4 + BIT5;                       //outputs: BLUE+GREEN
    P1SEL |= BIT4 + BIT5;                       //selecciona puerto de proposito especifico/general PWM
    P2DIR |= BIT4;                              //outputs: RED
    P2SEL |= BIT4;                              //selecciona puerto de proposito especifico/general PWM


    // Auxiliaries
    int duty_X;
    float ADC_value_X;
    float aux_X;
    int duty_Y;
    float ADC_value_Y;
    float aux_Y;

    while (1){
        //Get ADC value
        start_conversion();

        //EJE X del JOYSTICK genera PWM del pin P1.4
        // Valor del ADC a float
        ADC_value_X = (float) ADC_x;
        //Linealizacion
        aux_X = ADC_value_X*255/4095;
        //Conversion del valor a numero entero
        duty_X = (int) aux_X;
        //Ciclo de trabajo PWM
        TA0CCR3 = duty_X;

        //EJE Y del JOYSTICK genera PWM del pin P1.5
        // Valor del ADC a float
        ADC_value_Y = (float) ADC_y;
        //Linealizacion
        aux_Y = ADC_value_Y*255/4095;
        //Conversion del valor a numero entero
        duty_Y = (int) aux_Y;
        //Ciclo de trabajo PWM
        TA2CCR1 = duty_Y;
    }
}

void config_adc(void)
{
    // Turn on ADC      + multiple sample and conversion
    ADC12CTL0 = ADC12ON + ADC12MSC;

    // ClK = SMLK  SHP = Use sampling timer+ Prescaler + conversion: Sequence-of-channels
    ADC12CTL1 |= ADC12SSEL_3 + ADC12SHP + ADC12DIV_7 + ADC12CONSEQ_1;

    // Reference Voltage (3.3) + PIN P6.5-EJE X
    ADC12MCTL0 |= ADC12SREF_0 + ADC12INCH_5;                    //Canal 0 para ADC eje X

    // Reference Voltage (3.3) + PIN P6.3-EJE Y + End of sequence
    ADC12MCTL1 |= ADC12SREF_0 + ADC12INCH_3 + ADC12EOS;         //Canal 1 para ADC eje Y

    // Enable conversion
    ADC12CTL0 |= ADC12ENC;
}

void start_conversion(void)
{
    // Star conversion
    ADC12CTL0 |= ADC12SC;

    // Wait conversion
    while(ADC12CTL1 & ADC12BUSY);

    // Return ADC value
    ADC_x = ADC12MEM0;          //Canal 0: eje x se lee en P6.5
    ADC_y = ADC12MEM1;          //Canal 1: eje y se lee en P6.3
}


void timer_A(void)
{
    //REGISTRO PRINCIPAL TIMER 0
    // TA0CTL control register:
    // SMCLK = 1.048 MHz - Prescaler = 8 - UPMODE
    TA0CTL |= TASSEL_2 + ID_3 + MC_1;
    TA0CCR0 = 255;          //Top

    // Output mode PIN 1.4: BLUE
    TA0CCTL3 |= OUTMOD_7;
    //flag
    TA0CCR3 = 0;

    // Output mode PIN 1.5: GREEN
    TA0CCTL4 |= OUTMOD_7;
    //flag
    TA0CCR4 = 0;

    //REGISTRO PRINCIPAL TIMER 2
    // SMCLK = 1.048 MHz - Prescaler = 8 - UPMODE
    TA2CTL |= TASSEL_2 + ID_3 + MC_1;
    TA2CCR0 = 255;          //Top

    // Output mode PIN 2.4: RED
    TA2CCTL1 = OUTMOD_7;
    //flag
    TA2CCR1 = 0;
}
