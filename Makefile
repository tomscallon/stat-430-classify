OBJS = main.o Stats.o Dataset.o DistanceMeasures.o AgglomerativeClustering.o
CCOM = g++
CFLAGS = -Wall -c -std=c++1z $(DEBUG)
LFLAGS = -Wall $(DEBUG)

make: compile clean

run:
	./classify

compile: $(OBJS)
	$(CCOM) $(OBJS) $(LFLAGS) -o classify

%.o: %.cpp
	$(CCOM) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJS)
