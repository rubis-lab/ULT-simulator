CC=g++
OBJS=ListenerMover.o \
LogRecorder.o \
Random.o \
TestRoom.o \
SimulationTracer.o \
Analyzer.o \
Argument.o \
BeaconInfo.o \
Cylinder.o \
FilterManager.o \
HeardList.o \
KalmanFilter.o \
KFTracker.o \
LocationEstimator.o \
Plane.o \
PositionSolver.o \
Vector.o \
Sphere.o \
PlaneDetector.o


SRCS=$(OBJS:.o=.cpp)
INC=-I../location -I gsl/include
LIBS=-lm -lgsl
CPPFLAGS=$(INC) $(LIBS) -Wall -O2 -DENV_LINUX
DEBUGFLAGS=$(CPPFLAGS:O2=g)

all:$(OBJS)
	$(CC) -o simulator $(SRCS) $(CPPFLAGS) $(LIBS) $(INC)

clean:
	rm *.o -rf

debug:
	$(CC) -c $(SRCS) $(DEBUGFLAGS)

uninstall:
	rm *.cpp *.h
