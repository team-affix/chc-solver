COMMON_SRC = $(wildcard common/cpp/*.cpp)
A01_SRC    = $(wildcard a01/cpp/*.cpp)
A02_SRC    = $(wildcard a02/cpp/*.cpp)

all:
	g++ -std=c++20 -g -DDEBUG ./test/main.cpp $(COMMON_SRC) $(A01_SRC) $(A02_SRC) -o main

fastdebug:
	g++ -std=c++20 -O3 -DDEBUG ./test/main.cpp $(COMMON_SRC) $(A01_SRC) $(A02_SRC) -o main

parser:
	cd syntax && antlr4 -Dlanguage=Cpp -visitor -no-listener -o generated/ CHC.g4

clean:
	rm -f main
	rm -rf syntax/generated
