CXX = c++
CXXFLAGS = -std=c++17 -lpthread -lstdc++fs -fsanitize=address
OBJECTS = main.o KMP.o

psearch: $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(OBJECTS): KMP.h
