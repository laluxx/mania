# Compiler
CC = gcc

# Flags for the compiler (Wall for warnings, g for debugging)
CFLAGS = -Wall -g

# Linker flags (specify the libraries you're linking against here)
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Your source file(s)
SOURCES = main.c
# The output executable's name
EXECUTABLE = rhythm_game

all: $(SOURCES)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(EXECUTABLE)
