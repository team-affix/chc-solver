CXX      = g++
CXXFLAGS = -std=c++20
AR       = ar
ARFLAGS  = rcs

SRC = $(wildcard cpp/*.cpp)

RELEASE_OBJ = $(patsubst cpp/%.cpp, build/release/%.o, $(SRC))
DEBUG_OBJ   = $(patsubst cpp/%.cpp, build/debug/%.o, $(SRC))

RELEASE_LIB = libchc.a
DEBUG_LIB   = libchc_debug.a

$(RELEASE_LIB): $(RELEASE_OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(DEBUG_LIB): $(DEBUG_OBJ)
	$(AR) $(ARFLAGS) $@ $^

build/release/%.o: cpp/%.cpp | build/release
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/debug/%.o: cpp/%.cpp | build/debug
	$(CXX) $(CXXFLAGS) -DDEBUG -c $< -o $@

build/release build/debug:
	mkdir -p $@

all: $(DEBUG_LIB)
	$(CXX) $(CXXFLAGS) -DDEBUG -g test/main.cpp -L. -lchc_debug -o main

fastdebug: $(DEBUG_LIB)
	$(CXX) $(CXXFLAGS) -DDEBUG -O3 test/main.cpp -L. -lchc_debug -o main

parser:
	cd syntax && antlr4 -Dlanguage=Cpp -visitor -no-listener -o generated/ CHC.g4

clean:
	rm -f main $(RELEASE_LIB) $(DEBUG_LIB)
	rm -rf build
	rm -rf syntax/generated
