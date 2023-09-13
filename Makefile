COMPILER = gcc
OUTPUT_FILE = md5
OUTPUT_FILE_SLAVE = slave
CFLAGS = -Wall -fsanitize=address -g
DEBUG_COMPILER = -Wall

all: app slave
    rm *.o

app: app.o utils.o
    $(COMPILER) app.o utils.o $(CFLAGS) -o $(OUTPUT_FILE)

app.o:
    $(COMPILER) -I./src/include src/app.c -c $(CFLAGS)

utils.o:
    $(COMPILER) -I./src/include src/utils/utils.c -c $(CFLAGS)

slave: src/slave.c
    $(COMPILER) -I./src/include src/slave.c $(CFLAGS) -o $(OUTPUT_FILE_SLAVE)

debug: CFLAGS=$(DEBUG_COMPILER)
debug: all

clean:
    rm -r $(OUTPUT_FILE) $(OUTPUT_FILE_SLAVE) *.o
