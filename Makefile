CXX=g++

CXXFLAGS=-std=c++11 -g

pagetable : byu_tracereader.o output_mode_helpers.o pagetable.o page.o main.o
	$(CXX) $(CXXFLAGS) byu_tracereader.o output_mode_helpers.o pagetable.o page.o main.o -o pagetable

byu_tracereader.o : byutr.h byu_tracereader.c
	$(CXX) $(CXXFLAGS) -c byu_tracereader.c

output_mode_helpers.o : output_mode_helpers.h output_mode_helpers.cpp
	$(CXX) $(CXXFLAGS) -c output_mode_helpers.cpp

pagetable.o: pagetable.h pagetable.cpp
	$(CXX) $(CXXFLAGS) -c pagetable.cpp

page.o: page.cpp
	$(CXX) $(CXXFLAGS) -c page.cpp

main.o : main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

clean :
	rm *.o

