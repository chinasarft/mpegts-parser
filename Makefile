CC=gcc
CFLAGS=-c -g
LDFLAGS=
SOURCES=main.c tsparser.c bitreader.c inctime.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=tsparser

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o tsunpacker
