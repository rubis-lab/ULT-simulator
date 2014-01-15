CC=g++
INC=-Iestimator -Ibase
LIBS=-lgsl -lgslcblas 
CPPFLAGS = $(INC) -Wall -Wextra -O2 $(LIBS) 

OBJS=estimator/*.o base/*.o kalmanfilter/*.o simulator/*.o analyzer/*.o
#SRCS=SimulationTracer/SimulationTracer.cpp
TARGET=../bin/simulator




all: estimator base kalmanfilter simulator analyzer $(SRCS)
	$(MAKE) -C base
	$(MAKE) -C estimator
	$(MAKE) -C kalmanfilter
	$(MAKE) -C simulator
	$(MAKE) -C analyzer
	$(CC) -o $(TARGET) $(OBJS) $(CPPFLAGS) 

debug:
	$(MAKE) debug -C estimator
	$(MAKE) debug -C base
	$(MAKE) debug -C kalmanfilter
	$(MAKE) debug -C simulator
	$(MAKE) debug -C analyzer
	$(CC) -o $(TARGET) $(OBJS) $(CPPFLAGS) -g

estimator:
	$(MAKE) -C estimator

base:
	$(MAKE) -C base

kalmanfilter:
	$(MAKE) -C kalmanfilter

simulator:
	$(MAKE) -C simulator

analyzer:
	$(MAKE) -C analyzer

clean:
	$(MAKE) clean -C estimator
	$(MAKE) clean -C base
	$(MAKE) clean -C kalmanfilter
	$(MAKE) clean -C simulator
	$(MAKE) clean -C analyzer
	rm *.o -rf

