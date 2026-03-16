all:
	g++ -std=c++20 -g -DDEBUG ./test/main.cpp ./cpp/* -o main

fastdebug:
	g++ -std=c++20 -O3 -DDEBUG ./test/main.cpp ./cpp/* -o main

parser:
	antlr4 -Dlanguage=Cpp -visitor -no-listener -o syntax/generated/ syntax/CHC.g4

clean:
	rm -f main 
	rm -rf syntax/generated
