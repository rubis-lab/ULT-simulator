CC=g++
OBJS=Vector.o \
	 Plane.o \
	 Beacon.o \


SRCS=$(OBJS:.o=.cpp)
INC=-I../estimator -I./
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


#depend: .depend

#.depend: $(SRCS)
#	rm -f ./.depend
#	$(CC) $(CPPFLAGS) -MM $^ -MF ./.depend;

#include .depend
