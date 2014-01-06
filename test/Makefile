CC=g++

INC=-Igsl/include -I../estimator -I../base -I../kalmanfilter -I../setting -I../simulator
LIBS=-lm -lgsl
CPPFLAGS=$(INC) $(LIBS) -Wall -O2 -DENV_LINUX
DEBUGFLAGS=$(CPPFLAGS:O2=g)
OBJS=../base/*.o ../estimator/*.o ../kalmanfilter/*.o ../setting/*.o ../simulator/*.o
SRCS=EstimatorTest.cpp BeaconInfo.cpp

all: estimator base kalmanfilter setting simulator $(SRCS)

	$(CC) -o estimator_test $(SRCS) -ggdb $(LIBS) $(INC) $(OBJS)

clean:
	rm *.o -rf

debug:
	$(CC) -o estimator_test EstimatorTest.cpp BeaconInfo.cpp $(LIBS) $(INC) $(OBJS)

uninstall:
	rm *.cpp *.h

estimator:
	$(MAKE) -C ../estimator

base:
	$(MAKE) -C ../base

kalmanfilter:
	$(MAKE) -C ../kalmanfilter

setting:
	$(MAKE) -C ../setting
	
simulator:
	$(MAKE) -C ../simulator
