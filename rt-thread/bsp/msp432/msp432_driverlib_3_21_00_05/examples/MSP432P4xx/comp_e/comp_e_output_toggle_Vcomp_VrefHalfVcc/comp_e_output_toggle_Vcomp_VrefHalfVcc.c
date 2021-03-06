/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_21_00_05 
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 * MSP432 Comparator - Comparator Toggle from SLEEP; input channel C2;
 *      Vcompare is compared against Vcc/2
 *
 * Description: Use Comp and shared reference to determine if input 'Vcompare'
 * is high or low.  When Vcompare exceeds Vcc*1/2 COUT goes low and when
 * Vcompare is less than Vcc*1/2 then CEOUT goes high.
 *
 *
 *                 MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST        P7.7/C2|<--Vcompare
 *            |                  |
 *            |         P7.1/COUT|----> 'low'(Vcompare>Vcc*1/2); 'high'(Vcompare<Vcc*1/2)
 *            |                  |  
 *            |                  |  
 *            |                  |
 * Author: Timothy Logan
 ******************************************************************************/
 /* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>

#include <stdbool.h>

/* Comparator configuration structure */
const COMP_E_Config compConfig =
        {
                COMP_E_VREF,                  // Positive Input Terminal
                COMP_E_INPUT2,              // Negative Input Terminal
                COMP_E_FILTEROUTPUT_DLYLVL4,  // Level 4 Filter
                COMP_E_NORMALOUTPUTPOLARITY   // Normal Output Polarity
        };

int main(void)
{
    volatile uint32_t ii;

    /* Stop WDT and disable */
    MAP_WDT_A_holdTimer();

    /*
     * Select Port 7
     * Set Pin 1 to output Primary Module Function, (COUT).
     */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7, GPIO_PIN1,
            GPIO_PRIMARY_MODULE_FUNCTION);
            
    /* Set P7.7 to be comparator in (C0.2) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P7, GPIO_PIN7,
            GPIO_TERTIARY_MODULE_FUNCTION);

    /* Initialize the Comparator E module
     *  Comparator Instance 0,
     *  Pin CE2 to Positive(+) Terminal,
     *  Reference Voltage to Negative(-) Terminal,
     *  Normal Power Mode,
     *  Output Filter On with max delay,
     *  Non-Inverted Output Polarity
     */
    MAP_COMP_E_initModule(COMP_E0_BASE, &compConfig);

    /*Set the reference voltage that is being supplied to the (-) terminal
     *  Comparator Instance 0,
     *  Reference Voltage of Vcc V,
     *  Lower Limit of Vcc*(16/32) = Vcc*1/2, (aprox. 1.65v at Vcc=3.3v)
     *  Upper Limit of Vcc*(16/32) = Vcc*1/2
     */
    MAP_COMP_E_setReferenceVoltage(COMP_E0_BASE, COMP_E_REFERENCE_AMPLIFIER_DISABLED,
                                        16, 16);

    /* Disable Input Buffer on P1.2/CE2
     *  Base Address of Comparator E,
     *  Input Buffer port
     *  Selecting the CEx input pin to the comparator
     *  multiplexer with the CEx bits automatically
     *  disables output driver and input buffer for
     *  that pin, regardless of the state of the
     *  associated CEPD.x bit
     */
    MAP_COMP_E_disableInputBuffer(COMP_E0_BASE, COMP_E_INPUT2);

    /* Allow power to Comparator module */
    MAP_COMP_E_enableModule(COMP_E0_BASE);

    /* Delaying to allow comparator to settle */
    for (ii = 0; ii < 400; ii++);

    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
}
