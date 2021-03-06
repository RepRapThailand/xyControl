/*
 * xycontrol.c
 *
 * Copyright (c) 2013, Thomas Buck <xythobuz@me.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <serial.h>
#include <spi.h>
#include <time.h>
#include <xmem.h>
#include <xycontrol.h>
#include <twi.h>
#include <adc.h>
#include <uartMenu.h>
#include <tasks.h>
#include <config.h>

/** \addtogroup xycontrol xyControl Hardware
 *  \ingroup System
 *  Controls xyControl On-Board Hardware like LEDs.
 *  @{
 */

/** \file xycontrol.c
 *  xyControl API Implementation.
 */

char PROGMEM helpText[] = "Print this Help"; /**< UART Menu Help Text */
char PROGMEM resetText[] = "Reset MCU"; /**< UART Menu Reset Text */

FILE inFile; /**< FILE for stdin */
FILE outFile; /**< FILE for stdout and stderr */

/** Method used to write to stdout and stderr. */
int uartoutput(char c, FILE *f) {
    // Inject CR here, instead of in the serial library,
    // so we can still do binary transfers with serialWrite()...
    if (c == '\n') {
        for (uint8_t i = 0; i < serialAvailable(); i++)
            serialWrite(i, '\r');
    }
    if (c != '\r') {
        for (uint8_t i = 0; i < serialAvailable(); i++)
            serialWrite(i, c);
    }
    return 0;
}

/** Method used to read from stdin. */
int uartinput(FILE *f) {
    for (;;) {
        for (uint8_t i = 0; i < serialAvailable(); i++) {
            if (serialHasChar(i)) {
                return serialGet(i);
            }
        }
    }
}

void xyInit(void) {
    xmemInit(); // Most important!

    // LEDs
    LED0DDR |= (1 << LED0PIN);
    LED1DDR |= (1 << LED1PIN);
    LED2DDR |= (1 << LED2PIN);
    LED3DDR |= (1 << LED3PIN);
    xyLed(4, 1);

    initSystemTimer();
    for (uint8_t i = 0; i < serialAvailable(); i++) {
        serialInit(i, BAUD(38400, F_CPU));
    }
    twiInit();
    spiInit(MODE_0, SPEED_2);
    adcInit(AVCC);

    addMenuCommand('q', resetText, &xySelfReset);
    addMenuCommand('h', helpText, &uartMenuPrintHelp);
    addTask(&uartMenuTask);

    // fdevopen() is using malloc, so printf in a different
    // memory bank will not work!
    //   fdevopen(&uartoutput, NULL); // stdout & stderr
    //   fdevopen(NULL, &uartinput); // stdin
    // Instead we have the FILE structs as static variables
    // and assign them to stdin, stdout and stderr

    fdev_setup_stream(&outFile, &uartoutput, NULL, _FDEV_SETUP_WRITE);
    fdev_setup_stream(&inFile, NULL, &uartinput, _FDEV_SETUP_READ);
    stdin = &inFile;
    stdout = &outFile;
    stderr = &outFile;

    sei();
}

/** Internal LED Manipulation function.
 *  \param v New LED State (Off, On, Toggle)
 *  \param port The Corresponding Output Port
 *  \param pin The LED Pin
 */
void xyLedInternal(uint8_t v, volatile uint8_t *port, uint8_t pin) {
    if (v == 0) {
        *port &= ~(1 << pin);
    } else if (v == 1) {
        *port |= (1 << pin);
    } else {
        *port ^= (1 << pin);
    }
}

void xyLed(uint8_t l, uint8_t v) {
    if (l == LED_RED0) {
        xyLedInternal(v, &LED0PORT, LED0PIN);
    } else if (l == LED_RED1) {
        xyLedInternal(v, &LED1PORT, LED1PIN);
    } else if (l == LED_GREEN0) {
        xyLedInternal(v, &LED2PORT, LED2PIN);
    } else if (l == LED_GREEN1) {
        xyLedInternal(v, &LED3PORT, LED3PIN);
    } else if (l == LED_ALL){
        xyLed(0, v);
        xyLed(1, v);
        xyLed(2, v);
        xyLed(3, v);
    } else if (l == LED_BITMAP) {
        xyLed(0, v & 0x01);
        xyLed(1, (v & 0x02) >> 1);
        xyLed(2, (v & 0x04) >> 2);
        xyLed(3, (v & 0x08) >> 3);
    } else if (l == LED_GREEN) {
        xyLed(LED_GREEN0, v);
        xyLed(LED_GREEN1, v);
    } else if (l == LED_RED) {
        xyLed(LED_RED0, v);
        xyLed(LED_RED1, v);
    }
}

double getVoltage(void) {
    adcStart(BATT_CHANNEL);
    while(!adcReady());
    uint16_t v = adcGet(0) * BATT_MAX;
    return ((double)v / 1024.0);
}

void xySelfReset(void) {
    wdt_enable(WDTO_15MS);
    for(;;);
}

int64_t map(int64_t value, int64_t oldMin, int64_t oldMax, int64_t newMin, int64_t newMax) {
    return (value - oldMin) * (newMax - newMin) / (oldMax - oldMin) + newMin;
}
/** @} */
