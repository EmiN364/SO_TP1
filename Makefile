COMPILER = gcc
OUTPUT_FILE = md5
OUTPUT_FILE_SLAVE = slave
CFLAGS = -Wall
DEBUG_COMPILER = -Wall -fsanitize=address -g

all: app slave
	rm *.o

app: app.o utils.o
	$(COMPILER) app.o utils.o $(CFLAGS) -o $(OUTPUT_FILE)

app.o: src/app.c
	$(COMPILER) -I./src/include src/app.c -c $(CFLAGS)

slave: slave.o utils.o
	$(COMPILER) -I./src/include slave.o utils.o $(CFLAGS) -o $(OUTPUT_FILE_SLAVE)

slave.o: src/slave.c
	$(COMPILER) -I./src/include src/slave.c -c $(CFLAGS)

utils.o: src/utils/utils.c
	$(COMPILER) -I./src/include src/utils/utils.c -c $(CFLAGS)

debug: CFLAGS=$(DEBUG_COMPILER)
debug: all

clean:
	rm $(OUTPUT_FILE) $(OUTPUT_FILE_SLAVE) *.o
