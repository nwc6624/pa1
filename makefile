##############################################################
#
#                   DO NOT EDIT THIS FILE!
#
##############################################################
# Command line arguments
CACHESIZE = 32
WAYS = 4
BLOCKSIZE = 64 # bytes

# Define the compiler to use
CC = g++

# Define any compile-time flags
CCFLAGS = -std=c++11 -Wall -g

# Define the cpp source files
SRC_DIR = ./src
SRCS = $(wildcard $(SRC_DIR)/*.cpp) # Using .cpp extension

# Define the target directory for object files
OBJ_DIR = ./target
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Define the target directory for the executable
MAIN_DIR = ./target
MAIN = $(MAIN_DIR)/CacheSim # Save the executable in the target directory

all: $(OBJ_DIR) $(MAIN)

# Create the target directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR) # Create the target directory if it doesn't exist

# Build the executable file
$(MAIN): $(OBJ_DIR) $(OBJS)
	$(CC) $(CCFLAGS) -o $@ $(OBJS)
	@echo CacheSim has been compiled

# Rule for building .o's from .cc's
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CCFLAGS) -c $< -o $@

# Run the executable file
run: $(OBJ_DIR) $(MAIN)
	./$(MAIN) $(CACHESIZE) $(WAYS) $(BLOCKSIZE)

# Remove the executable file, the .o object files, and backup files
clean:
	rm -rf $(OBJ_DIR) # Delete the target directory
	find . -name "*.csv" -exec rm -f {} ;
	find . -name "*.db" -exec rm -f {} ;
	find . -name "*.o" -exec rm -f {} ;
	rm -rf ./docs/html

# Ensure the target directory is created each time before compiling
$(MAIN): $(OBJ_DIR)
##############################################################
#
#                   DO NOT EDIT THIS FILE!
#
##############################################################
