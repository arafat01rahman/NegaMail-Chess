CXX = g++
CXXFLAGS = -g -std=c++17 -Wall
TARGET = engine
SOURCES = main.cpp board.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)