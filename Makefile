COMPILER = gcc
OUTPUT_FILE = md5
OUTPUT_FILE_SLAVE = slave
CFLAGS = -Wall -fsanitize=address -g
DEBUG_COMPILER = -Wall

all: app.o slave.o
	$(COMPILER) -o $(OUTPUT_FILE) app.o $(CFLAGS)
	$(COMPILER) -o $(OUTPUT_FILE_SLAVE) slave.o $(CFLAGS)

app.o: src/app.c
	$(COMPILER) -c src/app.c $(CFLAGS)

slave.o: src/slave.c
	$(COMPILER) -c src/slave.c $(CFLAGS)

debug: CFLAGS=$(DEBUG_COMPILER)
debug: all

clean:
	rm -r $(OUTPUT_FILE) *.o
