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

#define TESTING

//-------------------------------------------------------------------------
// fifo_iterate_fn
//-------------------------------------------------------------------------
static void print_element(int iIndex, void *pValue, BOOL bCellIsEmpty, BOOL bIsNextPushLocation, BOOL bIsNextPopLocation)
{
   char *pTempValue;

   if (bCellIsEmpty)
      pTempValue = "<empty>";
   else
      pTempValue = (pValue==NULL) ? "<NULL>" : (char *)pValue;
   printf("  Cell_%2.2d: %s", iIndex, pTempValue);
   if (bIsNextPushLocation) printf(" <== In");
   if (bIsNextPopLocation) printf(" <== Out");
   printf("\n");
}

//-------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------
static void print_table(void)
{
   printf("\n");
   printf("  Number of Cells = %d\n",fifo_get_number_of_cells());
   printf("  Number of Used Cells = %d\n",fifo_get_number_of_used_cells());
   printf("\n");
   fifo_iterate(print_element);
}

//-------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------
int main(int argc, char* argv[])
{
   int tmpTableSize = 0;
   int tmpAction = 0;

   printf(" Specify size of Table: ");
   scanf("%d", &tmpTableSize);

   /* Initialise the FIFO */
   fifo_init(tmpTableSize);

   do
   {
#ifdef TESTING
   /* ANSI escape sequence to clear the screen */
   printf("\x1B[2J");
   print_table();
#endif

      putchar('\n');

      printf("Options...\n");
      printf(" 0: Exit\n");
      printf(" 1: Push string\n");
      printf(" 2: Pop string\n");
#ifndef TESTING
      printf(" 3: Print entire table\n");
#endif
      printf("Enter Option: ");
      //scanf("%d", &tmpAction);
      tmpAction = mygetch();
      putchar('\n');

      /* Validate user's choice */
      if (tmpAction >= '0' && tmpAction <= '9')
         tmpAction -= '0';
      else if (tmpAction == 0x1B) // Escape char
         tmpAction = 0;
      else
         // Ignore and continue
         continue;

      switch (tmpAction)
      {
         case 1:
         {
            char TempStr[64];
            char* p;

            printf(" Insert elements\n\n");
#ifdef TESTING
            {
               static int datacounter = 0;
               sprintf(TempStr,"Testing%2.2d",datacounter++);
            }
#else
            printf(" Enter string: ");
            scanf("%s", TempStr);
#endif

            p = strdup(TempStr);
            if ( !fifo_push((void *)p))
            {
               free(p);
               printf(" Table is full\n");
            }
            else
            {
               printf(" Push successful\n");
            }
            break;
         }

         case 2:
            printf(" Pop string : ");
            if (fifo_is_empty())
            {
               printf("No string in table\n");
            }
            else
            {
               char *p = (char *)fifo_pop();
               printf("%s\n", p);
               free(p);
            }
            break;

#ifndef TESTING
         case 3:
            print_table();
            break;
#endif
      }
   } while (tmpAction != 0);

   fifo_destroy();

   exit(0);
}
