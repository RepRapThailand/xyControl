/*
 * tasks.c
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
#include <stdlib.h>
#include <stdint.h>

#include <xmem.h>
#include <tasks.h>

/** \addtogroup task Task Handler
 *  \ingroup System
 *  @{
 */

/** \file tasks.c
 *  Task API Implementation.
 */

TaskElement *taskList = NULL; /**< List of registered Tasks */

uint8_t tasksRegistered(void) {
    uint8_t c = 0;
    MEMSWITCH(BANK_GENERIC);
    for (TaskElement *p = taskList; p != NULL; p = p->next) {
        c++;
    }
    MEMSWITCHBACK(BANK_GENERIC);
    return c;
}

uint8_t addTask(Task func) {
    MEMSWITCH(BANK_GENERIC);
    TaskElement *p = (TaskElement *)malloc(sizeof(TaskElement));
    if (p == NULL) {
        MEMSWITCHBACK(BANK_GENERIC);
        return 1;
    }
    p->task = func;
    p->next = taskList;
    taskList = p;
    MEMSWITCHBACK(BANK_GENERIC);
    return 0;
}

uint8_t removeTask(Task func) {
    MEMSWITCH(BANK_GENERIC);
    TaskElement *p = taskList;
    TaskElement *prev = NULL;
    while (p != NULL) {
        if (p->task == func) {
            if (prev == NULL) {
                taskList = p->next;
            } else {
                prev->next = p->next;
            }
            free(p);
            MEMSWITCHBACK(BANK_GENERIC);
            return 0;
        }
        prev = p;
        p = p->next;
    }
    MEMSWITCHBACK(BANK_GENERIC);
    return 1;
}

void tasks(void) {
    MEMSWITCH(BANK_GENERIC);
    static TaskElement *p = NULL;
    if (p == NULL) {
        p = taskList;
    }
    if (p != NULL) {
        p->task();
        p = p->next;
    }
    MEMSWITCHBACK(BANK_GENERIC);
}
/** @} */
