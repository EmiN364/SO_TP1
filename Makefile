COMPILER = gcc
OUTPUT_FILE = md5
OUTPUT_FILE_SLAVE = slave
OUTPUT_FILE_VIEW = view
CFLAGS = -Wall
DEBUG_COMPILER = -Wall -fsanitize=address -g

all: app slave view
	rm *.o

app: app.o utils.o shmAdt.o
	$(COMPILER) app.o utils.o shmAdt.o $(CFLAGS) -o $(OUTPUT_FILE)

app.o: src/app.c
	$(COMPILER) -I./src/include src/app.c -c $(CFLAGS)

slave: slave.o utils.o
	$(COMPILER) -I./src/include slave.o utils.o $(CFLAGS) -o $(OUTPUT_FILE_SLAVE)

slave.o: src/slave.c
	$(COMPILER) -I./src/include src/slave.c -c $(CFLAGS)

view: view.o shmAdt.o
	$(COMPILER) view.o shmAdt.o $(CFLAGS) -o $(OUTPUT_FILE_VIEW)

view.o: src/view.c
	$(COMPILER) -I./src/include src/view.c -c $(CFLAGS)

utils.o: src/utils/utils.c
	$(COMPILER) -I./src/include src/utils/utils.c -c $(CFLAGS)

shmAdt.o: src/utils/shmAdt.c
	$(COMPILER) -I./src/include src/utils/shmAdt.c -c $(CFLAGS)

debug: CFLAGS=$(DEBUG_COMPILER)
debug: all

clean:
	rm $(OUTPUT_FILE) $(OUTPUT_FILE_SLAVE) $(OUTPUT_FILE_VIEW) *.o
