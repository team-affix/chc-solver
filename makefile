all:
	g++ -std=c++20 ./test/main.cpp ./cpp/* -o main

clean:
	rm -f main
