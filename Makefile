# Compiler
CXX = g++
CC  = gcc

# Flags
CXXFLAGS = -Wall -std=c++17 -O2
CFLAGS   = -Wall -O2

# Platform-specific linker flags
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lglfw3dll -lgdi32 -lopengl32 -luser32 -lkernel32 -lwinmm
else
    LDFLAGS = -lglfw -lGL -lpthread -ldl -lfreetype
endif

# Directories
INCLUDE_DIRS = -Iinclude -I/usr/include/freetype2
OBJ_DIR      = build
SRC_DIR      = src
LIB_DIR      = include/lib
GLAD_SRC     = include/glad.c
OUTPUT       = graph

# Find all cpp files recursively
SRC = $(shell find $(SRC_DIR) -name "*.cpp")
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
GLAD_OBJ = $(OBJ_DIR)/glad.o

# Default target
all: $(OUTPUT)

# Linking
$(OUTPUT): $(OBJ) $(GLAD_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L $(LIB_DIR) $(LDFLAGS)

# Compile C++ source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE_DIRS)

# Compile GLAD
$(GLAD_OBJ): $(GLAD_SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $< $(INCLUDE_DIRS)

# Ensure build directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Debug target
debug: CXXFLAGS += -DDEBUG -O0 -g
debug: clean all

# Clean target
clean:
	@echo Cleaning...
ifeq ($(OS),Windows_NT)
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	@if exist $(OUTPUT).exe del /q $(OUTPUT).exe
else
	@rm -rf $(OBJ_DIR) $(OUTPUT)
endif
	@echo Cleaned!
