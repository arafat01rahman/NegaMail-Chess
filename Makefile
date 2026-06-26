engine:
	g++ -g -std=c++17 -Wall main.cpp math.cpp board.cpp -o engine
clean:
	rm -f engine