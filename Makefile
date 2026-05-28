CXX = g++

TARGET = main

SRCS = main.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) -g $(OBJS) -lsfml-graphics -lsfml-window -lsfml-system -fopenmp

%.o: %.cpp
	$(CXX) -c -g $< -o $@ -fopenmp

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)