CXX=g++
CXXFLAGS=-std=c++17 -pedantic -Wall -Wextra -Werror -g
TARGET=runfile

BUILD_DIR=build
MKDIR=mkdir -p

.PHONY: all
all: compile run

.PHONY: compile
compile: $(TARGET)

.PHONY: run
run: $(TARGET)
	./$(TARGET)

.PHONY: valgrind
valgrind: $(TARGET)
	valgrind ./$(TARGET)

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)/ $(TARGET) 2>/dev/null

$(TARGET): $(BUILD_DIR)/CFileHandler.o $(BUILD_DIR)/CImage_PNG.o $(BUILD_DIR)/main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: src/%.cpp
	$(MKDIR) $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/CFileHandler.o: src/CFileHandler.cpp src/CFileHandler.h
$(BUILD_DIR)/CImage_PNG.o: src/CImage_PNG.cpp src/CImage_PNG.h src/CFileHandler.cpp src/CFileHandler.h
$(BUILD_DIR)/main.o: src/main.cpp src/CImage_PNG.h src/CFileHandler.cpp src/CFileHandler.h