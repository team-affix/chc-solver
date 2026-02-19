all:
	g++ -std=c++20 -DDEBUG ./test/main.cpp ./cpp/* -o main

clean:
	rm -f main
