CC=g++
OBJS= \
	  EventLog.o \
	  BeaconDeploy.o \
	  PlaneGenerator.o \
	  SimulatorArgument.o \
	  DistanceScenario.o \
	  DistanceSimulator.o \
	  EventGenerator.o \
	  ListenerMover.o \
	  PathGenerator.o \







#	  Simulator.o \





SRCS=$(OBJS:.o=.cpp)
INC=-I../base -I../estimator -I../kalmanfilter -I../setting -I../simulator
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
