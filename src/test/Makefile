CC=g++

INC=-Igsl/include -I../base -I../estimator -I../kalmanfilter -I../simulator -I../analyzer
LIBS=-lm -lgsl
CPPFLAGS=$(INC) $(LIBS) -Wall -Wextra -O2 -DENV_LINUX
DEBUGFLAGS=$(CPPFLAGS:O2=g)
OBJS=../base/*.o ../estimator/*.o ../kalmanfilter/*.o ../simulator/*.o ../analyzer/*.o
SRCS=EstimatorTest.cpp BeaconInfo.cpp

all: estimator base kalmanfilter simulator analyzer $(SRCS)

	$(CC) -o estimator_test $(SRCS) $(CPPFLAGS) $(LIBS) $(INC) $(OBJS)
	$(CC) -o event_test EventGeneratorTest.cpp $(CPPFLAGS) $(LIBS) $(INC) $(OBJS)

clean:
	rm *.o -rf

debug:
	$(MAKE) debug -C ../estimator
	$(MAKE) debug -C ../base
	$(MAKE) debug -C ../kalmanfilter
	$(MAKE) debug -C ../simulator
	$(MAKE) debug -C ../analyzer
	$(CC) -o estimator_test $(SRCS) $(DEBUGFLAGS) $(LIBS) $(INC) $(OBJS)
	$(CC) -o event_test EventGeneratorTest.cpp $(DEBUGFLAGS) $(LIBS) $(INC) $(OBJS)

uninstall:
	rm *.cpp *.h

estimator:
	$(MAKE) -C ../estimator

base:
	$(MAKE) -C ../base

kalmanfilter:
	$(MAKE) -C ../kalmanfilter

simulator:
	$(MAKE) -C ../simulator

analyzer:
	$(MAKE) -C ../analyzer
