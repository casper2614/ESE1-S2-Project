/*! ***************************************************************************
 *
 * \brief     FIFO abstract data structure with support for multi-byte items
 * \file      fifo.c
 * \author    Hugo Arends
 * \date      January 2026
 *
 * \copyright 2026 HAN University of Applied Sciences. All Rights Reserved.
 *            \n\n
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the "Software"), to deal in the Software without
 *            restriction, including without limitation the rights to use,
 *            copy, modify, merge, publish, distribute, sublicense, and/or sell
 *            copies of the Software, and to permit persons to whom the
 *            Software is furnished to do so, subject to the following
 *            conditions:
 *            \n\n
 *            The above copyright notice and this permission notice shall be
 *            included in all copies or substantial portions of the Software.
 *            \n\n
 *            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *            OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *            NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *            HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *            WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *            FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *            OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#include <board.h>
#include "fifo.h"

void f_init(fifo_t *f, void *buffer, const uint32_t items, const uint32_t item_size)
{
    #ifdef DEBUG

    // Check if the FIFO size is a power of 2
    if((items & (items - 1)) != 0)
    {
        // Error: FIFO size is not a power of 2
        while(1)
        {}
    }

    #endif

    // Initialize FIFO
    f->data = buffer;
    f->head = 0;
    f->tail = 0;
    f->cnt = 0;
    f->items = items;
    f->item_size = item_size;
    f->mask = items - 1;
}

bool f_push(fifo_t *f, void *data)
{
    // FIFO not full?
    if(f->cnt < f->items)
    {
        // Critical section start
        uint32_t m = __get_PRIMASK();
        __disable_irq();

        // Calculate new head
        f->head = (f->head + 1) & f->mask;

        // Store data
        memcpy(&(((uint8_t *)(f->data))[f->head * f->item_size]), data, f->item_size);

        // Increment count
        (f->cnt)++;

        // Critical section end
        __set_PRIMASK(m);

        return true;
    }
    else
    {
        return false;
    }
}

bool f_pop(fifo_t *f, void *data)
{
    // FIFO not empty?
    if(f->cnt > 0)
    {
        // Critical section start
        uint32_t m = __get_PRIMASK();
        __disable_irq();

        // Calculate new tail
        f->tail = (f->tail + 1) & f->mask;

        // Read data
        memcpy(data, &(((uint8_t *)(f->data))[f->tail * f->item_size]), f->item_size);

        // Decrement count
        (f->cnt)--;

        // Critical section end
        __set_PRIMASK(m);

        return true;
    }
    else
    {
        return false;
    }
}

inline uint32_t f_cnt(fifo_t *f)
{
    return f->cnt;
}
