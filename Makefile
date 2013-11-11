CC=g++
INC=-Iestimator -Ibase
LIBS=-lgsl -lgslcblas 
CPPFLAGS = $(INC) -Wall -O2 $(LIBS) 

OBJS=estimator/*.o base/*.o
SRCS=SimulationTracer/SimulationTracer.cpp
TARGET=simulator



all: estimator base $(SRCS)
	$(CC) -o $(TARGET) $(OBJS) $(CPPFLAGS) 

debug:
	$(MAKE) debug -C estimator
	$(MAKE) debug -C base
	$(CC) -o $(TARGET) $(OBJS) $(CPPFLAGS) -g

estimator:
	$(MAKE) -C estimaotr

base:
	$(MAKE) -C base

clean:
	$(MAKE) clean -C estimator
	$(MAKE) clean -C base
	rm *.o -rf

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $*.cpp -o $*.o
	$(CC) -MM $(CPPFLAGS) $*.cpp > .$*.d

-include .*.d
