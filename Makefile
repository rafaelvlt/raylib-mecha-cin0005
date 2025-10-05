# =============================================================================
# Usage:
#   - `make`: Compiles from scratch
#   - `make run`: Compiles and executes
#   - `make clean`: Clean generated files
# =============================================================================

# Executable Name
PROJECT_NAME := mechaGame

# Project Folders
SRC_DIR := src
INCLUDE_DIR := include
LIB_DIR := lib

# Find all C code to be linked and compiled
SOURCES := $(wildcard $(SRC_DIR)/*.c)

# Find the Host OS
OS := $(shell uname -s)


# Windows
ifneq ($(findstring MINGW,$(OS)),)
    IS_WINDOWS := 1
else ifneq ($(findstring Windows,$(OS)),)
    IS_WINDOWS := 1
else
    IS_WINDOWS := 0
endif

# Args for Windows
ifeq ($(IS_WINDOWS), 1)
    TARGET := $(PROJECT_NAME).exe
    LIBS := -lraylib -lopengl32 -lgdi32 -lwinmm
    RM := del /Q /F
# Args for Linux/Unix
else
    TARGET := $(PROJECT_NAME)
    LIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    RM := rm -f
endif

# The compiler
CC := gcc

# Compiler Flags and where to find include 
CFLAGS := -g -Wall -I$(INCLUDE_DIR) -I$(LIB_DIR) -I$(LIB_DIR)/raylib/src

# Flags for the linker
LDFLAGS := -L$(LIB_DIR)/raylib/src

# Make default rule
all: $(TARGET)

# If any .c file changes, the project must be rebuilt
$(TARGET): $(SOURCES)
	@echo "--- Compiling and linking the project for $(OS) ---"
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS) $(LIBS)
	@echo "--- Build finished: $(TARGET) ---"

# Rule for compiling and running
run: all
	@echo "--- Opening the Game ---"
	./$(TARGET)

# Rule for cleaning the build files
clean:
	@echo "--- Cleaning build files --	-"
	$(RM) $(TARGET)

# Declaration for our rules to not create files
.PHONY: all run clean

