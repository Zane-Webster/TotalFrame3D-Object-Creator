# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Wno-unused -Wno-missing-field-initializers -Wno-implicit-fallthrough -Iinclude -Iinclude/SDL3 -Iinclude/GL -Iinclude/glm -Iinclude/tfd -Iinclude/stb
LDFLAGS = -Llib -Wl,-subsystem,windows
LIBS = -lmingw32 -lSDL3 -lSDL3_image -lSDL3_mixer -lSDL3_ttf -lglew32 -lopengl32 -lole32 -luuid -lcomdlg32

# Directories
SRC_DIR = src
OBJ_DIR = o
BIN_DIR = bin

# Sources and objects
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
TFD_SRC = include/tfd/tinyfiledialogs.cpp
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
TFD_OBJ = $(OBJ_DIR)/tinyfiledialogs.o

# Output executable
TARGET = $(BIN_DIR)/Main

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(BIN_DIR) $(OBJS) $(TFD_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(TFD_OBJ) $(LIBS)

# Compile each source file to an object file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile tinyfiledialogs separately
$(TFD_OBJ): $(TFD_SRC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Create the bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean