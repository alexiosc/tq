# The version number

MAJOR=0
MINOR=5

CC=gcc
LD=gcc
STRIP=strip
LIBS=-lcurses
CFILES=$(shell echo *.c)
OFILES=$(CFILES:.c=.o)
CFLAGS=-O2 -Wall $(DEFINES)
DEFINES=-DMAJOR_VERSION=$(MAJOR) -DMINOR_VERSION=$(MINOR)

all:		tq

clean:		
		\rm -f *.o tq *~ core

tq:	 	$(OFILES)
		$(LD) -o tq $(OFILES) $(LIBS) $(LDFLAGS)
		$(STRIP) tq

%.o:		%.c
		$(CC) $(CFLAGS) -c $< -o $@
