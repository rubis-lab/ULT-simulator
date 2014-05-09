SRCS=$(OBJS:.o=.cpp)

all:$(OBJS)
	
clean:
	rm -rf *.o .*.d

debug:
	$(CC) -c $(SRCS) $(DEBUGFLAGS)

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $*.cpp -o $*.o
	$(CC) -MM $(CPPFLAGS) $*.cpp > .$*.d

-include .*.d
