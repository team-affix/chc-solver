all:
	g++ -std=c++20 -g -DDEBUG ./test/main.cpp ./cpp/* -o main

fastdebug:
	g++ -std=c++20 -O3 -DDEBUG ./test/main.cpp ./cpp/* -o main

parser:
	cd syntax && antlr4 -Dlanguage=Cpp -visitor -no-listener -o generated/ CHC.g4

clean:
	rm -f main 
	rm -rf syntax/generated
