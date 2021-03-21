# CXX Make variable for compiler
CXX=g++
# Make variable for compiler options
#	-std=c++11  C/C++ variant to use, e.g. C++ 2011
#	-g          include information for symbolic debugger e.g. gdb 
CXXFLAGS=-std=c++11 -g

# Rules format:
# target : dependency1 dependency2 ... dependencyN
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
a3 : byu_tracereader.o output_mode_helpers.o base_helpers.o pagetable.o page.o main.o
	$(CXX) $(CXXFLAGS) byu_tracereader.o base_helpers.o pagetable.o page.o main.o -o pagetable

byu_tracereader.o : byutr.h byu_tracereader.c
	$(CXX) $(CXXFLAGS) -c byu_tracereader.c

output_mode_helpers : output_mode_helpers.h output_mode_helpers.C
	$(CXX) $(CXXFLAGS) -c output_mode_helpers.c

base_helpers.o : base_helpers.h base_helpers.cpp
	$(CXX) $(CXXFLAGS) -c base_helpers.cpp

pagetable.o: pagetable.h pagetable.cpp
	$(CXX) $(CXXFLAGS) -c pagetable.cpp

page.o: page.cpp
	$(CXX) $(CXXFLAGS) -c page.cpp

main.o : main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

clean :
	rm *.o

