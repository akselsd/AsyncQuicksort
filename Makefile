CC := g++
CFLAGS := -pthread -O
DEPENDENCIES := quicksort.hpp
SRC := main.cpp
BUILD := build
OBJ = $(SRC:%.cpp=$(BUILD)/%.o)


$(BUILD):
	mkdir $(BUILD)

.DEFAULT_GOAL := default

$(BUILD)/%.o: %.cpp $(DEPENDENCIES) | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: echo
echo:
	echo $(OBJ)
	echo $(SRC)

default: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o bin
