CC=g++
INC=-Iestimator -Ibase
LIBS=-lgsl -lgslcblas 
CPPFLAGS = $(INC) -Wall -O2 $(LIBS) 

OBJS=estimator/*.o base/*.o kalmanfilter/*.o
#SRCS=SimulationTracer/SimulationTracer.cpp
TARGET=simulator




all: estimator base kalmanfilter $(SRCS)
	$(MAKE) -C base
	$(MAKE) -C estimator
	$(MAKE) -C kalmanfilter
	$(CC) -o $(TARGET) $(OBJS) $(CPPFLAGS) 

debug:
	$(MAKE) debug -C estimator
	$(MAKE) debug -C base
	$(MAKE) debug -C kalmanfilter
	$(CC) -o $(TARGET) $(OBJS) $(CPPFLAGS) -g

estimator:
	$(MAKE) -C estimator

base:
	$(MAKE) -C base

kalmanfilter:
	$(MAKE) -C kalmanfilter

clean:
	$(MAKE) clean -C estimator
	$(MAKE) clean -C base
	$(MAKE) clean -C kalmanfilter
	rm *.o -rf

