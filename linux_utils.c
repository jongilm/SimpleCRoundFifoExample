#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <termios.h>
#include <unistd.h> // for STDIN_FILENO etc

#include "linux_utils.h"

//-------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------
int mygetch(void)
{
  struct termios oldt;
  struct termios newt;
  int ch;

  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

  return ch;
}

