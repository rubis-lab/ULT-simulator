CC=g++
OBJS=PositionSolver.o \
	 HeardList.o \

SRCS=$(OBJS:.o=.cpp)
INC=-I../base -I./
LIBS=
CPPFLAGS=$(INC) $(LIBS) -Wall -O2 -DENV_LINUX
DEBUGFLAGS=$(CPPFLAGS:O2=g)

all:$(OBJS)

clean:
	rm -rf *.o .*.d

debug:
	$(CC) -c $(SRCS) $(DEBUGFLAGS)

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $*.cpp -o $*.o
	$(CC) -MM $(CPPFLAGS) $*.cpp > .$*.d

-include .*.d
