/*******************************************************************************
 * An implementation of a round FIFO queue in C.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "linux_utils.h"
#include "circular_fifo.h"

#pragma warning(disable:4996)

static int gFifoNumberOfCells;
static int gFifoIndexIn;
static int gFifoIndexOut;
static int gFifoUsedCells;
static void **gFifoTable;

//-------------------------------------------------------------------------
// Initialise the FIFO queue
//-------------------------------------------------------------------------
void fifo_init(int size)
{
   gFifoUsedCells     = 0;
   gFifoIndexIn       = 0;
   gFifoIndexOut      = 0;
   gFifoNumberOfCells = size;

   gFifoTable = (void **)malloc(gFifoNumberOfCells * sizeof(void *));
   memset(gFifoTable, 0, gFifoNumberOfCells * sizeof(void *));
}

//-------------------------------------------------------------------------
// Empty queue = 1 else 0
//-------------------------------------------------------------------------
int fifo_is_empty(void)
{
   return (gFifoUsedCells == 0);
}

//-------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------
BOOL fifo_cell_is_empty(int i)
{
#define TRY3

#ifdef TRY3
      // Works Well, and elegant
      int position_in_out_queue;

      // We know how many items exist in the table. These are all in the "out queue".
      // If the item in question is in this "out-queue" then it is not empty.
      //
      // Cells in table = 16
      // Items in table = 11
      //
      //                  x
      //                x   x
      //              x       x
      //            x           i   <---- i
      //           x             x
      //            x           x
      //  OUT ---->  x       -   <---- IN
      //                -   -
      //                  -
      //
      // If we assume a fully linear implementation, these are from gFifoIndexOut to gFifoIndexOut+gFifoUsedCells.
      //
      //           xxxixx-----xxxxxx
      //                            xxxixx-----xxxxxx
      //                      ^OUT
      //                 ^IN              ^IN
      //              ^i               ^i
      //                      0123456789AB (out queue)
      //              ^                ^position_in_out_queue
      //

      position_in_out_queue = i - gFifoIndexOut;
      if (position_in_out_queue < 0)
         position_in_out_queue += gFifoNumberOfCells;

      // i.e. position_in_out_queue = (i < gFifoIndexOut) ? (i - gFifoIndexOut + gFifoNumberOfCells) : (i - gFifoIndexOut);
      // or   position_in_out_queue = (i - gFifoIndexOut + gFifoNumberOfCells) % gFifoNumberOfCells;

      if (position_in_out_queue < gFifoUsedCells)
         return FALSE;
      return TRUE;
#endif

#ifdef TRY2
      // Works Well, but inefficient
      int j;
      for (j=0; j<gFifoUsedCells; j++)
      {
         int elem = (gFifoIndexOut+j) % gFifoNumberOfCells;
         if (elem==i)
            return FALSE;
      }
      return TRUE;
      x = 2;
#endif

#ifdef TRY1
   // Works Well, but verbose
   BOOL cell_is_empty = TRUE;

   // If InPtr and OutPtr are on the same cell...
   if (gFifoIndexIn == gFifoIndexOut)
   {
      // InPtr and OutPtr are on the same cell, but the table is full
      if (gFifoUsedCells==gFifoNumberOfCells)
         cell_is_empty = FALSE;
      else // InPtr and OutPtr are on the same cell, but the table is empty
         cell_is_empty = TRUE;
   }
   // Otherwise, InPtr is always empty
   else if (i==gFifoIndexIn)
   {
      cell_is_empty = TRUE;
   }
   // and, OutPtr is always non-empty
   else if (i==gFifoIndexOut)
   {
      cell_is_empty = FALSE;
   }
   else if (gFifoIndexOut<gFifoIndexIn)
   {
      if (i>gFifoIndexOut && i<gFifoIndexIn)
         cell_is_empty = FALSE;
      else
         cell_is_empty = TRUE;
   }
   else if (gFifoIndexOut>gFifoIndexIn)
   {
      if (i>gFifoIndexIn && i<gFifoIndexOut)
         cell_is_empty = TRUE;
      else
         cell_is_empty = FALSE;
   }

   return cell_is_empty;
#endif
}

//-------------------------------------------------------------------------
// Free memory
//-------------------------------------------------------------------------
void fifo_destroy(void)
{
   int i;

   if (!fifo_is_empty())
   {
/*
      for (i = 0; i < gFifoNumberOfCells; i++)
      {
         if (!fifo_cell_is_empty(i))
         {
            printf("Freeing element %d\n",i);
            free(gFifoTable[i]);
         }
      }
*/
      for (i=0; i<gFifoUsedCells; i++)
      {
         int elem = (gFifoIndexOut+i) % gFifoNumberOfCells;
         printf("Freeing element %d\n",elem);
         free(gFifoTable[elem]);
      }
   }
   printf("Freeing table\n");
   free(gFifoTable);
}

//-------------------------------------------------------------------------
// Insert element
//-------------------------------------------------------------------------
int fifo_push(void *next)
{
   if (gFifoUsedCells == gFifoNumberOfCells)
   {
      return (0);
   }
   else
   {
      gFifoTable[gFifoIndexIn] = next;
      gFifoUsedCells++;
      gFifoIndexIn = (gFifoIndexIn + 1) % gFifoNumberOfCells;
      return (1);
   }
}

//-------------------------------------------------------------------------
// Return next element
//-------------------------------------------------------------------------
void *fifo_pop(void)
{
   void *pElement;

   if (gFifoUsedCells > 0)
   {
      pElement = gFifoTable[gFifoIndexOut];
      gFifoTable[gFifoIndexOut] = NULL;
      gFifoIndexOut = (gFifoIndexOut + 1) % gFifoNumberOfCells;
      gFifoUsedCells--;
      return (pElement);
   }
}

//-------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------
int fifo_get_number_of_cells(void)
{
   return gFifoNumberOfCells;
}

//-------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------
int fifo_get_number_of_used_cells(void)
{
   return gFifoUsedCells;
}

//-------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------
void fifo_iterate(void (*fifo_iterate_fn)(int iIndex, void *pValue, BOOL bCellIsEmpty, BOOL bIsNextPushLocation, BOOL bIsNextPopLocation))
{
   int i;

   for (i = 0; i < gFifoNumberOfCells; i++)
   {
      fifo_iterate_fn(i, gFifoTable[i], fifo_cell_is_empty(i), i==gFifoIndexIn, i==gFifoIndexOut);
   }
}
