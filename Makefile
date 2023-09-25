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

view: view2.o shmAdt.o
	$(COMPILER) view2.o shmAdt.o $(CFLAGS) -o $(OUTPUT_FILE_VIEW)

view2.o: src/view2.c
	$(COMPILER) -I./src/include src/view2.c -c $(CFLAGS)

utils.o: src/utils/utils.c
	$(COMPILER) -I./src/include src/utils/utils.c -c $(CFLAGS)

shmAdt.o: src/utils/shmAdt.c
	$(COMPILER) -I./src/include src/utils/shmAdt.c -c $(CFLAGS)

pvs:
	pvs-studio-analyzer trace -- make
	pvs-studio-analyzer analyze
	plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log

cleanpvs:
	rm -f strace_out PVS-Studio.log report.tasks

valgrind: all
	valgrind ./md5 files/* | ./view

files:
	mkdir -p files;
	number=1 ; while [ "$$number" -le 60 ] ; do \
        echo $$number > files/$$number ; \
        number=`expr $$number + 1` ; \
    done

cleanFiles:
	rm -rf files

debug: CFLAGS=$(DEBUG_COMPILER)
debug: all

clean:
	rm -f $(OUTPUT_FILE) $(OUTPUT_FILE_SLAVE) $(OUTPUT_FILE_VIEW) output.txt *.o
