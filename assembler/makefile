CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Source files
SRC = assembler.c parser.c
# Object files
OBJ = $(SRC:.c=.o)
# Executable name
TARGET = assembler

# Default rule
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compile C files into object files
%.o: %.c parser.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJ) $(TARGET)
