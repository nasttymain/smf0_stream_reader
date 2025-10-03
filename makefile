run: test.out
			./test.out
test.out: test.cpp smstream.o
			gcc test.cpp smstream.o -o test.out
smstream.o: smstream.cpp smstream.hpp
			gcc -c smstream.cpp