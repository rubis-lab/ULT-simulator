CC=g++
OBJS=Vector.o \
	 Plane.o \
	 Beacon.o \
	 IOUtil.o \
	 Random.o \



SRCS=$(OBJS:.o=.cpp)
INC=-I../base -I../estimator -I../kalmanfilter -I../setting
INC=-I../base -I../estimator -I../kalmanfilter -I../setting -I../simulator
LIBS=
CPPFLAGS=$(INC) $(LIBS) -Wall -Wextra -O2 -DENV_LINUX
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
