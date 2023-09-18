# SO_TP1
System in C language which calculates md5 hashes of files using slaves.
This consist of three processes (app.c, view.c, and slave.c).

## Authors

- Gonzalo Candisano (62.616)
- Emilio Neme (62.601)
- Theo Shlamovitz (62.087)

## Content

- [SO TP1](#so_tp1)
  - [Authors](#authors)
  - [Table Content](#content)
  - [Project Structure](#project-structure)
  - [Requirements](#requirements)
  - [Compilation Instructions](#compilation-instructions)
  - [MakeFile](#makefile)
  - [Execution Instructions](#execution-instructions)
    - [Case 1](#case-1)
    - [Case 2](#case-2)
    - [Case 3](#case-3)
  - [Results](#results)
  - [MD5 of Project Files](#MD5-of-Project-Files)


## Project Structure

* app.c is the main process, it creates the slaves required in order to calculate all md5 hashes, it opens the shared memory required in order to communicate with the view.c file and it distributes the files to the slaves. 
* slave.c is the process that control the slaves. It listens in his stdout for a file path, then it creates a child process which calculates the md5 hash of the file and then it sends the result to the app process using stdout. Both input and output are redirected to the app process using pipes.
* view.c accesses the output of the application process in real time using the shared memory created by the app process
* include folder has the .h files for all the c files used, which have the function prototypes, the structs used, constants and the necesary includes.
* utils folder includes utils functions for files and file descriptors manipulations, and the Shared Memory ADT.


## Requirements
Programs should be compiled and run using the following docker container:
```
user@linux:~$ docker pull agodio/itba-so:2.0
user@linux:~$ cd ./path/to/SO_TP1
user@linux:~$ docker run -v ${PWD}:/root --privileged -ti --name SO agodio/itba-so:2.0
user@docker:~$ cd
```

## Compilation Instructions
In order to compile the program, inside the docker container run:
```
user@docker:~$ make
```

### MakeFile
The Makefile has the following options:
* <code>make</code> compiles the program
* <code>make debug</code> compiles the program with debug flags
* <code>make clean</code> cleans the object files and the executable
* <code>make files</code> creates the files used for testing
* <code>make cleanFiles</code> cleans the files used for testing
* <code>make valgrind</code> runs the program with valgrind
* <code>make pvs</code> runs the program with pvs-studio
* <code>make cleanpvs</code> cleans the pvs-studio files

## Execution Instructions
There are 3 cases for executing the program

### Case 1
If you want to just calculate the md5 hashes of files and saved the output to a file run:
```
user@docker:~$ ./md5 <files>
```

### Case 2
If you want to view the process in real time run:
```
user@docker:~$ ./md5 <files> | ./view
```

### Case 3
If you want to have case 2 in two different terminals run:

#### Terminal 1
```
user@docker:~$ ./md5 <files>
```
This program will output the shared memory name which *has to be used as argument for the view*.

#### Terminal 2
```
user@docker:~$ ./view <name1>
```

## Result
The results are stored in <code>output.txt</code>

## MD5 of Project Files
Using this project, we calculated the MD% hashes of the files in the project. The results are the following:
```
2d77bcaa77283f5174f113ebbc5bae1e  src/app.c
37a1c70a3cf6e125502e6ec7c11d482b  src/slave.c
c98d9ffe35188fbecd991014cb3d8323  src/view.c
cac5659c4ef17b0a0958a9d3f0627cb3  src/utils/shmAdt.c
1f7a13e600583183d7fb1c9a3f1d91be  src/utils/utils.c
```