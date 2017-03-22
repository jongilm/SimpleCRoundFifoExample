
/*******************************************************************************
 * An implementation of a round FIFO queue.
 *
 * Each cell is a generic void ptr allowing storage of any data type.
 *
 * In this example the FIFO is used to store strings.
 * The user chooses the size and then inserts or deletes strings from the queue.
 *
 * You can imagine it to be a cyclic buffer, something like the following...
 *
 *                       x    <--- Starting point of both IN and OUT
 *                     x   x
 *                   x       x
 *                 x           x
 *                x             x
 *                 x           x
 *       OUT ---->  x       -   <---- IN
 *                     -   -
 *                       -
 * Where...
 *   'IN'  is a pointer to the next empty cell i.e. where the next inserted item
 *         will be stored (gFifoIndexIn),
 *   'OUT' is a pointer to the next retrievable item (gFifoIndexOut).
 *   'x'   means that there is data in the cell
 *   '-'   means that the cell is empty.
 *
 *   An integer (gFifoUsedCells) counts how many cells are used.
 *   In this case gFifoNumberOfCells is 16 and gFifoUsedCells is 11.
 *
 *******************************************************************************/

#ifndef __CIRCULAR_FIFO_H__
#define __CIRCULAR_FIFO_H__

#include "linux_utils.h" /* Needed for BOOL */

extern void fifo_init(int size);
extern int fifo_is_empty(void);
extern BOOL fifo_cell_is_empty(int i);
extern void fifo_destroy(void);
extern int fifo_push(void *next);
extern void *fifo_pop(void);
extern int fifo_get_number_of_cells(void);
extern int fifo_get_number_of_used_cells(void);
extern void fifo_iterate(void (*fifo_iterate_fn)(int iIndex, void *pValue, BOOL bCellIsEmpty, BOOL bIsNextPushLocation, BOOL bIsNextPopLocation));

#endif // __CIRCULAR_FIFO_H__
