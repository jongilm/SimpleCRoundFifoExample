#_DEPS = hellomake.h
#DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

#_OBJ = hellomake.o hellofunc.o 
#OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

#.c{$(ODIR)}.o:
#	$(CC) -c -o $@ $< $(CFLAGS)

#.c.out: 
#	gcc -o $@ $<

#hellomake: $(OBJ)
#	gcc -o $@ $^ $(CFLAGS) $(LIBS)

##################################

IDIR =./include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=./obj
LDIR =./lib
BINDIR=./bin

LIBS=-lm -pthread -lrt

.PHONY: all
all : $(BINDIR)/SimpleCRoundFifoExample.out
	echo --- $@ Done

#########################

$(ODIR)/%.o : %.c $(DEPS)
	$(CC) -c $(CFLAGS) -o $@ $<

#####################

$(BINDIR)/SimpleCRoundFifoExample.out : $(ODIR)/SimpleCRoundFifoExample.o $(ODIR)/circular_fifo.o $(ODIR)/linux_utils.o
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
	echo --- $@ Done

.PHONY: clean
clean:
	rm -f $(BINDIR)/*.out $(ODIR)/*.o *~ core $(INCDIR)/*~ 

