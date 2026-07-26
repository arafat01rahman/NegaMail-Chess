CXX = g++
# CXXFLAGS = -g -std=c++17 -Wall
CXXFLAGS = -std=c++17 -o2 -s -Wall
TARGET = engine
SOURCES = main.cpp board.cpp evaluate.cpp search.cpp uci.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)