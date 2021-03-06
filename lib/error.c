/*
 * error.c
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
#include <avr/pgmspace.h>

#include <error.h>

/** \file error.c
 * Global listing of different error conditions.
 * Can be returned to signalise error or success.
 * Also allows to print human-readable error descriptions.
 */

char PROGMEM error0[] = "Success"; /**< String for SUCCESS */
char PROGMEM error1[] = "TWI doesn't answer"; /**< String for TWI_NO_ANSWER */
char PROGMEM error2[] = "TWI could not write"; /**< String for TWI_WRITE_ERROR */
char PROGMEM error3[] = "Not enough memory"; /**< String for MALLOC_FAIL */
char PROGMEM error4[] = "General Error"; /**< String for ERROR */
char PROGMEM error5[] = "Argument Error"; /**< String for ARGUMENT_ERROR */

/** Array of all error descriptions in Flash Memory */
PGM_P PROGMEM errorTable[] = {
    error0, error1, error2, error3, error4, error5
};

/** Returns a human-readable error description.
 * Free the string after use!
 */
char *getErrorString(Error e) {
    char *buff = (char *)malloc(strlen_P((PGM_P)pgm_read_word(&(errorTable[e]))));
    if (buff == NULL) {
        return NULL;
    }
    strcpy_P(buff, (PGM_P)pgm_read_word(&(errorTable[e])));
    return buff;
}
