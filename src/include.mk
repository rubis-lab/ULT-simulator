CC=g++
ROOT := $(dir $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
INC=-I$(ROOT)estimator \
	-I$(ROOT)base -Isimulator \
	-I$(ROOT)kalmanfilter \
	-I$(ROOT)planedetector \
	-I$(ROOT)analyzer
LIBS=-lgsl -lgslcblas 
CPPFLAGS = $(INC) -Wall -Wextra -O2 -g $(LIBS)  -DENV_LINUX -fopenmp
DEBUGFLAGS=$(CPPFLAGS:O2=g)
