CC=g++
OBJS=Analyzer.o \



SRCS=$(OBJS:.o=.cpp)
INC=-I../base -I../estimator -I../kalmanfilter -I../simulator -I../analyzer
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


