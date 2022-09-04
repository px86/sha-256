CXX = g++
CXXFLAGS = -Wall -Werror -Wpedantic -Weffc++ -std=c++17 -Isrc/include -O3

all: sha256

sha256: src/main.cpp sha256.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: src/%.cpp src/include/%.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: sha256
	sha256sum LICENSE | cut -d' ' -f1 > t1
	./sha256  LICENSE > t2
	diff t1 t2
	rm t1 t2

clean:
	rm -rf *.o sha256 t
