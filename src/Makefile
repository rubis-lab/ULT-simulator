include ./include.mk

DIRS=estimator \
	 base \
	 kalmanfilter \
	 analyzer \
	 simulator \
	 planedetector

OBJS=${DIRS:=/*.o}

#OBJS=estimator/*.o base/*.o kalmanfilter/*.o analyzer/*.o simulator/*.o planedetector/*.o

.PHONY: $(DIRS)

all: bin_simulator bin_planedetector bin_simulator_iter bin_planedetector_fixed bin_planedetector_all

debug: bin_simulator bin_planedetector
	for dir in $(DIRS);	do $(MAKE) debug -C $$dir;	done
	$(CC) -o ../bin/simulator $(OBJS) Simulator.cpp $(DEBUGFLAGS) 
	$(CC) -o ../bin/planedetector $(OBJS) DetectPlanes.cpp $(DEBUGFLAGS)

clean:
	for dir in $(DIRS);	do $(MAKE) clean -C $$dir;	done
	rm *.o -rf

bin_simulator: $(DIRS) 
	$(CC) -o ../bin/simulator $(OBJS) Simulator.cpp $(CPPFLAGS) 

bin_planedetector: $(DIRS)
	$(CC) -o ../bin/planedetector $(OBJS) DetectPlanes.cpp $(CPPFLAGS)

bin_simulator_iter: $(DIRS) 
	$(CC) -o ../bin/simulator_iter $(OBJS) SimulatorIter.cpp $(CPPFLAGS) 

bin_planedetector_fixed: $(DIRS)
	$(CC) -o ../bin/planedetector_fixed $(OBJS) DetectPlanesFixedPoint.cpp $(CPPFLAGS)

bin_planedetector_all: $(DIRS)
	$(CC) -o ../bin/planedetector_all $(OBJS) DetectPlanesAllPoints.cpp $(CPPFLAGS)
	
$(DIRS):
	$(MAKE) -C $@

