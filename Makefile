.PHONY = clean all

CXX = g++ -std=c++11
FLAGS = -Wextra -Wall
FILES_FOR_TAR = *.cpp *.hpp Makefile README
FILES_FOR_ALGO = algorithms.cpp  algorithms.hpp
FILES_FOR_CACHEFS = CacheFS.h CacheFS.cpp algorithms.hpp

CacheFS.a:  CacheFS.o algorithms.o
	ar rcs CacheFS.a CacheFS.o algorithms.o

algorithms.o: $(FILES_FOR_ALGO)
	$(CXX) -c $(FLAGS) algorithms.cpp

CacheFS.o: $(FILES_FOR_CACHEFS)
	$(CXX) -c $(FLAGS) CacheFS.cpp

tar: $(FILES_FOR_TAR)
	tar cvf ex4.tar $(FILES_FOR_TAR)

clean:
	-rm -f *.o *.a *.tar
